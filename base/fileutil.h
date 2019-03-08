#pragma once

#include<string>

using namespace std;

namespace FileUtil
{
	class ReadSmallFile
	{
	public:
		ReadSmallFile(const string & fileName);
		~ReadSmallFile();
		template<typename String>
		int readToString(int maxSize,
			String *content,
			int64_t * fileSize,
			int64_t * modifyTime,
			int64_t * creatTime);
		int readToBuffer(int *size);

		const char * buffer()const{ return buf_; }
		static const int kBufferSize = 64 * 1024;
	private:
		int fd_;
		int err_;
		char buf_[kBufferSize];

	};

	template<typename String>
	int readFile(const string &fileName,
		int maxSize,
		String *content,
		int64_t * fileSize = NULL,
		int64_t * modifyTime = NULL,
		int64_t * createTime = NULL)
	{
		ReadSmallFile file(fileName);
		return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
	}

	class AppendFile
	{
	public:
		explicit AppendFile(const string &fileName);
		~AppendFile();

		void append(const char * logLine, const size_t len);
		void flush();
		size_t writtenBytes()const{ return writtenBytes_; }

		size_t write(const char* logline, size_t len);
	private:
		FILE *fp_;
		char buf_[64 * 1024];
		size_t writtenBytes_;
	};
}
