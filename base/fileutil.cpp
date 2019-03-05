#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <inttypes.h>


#include "fileutil.h"

FileUtil::AppendFile::AppendFile(const string &fileName)
:fd_(::fopen(fileName.c_str(), "ae")), writtenBytes_(0)
{
	assert(fp_);
	::setbuffer(fp_, buffer_, sizeof buffer_);
}

FileUtil::AppendFile::~AppendFile()
{
	::fclose(fp_);
}