# pragma once

#include <string.h>
#include "timestamp.h"
#include "LogStream.h"

class Logger
{
public:
	enum LogLevel
	{
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		NUM_LOG_LEVELS,
	};
	
	class SourceFile
	{
	public:
		template<int N>
		inline SourceFile(const char(&arr)[N])
			:data_(arr),
			size_(N - 1)
		{
			const char* slash = strrchr(arr, '/');
			if (slash)
			{
				data_ = slash + 1;
				size_ -= static_cast<int>(data_ - arr);
			}
		}

		explicit SourceFile(const char* fileName)
			:data_(fileName)
		{
			const char* slash = strrchr(fileName, '/');
			if (slash)
			{
				data_ = slash + 1;
			}
			size_ = static_cast<int>(strlen(data_));
		}
		const char* data_;
		int size_;
	};
	Logger(SourceFile file, int line);
	Logger(SourceFile file, int line, LogLevel level);
	Logger(SourceFile file, int line, LogLevel level, const char* func);
	Logger(SourceFile file, int line, bool toAbort);
	~Logger();


	LogStream& stream(){ return impl_.stream_; }
	
	static LogLevel logLevel();
	static void setLoglevel(LogLevel logLevel);

	typedef void(*OutputFunc)(const char* msg, int len);
	typedef void(*FlushFunc)();
	static void setOutputFunc(OutputFunc);
	static void setFlushFunc(FlushFunc);

	void WriteLog(unsigned char* buffer, size_t size);
	
private:
	class Impl
	{
	public:
		typedef Logger::LogLevel LogLevel;
		Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
		void formatTime();
		void finish();
		Timestamp time_;
		LogStream stream_;
		LogLevel level_;
		int line_;
		SourceFile baseName_;
	};

	Impl impl_;
};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
	return g_logLevel;
}

#undef LOG_TRACE
#undef LOG_DEBUG
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERROR
#undef LOG_FATAL
#undef LOG_SYSERR

#define LOG_TRACE if (Logger::logLevel() <= Logger::TRACE) \
	Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (Logger::logLevel() <= Logger::DEBUG) \
	Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO if (Logger::logLevel() <= Logger::INFO) \
	Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#define  LOG_SYSERR Logger(__FILE__, __LINE__, false).stream()
#define  LOG_SYSFATAL Logger(__FILE__, __LINE__, true).stream()

#define LOG_DEBUG_BIN(x,l) if (Logger::logLevel() <= Logger::DEBUG) \
	Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).WriteLog((x), (l))

const char* strerror_tl(int savedErrno);

#define CHECK_NOTNULL(val) \
	CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

// A small helper for CHECK_NOTNULL().
template <typename T>
T* CheckNotNull(Logger::SourceFile file, int line, const char *names, T* ptr)
{
	if (ptr == NULL)
	{
		Logger(file, line, Logger::FATAL).stream() << names;
	}
	return ptr;
}