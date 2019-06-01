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
	//����Ĺ��캯����һ���ļ�������Ϊ��������û�������
	assert(fp_);
	::setbuffer(fp_, buf_, sizeof buf_);
}

FileUtil::AppendFile::~AppendFile()
{
	::fclose(fp_);
}

//ִ��ʵ�ʵ�д������ͬʱȷ��������ȫд��
void FileUtil::AppendFile::append(const char * logLine, const size_t len)
{
	size_t n = write(logLine, len);
	size_t remain = len - n;
	//���û��д�꣬��ôѭ������д��
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

//�ֶ�������������д�뵽�ļ�
void FileUtil::AppendFile::flush()
{
	::fflush(fp_);
}

//ִ��������д���ݵĲ���������ֱ�ӵ��øú���
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
			//����ļ���Ϣ
			if (::fstat(fd_, &statbuf) == 0)
			{
				//�������ͨ�ļ���
				if (S_ISREG(statbuf.st_mode))
				{
					*fileSize = statbuf.st_size;
					//���������Ĵ�С���ļ��Ĵ�С
					content->reserve(static_cast<int>(std::min(static_cast<int64_t>(maxSize), *fileSize)));
				}
				//������ļ���
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
		//�ļ���Ϣ�����Ժ󣬿�ʼ���ļ������ݶ�ȡ�������Ľṹcontext�С�
		while (content->size() < static_cast<size_t>(maxSize))
		{
			//ÿ�ζ�ȡ�Ĵ�С�ǣ�ʣ�����ͻ������Ĵ�С��
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
