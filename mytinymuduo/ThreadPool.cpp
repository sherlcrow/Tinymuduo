#include "ThreadPool.h"

ThreadPool::ThreadPool(const std::string& name)
    : mutex_(),
    cond_(),
    name_(name),
    running_(false)
{
}

ThreadPool::~ThreadPool()
{
    stop();
    for (auto &val : threads_) //必须用&，因为threads保存的是unique_ptr,不能拷贝
    {
        val->join();
    }
}

void ThreadPool::start()
{
    running_ = true;
    threads_.reserve(threadSize_);
    for (int i = 0; i < threadSize_; ++i)
    {
        std::string id = std::to_string(i + 1);
        threads_.emplace_back(new Thread(
            std::bind(&ThreadPool::runInThread, this), name_ + id
        ));
        threads_[i]->start();
    }
    //线程池为空
    if (threadSize_ == 0 && threadInitCallback_)
        threadInitCallback_();
}

void ThreadPool::stop()
{
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
    cond_.notify_all();
}

size_t ThreadPool::queueSize() const
{
    std::lock_guard<std::mutex> lock(mutex_);
	return queue_.size();
}

void ThreadPool::add(ThreadFunc tf)
{
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_back(tf);
    cond_.notify_one();
}

void ThreadPool::runInThread()
{
    if (threadInitCallback_)
        threadInitCallback_();
    ThreadFunc tf;
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (queue_.empty())
            {
                if (!running_)
                    return;
                cond_.wait(lock);
            }
            tf = queue_.front();
            queue_.pop_front();
        }
        if (tf)
            tf();
    }
}
