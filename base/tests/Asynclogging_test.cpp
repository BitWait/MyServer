#include "../asynclogging.h"
#include "../logging.h"
#include "../timestamp.h"

#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

off_t kRollSize = 500 * 1000 * 1000;

AsyncLogging* g_asyncLog = NULL;

void asyncOutput(const char* msg, int len)
{
	g_asyncLog->append(msg, len);
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

	printf("pid = %d\n", getpid());

	AsyncLogging log("/home/kux/work/base/logging.h", kRollSize);

}
