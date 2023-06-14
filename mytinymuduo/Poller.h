#ifndef POLLER_H
#define POLLER_H

#include "noncopyable.h"
#include "Channel.h"
#include "Timestamp.h"

#include <vector>
#include <unordered_map>

// muduo库中多路事件分发器的核心IO复用模块
// 派生类为poll和epoll
class Poller :
    public noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* Loop) : ownerLoop_(Loop) {};
    //虚函数设置为析构
    virtual ~Poller() = default;

    void assertInLoopThread() const
    {
        //ownerLoop_->assertInLoopThread();
    }

    /*
    * 需要交给派生类实现的接口
    * 用于监听感兴趣的事件和fd(封装成了channel)
    * 对于Poller是poll，对于EPollerPoller是epoll_wait
    * 最后返回epoll_wait/poll的返回时间
    */
    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

    // 当Channel销毁时移除此Channel
    virtual void updateChannel(Channel* channel) = 0;

    // 当Channel销毁时移除此Channel
    virtual void removeChannel(Channel* channel) = 0;

    // 判断 channel是否注册到 poller当中
    bool hasChannel(Channel* channel) const
    {
        auto it = channels_.find(channel->fd());
        return  it != channels_.end()
            && it->second == channel;
    };

    // EventLoop可以通过该接口获取默认的IO复用实现方式(默认epoll)
    /**
     * 它的实现并不在 Poller.cc 文件中
     * 如果要实现则可以预料会其会包含EPollPoller PollPoller
     * 那么外面就会在基类引用派生类的头文件，这个抽象的设计就不好
     * 因为实现该函数需要分别包含poll和epoll类的头文件
     * 如果写在poller.cpp文件中会导致基类poller包含派生类poll和epoll
     * 所以外面会单独创建一个 DefaultPoller.cc 的文件去实现
     */
    static Poller* newDefaultPoller(EventLoop* Loop);
protected:
    //protected 对象不能使用，继承子类可以使用
    using ChannelMap = std::unordered_map<int, Channel*>;
    // 储存 channel 的映射，（sockfd -> channel*）
    ChannelMap channels_;

private:
    EventLoop* ownerLoop_; // 定义Poller所属的事件循环EventLoop
    // 一个线程一个Evevntloop对应一个Poller，里面保存多个channels
};
#endif
