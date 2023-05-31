#include <semaphore.h>
#include "Thread.h"
#include "CurrentThread.h"

std::atomic_int32_t Thread::numCreated_(0);

Thread::Thread(ThreadFunc func, const std::string& name):
    started_(false), // ��δ��ʼ
    joined_(false),  // ��δ���õȴ��߳�
    tid_(0),         // ��ʼ tid ����Ϊ0
    func_(std::move(func)), // EventLoopThread::threadFunc()
    name_(name)     // Ĭ�������ǿ��ַ���
{
    setDefaultName();
}

Thread::~Thread()
{
    //�߳��Ѿ���������δ���õȴ��߳�
    if (started_ && !joined_)
    {
        // �����̷߳���(�ػ��̣߳�����Ҫ�ȴ��߳̽�������������¶��߳�)
        thread_->detach();
    }
}

void Thread::start()
{
    started_ = true;
    sem_t sem; //�ź���
    sem_init(&sem, false, 0);
    thread_ = std::shared_ptr<std::thread>(
        new std::thread([&] {
            tid_ = CurrentThread::tid();
            sem_post(&sem);
            func_();
            })
        );
    /**
    * �������ȴ���ȡ�����´������̵߳�tid
    * δ��ȡ����Ϣ�򲻻�ִ��sem_post�����Իᱻ����ס
    * �����ʹ���ź��������������̷߳���tidʱ�򣬿���������̻߳�û�л�ȡ��tid
    */
    sem_wait(&sem);
}

void Thread::join()
{
    joined_ = true;
    thread_->join();
}

void Thread::setDefaultName()
{
    int num = ++numCreated_;
    if (name_.empty())
    {
        char buf[32] = { 0 };
        snprintf(buf, sizeof(buf), "Thread%d", num);
        name_ = buf;
    }
}
