#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <sys/resource.h>

#include "../base/configfilereader.h"
#include "../base/logging.h"
#include "../base/timestamp.h"
#include "../base/asynclogging.h"
#include "../utils/DaemonRun.h"
#include "../base/singleton.h"
#include "../net/eventloop.h"
#include "../net/eventloopthreadpool.h"
#include "IMServer.h"

using namespace net;
EventLoop g_mainLoop;
AsyncLogging* g_asyncLog = NULL;
void asyncOutput(const char* msg, int len)
{
	if (g_asyncLog != NULL)
	{
		g_asyncLog->append(msg, len);
		std::cout << msg << std::endl;
	}
}

void prog_exit(int signo)
{
	std::cout << "program recv signal [" << signo << "] to exit." << std::endl;

	Singleton<EventLoopThreadPool>::Instance().stop();
	g_mainLoop.quit();

	//Logger::setOutput(defaultOutput);
}

int main(int argc, char* argv[])
{

	//设置信号处理
	signal(SIGCHLD, SIG_DFL);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, prog_exit);
	signal(SIGTERM, prog_exit);

	int ch;
	bool bdaemon = false;
	while ((ch = getopt(argc, argv, "d")) != -1)
	{
		switch (ch)
		{
		case 'd':
			bdaemon = true;
			break;
		}
	}

	if (bdaemon)
		daemon_run();

	CConfigFileReader config("../etc/chatserver.conf");
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
	Logger::setLoglevel(Logger::DEBUG);
	int kRollSize = 500 * 1000 * 1000;
	AsyncLogging log(strLogFileFullPath.c_str(), kRollSize);
	log.start();
	g_asyncLog = &log;
	Logger::setOutputFunc(asyncOutput);

	Singleton<EventLoopThreadPool>::Instance().Init(&g_mainLoop, 4);
	Singleton<EventLoopThreadPool>::Instance().start();

	const char* listenip = config.getConfigName("listenip");
	short listenport = (short)atol(config.getConfigName("listenport"));
	Singleton<IMServer>::Instance().Init(listenip, listenport, &g_mainLoop);

	g_mainLoop.loop();

	LOG_INFO << "exit chatserver.";

	return 0;

}