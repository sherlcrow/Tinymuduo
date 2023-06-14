#include "EPollPoller.h"
#include <string.h>

const int kNew = -1;    // ĳ��channel��û�����Poller          // channel�ĳ�Աindex_��ʼ��Ϊ-1
const int kAdded = 1;   // ĳ��channel�Ѿ������Poller
const int kDeleted = 2; // ĳ��channel�Ѿ���Pollerɾ��

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
		//����Ծ��channels����epoll_wait������events����vector����activechannels
		fillActiveChannels(numEvents, activeChannels); 
		//���ݣ�evevts_��׼���ý���ע���¼�������������epoll_wait�Ĵ������
		//������events_�����������������ʱnumEvents==events_�Ĵ�С
		if (numEvents == events_.size())
			events_.resize(events_.size() * 2);
	}
	else if (numEvents == 0)
	{
		//�����涨ʱ����Ȼû���յ���������
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
	// ��ȡ����channel��epoll��״̬
	const int index = channel->index();

	// δ���״̬����ɾ��״̬���п��ܻᱻ�ٴ���ӵ�epoll��
	if (index == kNew || index == kDeleted)
	{
		// ��ӵ���ֵ�� 
		if (index == kNew)
		{
			int fd = channel->fd();
			channels_[fd] = channel;
		}
		else // index == kAdd
		{
		}
		// �޸�channel��״̬����ʱ�������״̬
		channel->set_index(kAdded);
		// ��epoll�������channel
		update(EPOLL_CTL_ADD, channel);
	}
	// channel�Ѿ���poller��ע���
	else
	{
		// û�и���Ȥ�¼�˵�����Դ�epoll������ɾ����channel��
		if (channel->isNoneEvent())
		{
			update(EPOLL_CTL_DEL, channel);
			channel->set_index(kDeleted);
		}
		// �����¼�˵��֮ǰ���¼�ɾ�������Ǳ��޸���
		else
		{
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

// ��д��Ծ������
// ����β����const���������ڲ����ܶԱ������޸�
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
	// ��Map��ɾ��
	int fd = channel->fd();
	channels_.erase(fd);


	int index = channel->index();
	if (index == kAdded)
	{
		// �����fd�Ѿ�����ӵ�Poller�У������epoll������ɾ��
		update(EPOLL_CTL_DEL, channel);
	}
	// ��������channel��״̬Ϊδ��Pollerע��
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