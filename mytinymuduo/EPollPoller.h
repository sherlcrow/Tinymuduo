#ifndef EPOLLPOLLER_H
#define EPOLLPOLLER_H

#include <vector>
#include <sys/epoll.h>
#include <unistd.h>

#include "Logger.h"
#include "Poller.h"
#include "Timestamp.h"

/**
 * epoll_create
 * epoll_ctl
 * epoll_wait
 */

class EPollPoller :
    public Poller
{
    using EventList = std::vector<epoll_event>;
public:
    EPollPoller(EventLoop* Loop);
    ~EPollPoller() override;

    // ��д����Poller�ĳ��󷽷�
    Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;
private:
    // Ĭ�ϼ����¼�����
    static const int kInitEventListSize = 16;

    // ��д��Ծ������
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    // ����channelͨ���������ǵ�����epoll_ctl
    void update(int operation, Channel* channel);

    int epollfd_;       // epoll_create���ں˴����ռ䷵�ص�fd
    EventList events_;  // ���ڴ��epoll_wait���ص����з������¼����ļ�������
};
#endif

