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

    void start(); //�̳߳�������Ϊÿ���̷߳���runInThread������
    void stop();

    const std::string name() const { return name_; };
    size_t queueSize() const;

    void add(ThreadFunc tf);
private:
    void runInThread(); //�����߳���ִ������ĺ�������������л�ȡ��
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    std::string name_;
    ThreadFunc threadInitCallback_; //���������ص�����
    std::vector<std::unique_ptr<Thread> > threads_; //���������߳�
    std::deque<ThreadFunc> queue_; //�������
    bool running_;
    size_t threadSize_;
};

