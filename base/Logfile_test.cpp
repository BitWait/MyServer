#include "logfile.h"
#include "logging.h"
#include <memory>
#include <string>
#include <unistd.h>


std::unique_ptr<LogFile> g_logFile;

void outputFunc(const char* msg, int len)
{
	g_logFile->append(msg, len);
}

void flushFunc()
{
	g_logFile->flush();
}


int main(void)
{
	
	g_logFile.reset(new LogFile("/home/kux/work/base/logging.h", 200 * 1000));

	Logger::OutputFunc(outputFunc);
	Logger::setFlushFunc(flushFunc);

	std::string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	for (int i = 0; i < 10000; ++i)
	{
		LOG_INFO << line << i;

		usleep(1000);
	}
}