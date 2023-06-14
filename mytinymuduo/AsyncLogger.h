#pragma once
#ifndef ASYNC_LOGGING_H
#define ASYNC_LOGGING_H

#include "noncopyable.h"
#include "Thread.h"
#include "FixedBuffer.h"
#include "LogStream.h"
#include "LogFile.h"


#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
/*
1. AsyncLogging �ṩ����̣߳���ʱ�� log ����д�����̣�ά�����弰������С�
2. LogFile �ṩ��־�ļ��������ܣ�д�ļ����ܡ�
3. AppendFile ��װ��OS �ṩ�Ļ�����д�ļ����ܡ�
*/
class AsyncLogging
{
public:
    AsyncLogging(const std::string& basename,
        off_t rollSize,
        int flushInterval = 3);
    ~AsyncLogging()
    {
        if (running_)
        {
            stop();
        }
    }

    // ǰ�˵��� append д����־
    void append(const char* logling, int len);

    void start()
    {
        running_ = true;
        thread_.start();
    }

    void stop()
    {
        running_ = false;
        cond_.notify_one();
        thread_.join();
    }
private:
    using Buffer = FixedBuffer<kLargeBuffer>;
    using BufferVector = std::vector<std::unique_ptr<Buffer>>;
    using BufferPtr = BufferVector::value_type;

    void threadFunc();

    //����ʱ��
    const int flushInterval_;
    std::atomic<bool> running_;
    //�ļ���
    const std::string basename_;
    //�����ļ���С
    const off_t rollSize_;
    //�첽�߳�
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;

    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
};
#endif

