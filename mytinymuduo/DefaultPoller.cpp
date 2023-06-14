#include "Poller.h"
#include "EPollPoller.h"
#include "PollPoller.h"

#include <stdlib.h>
// 获取默认的Poller实现方式
Poller* Poller::newDefaultPoller(EventLoop* loop)
{
    // 获取环境变量
    if (::getenv("MUDUO_USE_POLL"))
    {
        return new PollPoller(loop);// 生成poll实例
    }
    else
    {
        return new EPollPoller(loop); // 生成epoll实例
    }
}