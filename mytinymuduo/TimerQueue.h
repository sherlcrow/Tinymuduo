#pragma once
#ifndef TIMER_QUEUE_H
#define TIMER_QUEUE_H

#include "Timestamp.h"
#include "Channel.h"

#include <vector>
#include <set>

class EventLoop;
class Timer;
class TimerQueue
{
public:
    using TimerCallback = std::function<void()>;

    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    // ���붨ʱ�����ص�����������ʱ�䣬�Ƿ��ظ���
    void addTimer(TimerCallback cb,
        Timestamp when,
        double interval);
private:
	using Entry = std::pair<Timestamp, Timer*>; // ��ʱ�����ȡ��ʱ��
	using TimerList = std::set<Entry>; // �ײ�ʹ�ú������������ʱ�������

	//ʹ��runinloop���µĶ�ʱ���ŵ�Loop�У��̰߳�ȫ��
	void addTimerInLoop(Timer* timer);
    // ��ʱ�����¼������ĺ���
    void handleRead();

    // ��������timerfd_
    void resetTimerfd(int timerfd_, Timestamp expiration);

    // �Ƴ������ѵ��ڵĶ�ʱ��
    // 1.��ȡ���ڵĶ�ʱ��
    // 2.������Щ��ʱ�������ٻ����ظ���ʱ����
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);

    // ���붨ʱ�����ڲ�����
    bool insert(Timer* timer);

    EventLoop* loop_;           // ������EventLoop
    const int timerfd_;         // timerfd��Linux�ṩ�Ķ�ʱ���ӿ�
    Channel timerfdChannel_;    // ��װtimerfd_�ļ�������
    // Timer list sorted by expiration
    TimerList timers_;          // ��ʱ�����У��ڲ�ʵ���Ǻ������

    bool callingExpiredTimers_; // �������ڻ�ȡ��ʱ��ʱ��
};
#endif
