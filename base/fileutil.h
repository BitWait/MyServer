#pragma once

#include<string>

using namespace std;

namespace FileUtil
{
	//这个类主要是将文件内容读取出来
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
		int fd_;//缓冲区
		int err_;//错误代码
		char buf_[kBufferSize];//缓冲区

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

	//这个类主要是将数据写入到文件
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
		FILE *fp_;//打开的流
		char buf_[64 * 1024];//打开流的缓冲区
		size_t writtenBytes_;//已经添加的长度
	};
}
