#pragma once
#ifndef LOGGING_H
#define LOGGING_H

#include "Timestamp.h"
#include "LogStream.h"

#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <functional>
//将带有路径的文件名去除路径
class sourcefile
{
public:
	sourcefile(const char* filename) : data_(filename) {
		//最后一次出现/的位置
		const char* slash = strrchr(filename, '/');
		if (slash)
			data_ = slash + 1;
		size_ = static_cast<int>(strlen(data_));
	}
	const char* data_;
	int size_;
};
class Logger
{
	//将所有信息输出到stream流，在析构时打印
public:
	enum LogLevel
	{
		TRACE = 0,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		LEVEL_COUNT,
	};
	
	Logger(const char* file, int line);
	Logger(const char* file, int line, LogLevel level);
	Logger(const char* file, int line, LogLevel level, const char* func);
	~Logger();

	// 流是会改变的
	LogStream& stream() { return impl_.stream_; }

	// TODO:static关键字 静态成员函数没有this指针且只有一份，直接在外部用Logger::set*修改即可
	static void setLogLevel(LogLevel level);

	// 输出函数和刷新缓冲区函数
	using OutputFunc = std::function<void(const char* msg, int len)>;
	using FlushFunc = std::function<void()>;
	static void setOutput(OutputFunc);
	static void setFlush(FlushFunc);
private:
	//内部类
	class Impl
	{
	public:
		using LogLevel = Logger::LogLevel;
		Impl(LogLevel level, int savedError, const char* file, int line);
		void formatTime(); //将当前时间格式化打印
		void finish();

		Timestamp time_; //当前时间
		LogStream stream_; //输出流
		LogLevel level_; //日志等级
		int line_; 
		sourcefile basename_;
	};

	//Logger类的操作均借助Impl类来实现；
	Impl impl_;
};

// 获取errno信息
const char* getErrnoMsg(int savedErrno);

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel logLevel()
{
	return g_logLevel;
}

/**
 * 当日志等级小于对应等级才会输出
 * 比如设置等级为FATAL，则logLevel等级大于DEBUG和INFO，DEBUG和INFO等级的日志就不会输出
 * 每个宏定义都会构造出一个 Logger 的临时对象，然后输出相关信息。
 * 在 Logger::Impl 的构造函数中会初始化时间戳、线程ID、日志等级这类固定信息，而正文消息靠 LogStream 重载实现。
 * 在 Logger 临时对象销毁时，会调用 Logger 的析构函数，其内部会将日志信息输出到指定位置。
 * 而 Logger 的实现文件中定义了两个全局函数指针，其执行的函数会确定日志信息的输出位置。
 */
//_FILE_和_LINE_宏代表当前文件名和行号
#define LOG_DEBUG if (logLevel() <= Logger::DEBUG) \
  Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO if (logLevel() <= Logger::INFO) \
  Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#endif

