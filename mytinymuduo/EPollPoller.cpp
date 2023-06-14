#include "EPollPoller.h"
#include <string.h>

const int kNew = -1;    // 某个channel还没添加至Poller          // channel的成员index_初始化为-1
const int kAdded = 1;   // 某个channel已经添加至Poller
const int kDeleted = 2; // 某个channel已经从Poller删除

EPollPoller::EPollPoller(EventLoop* loop)
	: Poller(loop),
	epollfd_(::epoll_create(EPOLL_CLOEXEC)),
	events_(kInitEventListSize)
{
	if (epollfd_ < 0)
		LOG_FATAL << "epoll_creat() error:" << errno;
}

EPollPoller::~EPollPoller()
{
	::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
	size_t numEvents = ::epoll_wait(epollfd_, &(*events_.begin()),
		static_cast<int>(events_.size()), timeoutMs);
	int saveErrno = errno;
	Timestamp now(Timestamp::now());
	if (numEvents > 0)
	{
		//填充活跃的channels，把epoll_wait传出的events传给vector数组activechannels
		fillActiveChannels(numEvents, activeChannels); 
		//扩容，evevts_是准备好接收注册事件的容器，根据epoll_wait的传入参数
		//最多出现events_被填满的情况，即此时numEvents==events_的大小
		if (numEvents == events_.size())
			events_.resize(events_.size() * 2);
	}
	else if (numEvents == 0)
	{
		//超过规定时间仍然没有收到连接请求
		LOG_DEBUG << "timeout";
	}
	else
	{
		if (saveErrno != EINTR)
		{
			errno = saveErrno;
			LOG_ERROR << "EPollPoller::poll() failed";
		}
	}
	return now;
}

void EPollPoller::updateChannel(Channel* channel)
{
	// 获取参数channel在epoll的状态
	const int index = channel->index();

	// 未添加状态和已删除状态都有可能会被再次添加到epoll中
	if (index == kNew || index == kDeleted)
	{
		// 添加到键值对 
		if (index == kNew)
		{
			int fd = channel->fd();
			channels_[fd] = channel;
		}
		else // index == kAdd
		{
		}
		// 修改channel的状态，此时是已添加状态
		channel->set_index(kAdded);
		// 向epoll对象加入channel
		update(EPOLL_CTL_ADD, channel);
	}
	// channel已经在poller上注册过
	else
	{
		// 没有感兴趣事件说明可以从epoll对象中删除该channel了
		if (channel->isNoneEvent())
		{
			update(EPOLL_CTL_DEL, channel);
			channel->set_index(kDeleted);
		}
		// 还有事件说明之前的事件删除，但是被修改了
		else
		{
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

// 填写活跃的连接
// 函数尾部加const表明函数内部不能对变量做修改
void EPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
	for (int i = 0; i < numEvents; i++)
	{
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
		channel->set_revents(events_[i].events);
		activeChannels->push_back(channel);
	}
}

void EPollPoller::removeChannel(Channel* channel)
{
	// 从Map中删除
	int fd = channel->fd();
	channels_.erase(fd);


	int index = channel->index();
	if (index == kAdded)
	{
		// 如果此fd已经被添加到Poller中，则还需从epoll对象中删除
		update(EPOLL_CTL_DEL, channel);
	}
	// 重新设置channel的状态为未被Poller注册
	channel->set_index(kNew);
}

void EPollPoller::update(int operation, Channel* channel)
{
	epoll_event event;
	::memset(&event, 0, sizeof(event));

	int fd = channel->fd();
	event.events = channel->events();
	event.data.fd = fd;
	event.data.ptr = channel;

	if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
	{
		if (operation == EPOLL_CTL_DEL)
		{
			LOG_ERROR << "epoll_ctl del error:" << errno;
		}
		else
		{
			LOG_FATAL << "epoll_ctl add/mod error:" << errno;
		}
	}
}