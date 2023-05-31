#pragma once
#include "noncopyable.h"
#include "Thread.h"

#include <deque>
#include <vector>
#include <mutex>
#include <condition_variable>
class ThreadPool :
    public noncopyable
{
public:
    using ThreadFunc = std::function<void()>;
    explicit ThreadPool(const std::string& name = std::string("ThreadPool"));
    ~ThreadPool();

    void setThreadInitCallback(const ThreadFunc& cb){ threadInitCallback_ = std::move(cb); };
    void setThreadSize(const int& num) { threadSize_ = num; };

    void start(); //线程池启动，为每个线程分配runInThread任务函数
    void stop();

    const std::string name() const { return name_; };
    size_t queueSize() const;

    void add(ThreadFunc tf);
private:
    void runInThread(); //单个线程内执行任务的函数（从任务队列获取）
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    std::string name_;
    ThreadFunc threadInitCallback_; //启动触发回调函数
    std::vector<std::unique_ptr<Thread> > threads_; //保存所有线程
    std::deque<ThreadFunc> queue_; //任务队列
    bool running_;
    size_t threadSize_;
};

