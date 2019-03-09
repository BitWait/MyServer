#include "logfile.h"
#include <memory>
#include <string.h>
#include <unistd.h>

std::unique_ptr<LogFile> g_logFile;
int main(int argc, char *argv[])
{
	
	g_logFile.reset(new LogFile("/home/kux/work/base", 200 * 1000));

}