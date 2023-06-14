#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include <memory>
#include <sys/epoll.h>

#include "noncopyable.h"
#include "Timestamp.h"
#include "Logger.h"

// ǰ��������������ͷ�ļ���ֹ��¶̫����Ϣ
class EventLoop;
class Timestamp;

/**
* Channel ���Ϊͨ������װ��sockfd�������Ȥ��event����EPOLLIN,EPOLLOUT
* ������poller���صľ����¼�
*/
class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    // fd�õ�poller֪ͨ�Ժ󣬴����¼��Ļص�����
    void handleEvent(Timestamp receiveTime);

    // ���ûص���������
    // ʹ����ֵ���ã��ӳ���cb������������ڣ������˿�������
    void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

    // TODO:��ֹ�� channel ִ�лص�����ʱ�����ֶ� remove ��
    void tie(const std::shared_ptr<void>&);

    int fd() const { return fd_; }                    // ���ط�װ��fd
    int events() const { return events_; }            // ���ظ���Ȥ���¼�
    void set_revents(int revt) { revents_ = revt; }  // ����Poller���صķ����¼�

    // ����fd��Ӧ���¼�״̬��update()�䱾�ʵ���epoll_ctl
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ &= kNoneEvent; update(); }

    // ����fd��ǰ���¼�״̬
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    /**
     * for Poller
     * const int kNew = -1;     // fd��δ��poller����
     * const int kAdded = 1;    // fd����poller������
     * const int kDeleted = 2;  // fd���Ƴ�poller
     */
    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }

    // one lool per thread
    EventLoop* ownerLoop() { return loop_; }
    void remove();

private:
    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    /**
     * const int Channel::kNoneEvent = 0;
     * const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
     * const int Channel::kWriteEvent = EPOLLOUT;
     */
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;   // ��ǰChannel���ڵ�EventLoop
    const int fd_;      // fd, Poller��������
    //events_��revents_����
    //events_��Muduo������ģ��ж�fd��ǰ״̬����ʵ��Ҳ��epoll״̬�����
    //revents_��epoll�������״̬
    int events_;        // ע��fd����Ȥ���¼�
    int revents_;       // poller���صľ��巢�����¼�
    int index_;         // ��Poller��ע������

    std::weak_ptr<void> tie_;   // ��ָ��ָ��TcpConnection(��Ҫʱ����Ϊshared_ptr��һ�����ü����������û���ɾ)
    bool tied_;  // ��־�� Channel �Ƿ񱻵��ù� Channel::tie ����

    // ��Ϊ channel ͨ�������ܹ���֪fd���շ����ľ�����¼�revents
    // �����¼�����ʱ�Ļص�����
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

#endif // CHANNEL_H