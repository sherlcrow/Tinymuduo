#ifndef POLLER_H
#define POLLER_H

#include "noncopyable.h"
#include "Channel.h"
#include "Timestamp.h"

#include <vector>
#include <unordered_map>

// muduo���ж�·�¼��ַ����ĺ���IO����ģ��
// ������Ϊpoll��epoll
class Poller :
    public noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* Loop) : ownerLoop_(Loop) {};
    //�麯������Ϊ����
    virtual ~Poller() = default;

    void assertInLoopThread() const
    {
        //ownerLoop_->assertInLoopThread();
    }

    /*
    * ��Ҫ����������ʵ�ֵĽӿ�
    * ���ڼ�������Ȥ���¼���fd(��װ����channel)
    * ����Poller��poll������EPollerPoller��epoll_wait
    * ��󷵻�epoll_wait/poll�ķ���ʱ��
    */
    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

    // ��Channel����ʱ�Ƴ���Channel
    virtual void updateChannel(Channel* channel) = 0;

    // ��Channel����ʱ�Ƴ���Channel
    virtual void removeChannel(Channel* channel) = 0;

    // �ж� channel�Ƿ�ע�ᵽ poller����
    bool hasChannel(Channel* channel) const
    {
        auto it = channels_.find(channel->fd());
        return  it != channels_.end()
            && it->second == channel;
    };

    // EventLoop����ͨ���ýӿڻ�ȡĬ�ϵ�IO����ʵ�ַ�ʽ(Ĭ��epoll)
    /**
     * ����ʵ�ֲ����� Poller.cc �ļ���
     * ���Ҫʵ�������Ԥ�ϻ�������EPollPoller PollPoller
     * ��ô����ͻ��ڻ��������������ͷ�ļ�������������ƾͲ���
     * ��Ϊʵ�ָú�����Ҫ�ֱ����poll��epoll���ͷ�ļ�
     * ���д��poller.cpp�ļ��лᵼ�»���poller����������poll��epoll
     * ��������ᵥ������һ�� DefaultPoller.cc ���ļ�ȥʵ��
     */
    static Poller* newDefaultPoller(EventLoop* Loop);
protected:
    //protected ������ʹ�ã��̳��������ʹ��
    using ChannelMap = std::unordered_map<int, Channel*>;
    // ���� channel ��ӳ�䣬��sockfd -> channel*��
    ChannelMap channels_;

private:
    EventLoop* ownerLoop_; // ����Poller�������¼�ѭ��EventLoop
    // һ���߳�һ��Evevntloop��Ӧһ��Poller�����汣����channels
};
#endif
