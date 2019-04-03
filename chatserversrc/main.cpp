#include "../base/configfilereader.h"
#include "../base/logging.h"
#include "../base/timestamp.h"

#include "../base/asynclogging.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <stdio.h>
#include <sys/resource.h>
#include <iostream>

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

void bench(bool longLog)
{
	Logger::setOutputFunc(asyncOutput);

	int cnt = 0;
	const int kBatch = 1000;
	std::string empty = " ";
	std::string longStr(3000, 'X');
	longStr += " ";

	for (int t = 0; t < 30; ++t)
	{
		Timestamp start = Timestamp::now();
		for (int i = 0; i < kBatch; ++i)
		{
			LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
				<< (longLog ? longStr : empty)
				<< cnt;
			++cnt;
		}
		Timestamp end = Timestamp::now();
		printf("%f\n", timeDifference(end, start) * 1000000 / kBatch);
		struct timespec ts = { 0, 500 * 1000 * 1000 };
		nanosleep(&ts, NULL);
	}
}
int main(void)
{
	CConfigFileReader config("mychatserver.conf");
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
	bench(true);


}