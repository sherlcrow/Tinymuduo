#include "Poller.h"
#include "EPollPoller.h"
#include "PollPoller.h"

#include <stdlib.h>
// ��ȡĬ�ϵ�Pollerʵ�ַ�ʽ
Poller* Poller::newDefaultPoller(EventLoop* loop)
{
    // ��ȡ��������
    if (::getenv("MUDUO_USE_POLL"))
    {
        return new PollPoller(loop);// ����pollʵ��
    }
    else
    {
        return new EPollPoller(loop); // ����epollʵ��
    }
}