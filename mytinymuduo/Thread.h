#pragma once
#ifndef THREAD_H
#define THREAD_H

#include <thread>
#include <functional>
#include <memory>
#include <string>
#include <atomic>
#include "noncopyable.h"
class Thread :
    public noncopyable
{
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(ThreadFunc func, const std::string& name = std::string()); //�����const����

    ~Thread();

    void start(); //��ʼ�߳�
    void join(); //�ȴ��߳�

    bool started() const { return started_; };
    pid_t tid() const { return tid_; };
    const std::string& name() const { return name_; }

    static int numCreated() { return numCreated_; }
private:
    void setDefaultName(); //�����߳���

    bool started_; //�Ƿ������߳�
    bool joined_; //�Ƿ�ȴ����߳�
    std::shared_ptr<std::thread> thread_; //��sharedptrָ���߳�����
    pid_t tid_; //�߳�tid
    ThreadFunc func_; //�ص�����
    std::string name_; //�߳���
    static std::atomic_int32_t numCreated_; //�߳�����
};
#endif

