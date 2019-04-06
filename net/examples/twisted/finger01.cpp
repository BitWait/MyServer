#include "../../../base/configfilereader.h"
#include "../../../base/logging.h"
#include "../../../base/timestamp.h"

#include "../../../base/asynclogging.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <stdio.h>
#include <sys/resource.h>
#include <iostream>

#include "../../eventloop.h"
#include "../../tcpserver.h"
#include "../../../base/singleton.h"
#include "../../eventloopthreadpool.h"
using namespace net;

using namespace std;

AsyncLogging* g_asyncLog = NULL;
void asyncOutput(const char* msg, int len)
{
	if (g_asyncLog != NULL)
	{
		g_asyncLog->append(msg, len);
		std::cout << msg << std::endl;
	}
}


void onMessage(const TcpConnectionPtr& conn,
	Buffer* buf,
	Timestamp receiveTime)
{

	if (buf->findCRLF())
	{
		conn->send("No such user\r\n");
		conn->shutdown();
	}
}

int main(void)
{
	CConfigFileReader config("chatserver.conf");
	const char* logfilepath = config.getConfigName("logfiledir");
	if (logfilepath == NULL)
	{
		LOG_SYSFATAL << "logdir is not set in config file";
		return 1;
	}
	else
	{
		LOG_INFO << logfilepath;
	}

	//	如果log目录不存在则创建之
	DIR* dp = opendir(logfilepath);
	if (dp == NULL)
	{
		if (mkdir(logfilepath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
		{
			LOG_SYSFATAL << "create base dir error, " << logfilepath << ", errno: " << errno << ", " << strerror(errno);
			return 1;
		}
	}
	closedir(dp);

	const char* logfilename = config.getConfigName("logfilename");
	if (logfilename == NULL)
	{
		LOG_SYSFATAL << "logfilename is not set in config file";
		return 1;
	}
	else
	{

		LOG_INFO << logfilename;
	}

	std::string strLogFileFullPath(logfilepath);
	strLogFileFullPath += logfilename;
	Logger::setLoglevel(Logger::TRACE);
	int kRollSize = 500 * 1000 * 1000;
	AsyncLogging log(strLogFileFullPath.c_str(), kRollSize);
	log.start();
	g_asyncLog = &log;
	Logger::setOutputFunc(asyncOutput);

	EventLoop loop;
	TcpServer server(&loop, InetAddress(1079), "Finger");
	server.setMessageCallBack(onMessage);
	Singleton<EventLoopThreadPool>::Instance().Init(&loop, 0);
	Singleton<EventLoopThreadPool>::Instance().start();

	server.start();
	loop.loop();


}
