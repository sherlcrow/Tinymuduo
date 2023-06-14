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
//������·�����ļ���ȥ��·��
class sourcefile
{
public:
	sourcefile(const char* filename) : data_(filename) {
		//���һ�γ���/��λ��
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
	//��������Ϣ�����stream����������ʱ��ӡ
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

	// ���ǻ�ı��
	LogStream& stream() { return impl_.stream_; }

	// TODO:static�ؼ��� ��̬��Ա����û��thisָ����ֻ��һ�ݣ�ֱ�����ⲿ��Logger::set*�޸ļ���
	static void setLogLevel(LogLevel level);

	// ���������ˢ�»���������
	using OutputFunc = std::function<void(const char* msg, int len)>;
	using FlushFunc = std::function<void()>;
	static void setOutput(OutputFunc);
	static void setFlush(FlushFunc);
private:
	//�ڲ���
	class Impl
	{
	public:
		using LogLevel = Logger::LogLevel;
		Impl(LogLevel level, int savedError, const char* file, int line);
		void formatTime(); //����ǰʱ���ʽ����ӡ
		void finish();

		Timestamp time_; //��ǰʱ��
		LogStream stream_; //�����
		LogLevel level_; //��־�ȼ�
		int line_; 
		sourcefile basename_;
	};

	//Logger��Ĳ���������Impl����ʵ�֣�
	Impl impl_;
};

// ��ȡerrno��Ϣ
const char* getErrnoMsg(int savedErrno);

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel logLevel()
{
	return g_logLevel;
}

/**
 * ����־�ȼ�С�ڶ�Ӧ�ȼ��Ż����
 * �������õȼ�ΪFATAL����logLevel�ȼ�����DEBUG��INFO��DEBUG��INFO�ȼ�����־�Ͳ������
 * ÿ���궨�嶼�ṹ���һ�� Logger ����ʱ����Ȼ����������Ϣ��
 * �� Logger::Impl �Ĺ��캯���л��ʼ��ʱ������߳�ID����־�ȼ�����̶���Ϣ����������Ϣ�� LogStream ����ʵ�֡�
 * �� Logger ��ʱ��������ʱ������� Logger ���������������ڲ��Ὣ��־��Ϣ�����ָ��λ�á�
 * �� Logger ��ʵ���ļ��ж���������ȫ�ֺ���ָ�룬��ִ�еĺ�����ȷ����־��Ϣ�����λ�á�
 */
//_FILE_��_LINE_�����ǰ�ļ������к�
#define LOG_DEBUG if (logLevel() <= Logger::DEBUG) \
  Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO if (logLevel() <= Logger::INFO) \
  Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#endif

