#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <inttypes.h>

#include "fileutil.h"

FileUtil::AppendFile::AppendFile(const string &fileName)
:fp_(::fopen(fileName.c_str(), "ae")), writtenBytes_(0)
{
	//该类的构造函数打开一个文件流，并为这个流设置缓冲区。
	assert(fp_);
	::setbuffer(fp_, buf_, sizeof buf_);
}

FileUtil::AppendFile::~AppendFile()
{
	::fclose(fp_);
}

//执行实际的写操作，同时确保数据完全写入
void FileUtil::AppendFile::append(const char * logLine, const size_t len)
{
	size_t n = write(logLine, len);
	size_t remain = len - n;
	//如果没有写完，那么循环往里写入
	while (remain > 0)
	{
		size_t x = write(logLine + n, remain);
		if(x == 0)
		{
			int err = ferror(fp_);
			if (err)
			{
				fprintf(stderr, "AppendFile::append() failed\n");
			}
			break;
		}
		n += x;
		remain = len - n;
	}
	writtenBytes_ += len;
}

//手动将缓冲区数据写入到文件
void FileUtil::AppendFile::flush()
{
	::fflush(fp_);
}

//执行向流中写数据的操作，但不直接调用该函数
size_t FileUtil::AppendFile::write(const char* logline, size_t len)
{
	return ::fwrite_unlocked(logline, 1, len, fp_);
}

FileUtil::ReadSmallFile::ReadSmallFile(const string&filename)
:fd_(::open(filename.c_str(), O_RDONLY | O_CLOEXEC)),
err_(0)
{
	buf_[0] = '\0';
	if (fd_ < 0)
	{
		err_ = errno;
	}
}

FileUtil::ReadSmallFile::~ReadSmallFile()
{
	if (fd_ >= 0)
	{
		::close(fd_);
	}
}

template<typename String>
int FileUtil::ReadSmallFile::readToString(int maxSize,
	String *content,
	int64_t * fileSize,
	int64_t * modifyTime,
	int64_t * createTime)
{
	static_assert(sizeof(off_t) == 8, "sizeof(off_t) != 8");
	assert(content != NULL);
	int err = err_;

	if (fd_ >= 0)
	{
		content->clear();
		if (fileSize)
		{
			struct stat statbuf;
			//填充文件信息
			if (::fstat(fd_, &statbuf) == 0)
			{
				//如果是普通文件。
				if (S_ISREG(statbuf.st_mode))
				{
					*fileSize = statbuf.st_size;
					//调整容器的大小到文件的大小
					content->reserve(static_cast<int>(std::min(static_cast<int64_t>(maxSize), *fileSize)));
				}
				//如果是文件夹
				else if (S_ISDIR(statbuf.st_mode))
				{
					err = EISDIR;
				}
				if (modifyTime)
				{
					*modifyTime = statbuf.st_mtime;
				}
				if (createTime)
				{
					*createTime = statbuf.st_ctime;
				}
			}
			else
			{
				err = errno;
			}
		}
		//文件信息填充好以后，开始将文件的内容读取到给定的结构context中。
		while (content->size() < static_cast<size_t>(maxSize))
		{
			//每次读取的大小是，剩余最大和缓冲区的大小。
			size_t toRead = std::min(static_cast<size_t>(maxSize)-content->size(), sizeof(buf_));
			ssize_t n = ::read(fd_, buf_, toRead);
			if (n > 0)
			{
				content->append(buf_, n);
			}
			else
			{
				if (n < 0)
				{
					err = errno;
				}
				break;
			}
		}
	}
	return err;
}

int FileUtil::ReadSmallFile::readToBuffer(int* size)
{
	int err = err_;
	if (fd_ >= 0)
	{
		ssize_t n = ::pread(fd_, buf_, sizeof(buf_)-1, 0);
		if (n >= 0)
		{
			if (size)
			{
				*size = static_cast<int>(n);
			}
			buf_[n] = '\0';
		}
		else
		{
			err = errno;
		}
	}
	return err;
}

template int FileUtil::readFile(const string& filename,
	int maxSize,
	string* content,
	int64_t*, int64_t*, int64_t*);

template int FileUtil::ReadSmallFile::readToString(
	int maxSize,
	string* content,
	int64_t*, int64_t*, int64_t*);
