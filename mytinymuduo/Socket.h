#pragma once
#include "noncopyable.h"

class InetAddress;

class Socket :
    public noncopyable
{
public:
    explicit Socket(int sockfd)
        : sockfd_(sockfd)
    {

    }
    ~Socket();
    // ��ȡsockfd
    int fd() const { return sockfd_; }
    // ��sockfd
    void bindAddress(const InetAddress& localaddr);
    // ����ʹsockfdΪ�ɽ�������״̬
    void listen();
    // ��������
    int accept(InetAddress* peeraddr);

    // ���ð�ر�
    void shutdownWrite();

    void setTcpNoDelay(bool on);    // ����Nagel�㷨 
    void setReuseAddr(bool on);     // ���õ�ַ����
    void setReusePort(bool on);     // ���ö˿ڸ���
    void setKeepAlive(bool on);     // ���ó�����
private:
    const int sockfd_;
};

