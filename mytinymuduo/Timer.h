#pragma once
#ifndef TIMER_H
#define TIMER_H

#include "noncopyable.h"
#include "Timestamp.h"
#include <functional>

/**
 * Timer��������һ����ʱ��
 * ��ʱ���ص���������һ�γ�ʱʱ�̣��ظ���ʱ����ʱ������
 */
class Timer : noncopyable
{
public:
	using TimerCallback = std::function<void()>;
    Timer(TimerCallback cb, Timestamp when, double interval)
        : callback_(move(cb)),
        expiration_(when),
        interval_(interval),
        repeat_(interval > 0.0) // һ���Զ�ʱ������Ϊ0
    {
    }

    void run() const
    {
        callback_();
    }

    Timestamp expiration() const { return expiration_; }
    bool repeat() const { return repeat_; }

    // ������ʱ��(����Ƿ��ظ��¼�����ʱ����Ϊ0)
    void restart(Timestamp now)
    {
        if (repeat_)
        {
            // ������ظ���ʱ�¼����������Ӷ�ʱ�¼����õ����¼������¼�
            expiration_ = addTime(now, interval_);
        }
        else
        {
            expiration_ = Timestamp();
        }
    }

private:
    const TimerCallback callback_;  // ��ʱ���ص�����
    Timestamp expiration_;          // ��һ�εĳ�ʱʱ��
    const double interval_;         // ��ʱʱ�����������һ���Զ�ʱ������ֵΪ0
    const bool repeat_;             // �Ƿ��ظ�(false ��ʾ��һ���Զ�ʱ��)
};
#endif

