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
1. AsyncLogging 提供后端线程，定时将 log 缓冲写到磁盘，维护缓冲及缓冲队列。
2. LogFile 提供日志文件滚动功能，写文件功能。
3. AppendFile 封装了OS 提供的基础的写文件功能。
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

    // 前端调用 append 写入日志
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

    //休眠时间
    const int flushInterval_;
    std::atomic<bool> running_;
    //文件名
    const std::string basename_;
    //滚动文件大小
    const off_t rollSize_;
    //异步线程
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;

    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
};
#endif

