#include "Channel.h"
#include "EventLoop.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    index_(-1),
    tied_(false)
{
}

//TODO:���������Ͷ��ԣ��ж��Ƿ����ڵ�ǰ�߳�
Channel::~Channel()
{
}

// ��TcpConnection������ʱ������
void Channel::tie(const std::shared_ptr<void>& obj)
{
    // weak_ptr ָ�� obj
    tie_ = obj;
    tied_ = true;
}

/**
 * ���ı�channel����ʾfd��events�¼���update������poller�������fd��Ӧ���¼�epoll_ctl
 *
 */
void Channel::update()
{
    //TODO:Channel::update()
    // ͨ����channel������EventLoop������poller��Ӧ�ķ�����ע��fd��events�¼�
    loop_->updateChannel(this);
}

// ��channel������EventLoop�У��ѵ�ǰ��channelɾ����
void Channel::remove()
{
    //TODO:Channel::remove()
    loop_->removeChannel(this);
}

// fd�õ�poller֪ͨ�Ժ�ȥ�����¼�
void Channel::handleEvent(Timestamp receiveTime)
{
    /**
     * ������Channel::tie�û�����tid_=true
     * ��TcpConnection::connectEstablished�����channel_->tie(shared_from_this());
     * ���Զ���TcpConnection::channel_ ��Ҫ��һ��ǿ���õı�֤�����û���ɾTcpConnection����
     */
    if (tied_)
    {
        // ���shared_ptr�������ü�������ֹ��ɾ
        std::shared_ptr<void> guard = tie_.lock();
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
        // guardΪ�������˵��Channel��TcpConnection�����Ѿ���������
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

// ������Ӧ�¼�ִ�лص�����
void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    // �Զ˹ر��¼�
    // ��TcpConnection��ӦChannel��ͨ��shutdown�ر�д�ˣ�epoll����EPOLLHUP
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        // ȷ���Ƿ�ӵ�лص�����
        if (closeCallback_)
        {
            closeCallback_();
        }
    }

    // �����¼�
    if (revents_ & (EPOLLERR))
    {
        LOG_ERROR << "the fd = " << this->fd();
        if (errorCallback_)
        {
            errorCallback_();
        }
    }

    // ���¼�
    if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        LOG_DEBUG << "channel have read events, the fd = " << this->fd();
        if (readCallback_)
        {
            LOG_DEBUG << "channel call the readCallback_(), the fd = " << this->fd();
            readCallback_(receiveTime);
        }
    }

    // д�¼�
    if (revents_ & EPOLLOUT)
    {
        if (writeCallback_)
        {
            writeCallback_();
        }
    }
}