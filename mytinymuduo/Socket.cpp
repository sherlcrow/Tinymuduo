#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include "Socket.h"
#include "Logger.h"
#include "InetAddress.h"

Socket::~Socket()
{
	::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& localaddr)
{
    if (0 != ::bind(sockfd_, (sockaddr*)localaddr.getSockAddr(), sizeof(sockaddr_in)))
    {
        LOG_FATAL << "bind sockfd:" << sockfd_ << " fail";
    }
}

void Socket::listen()
{
    if (0 != ::listen(sockfd_, 1024))
    {
        LOG_FATAL << "listen sockfd:" << sockfd_ << " fail";
    }
}

int Socket::accept(InetAddress* peeraddr)
{
    /**
     * 1. accept�����Ĳ������Ϸ�
     * 2. �Է��ص�connfdû�����÷�����
     * Reactorģ�� one loop per thread
     * poller + non-blocking IO
     **/
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    ::memset(&addr, 0, sizeof(addr));
    // fixed : int connfd = ::accept(sockfd_, (sockaddr *)&addr, &len);
    //SOCK_NONBLOCK ������  SOCK_CLOEXEC ��ֹ���ӽ����ļ�����Խ��
    int connfd = ::accept4(sockfd_, (sockaddr*)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    // int connfd = ::accept(sockfd_, (sockaddr *)&addr, &len);
    if (connfd >= 0)
    {
        peeraddr->setSockAddr(addr);
    }
    else
    {
        LOG_ERROR << "accept4() failed";
    }
    return connfd;
}
// TODO:socket�ĸ������õ��÷�
void Socket::shutdownWrite()
{
    /**
     * �ر�д�ˣ����ǿ��Խ��ܿͻ�������
     */
    if (::shutdown(sockfd_, SHUT_WR) < 0)
    {
        LOG_ERROR << "shutdownWrite error";
    }
}

// ������Nagle�㷨
void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)); // TCP_NODELAY����ͷ�ļ� <netinet/tcp.h>
}

// ���õ�ַ���ã���ʵ���ǿ���ʹ�ô���Time-wait�Ķ˿�
void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); // TCP_NODELAY����ͷ�ļ� <netinet/tcp.h>
}

// ͨ���ı��ں���Ϣ��������̿��԰�ͬһ����ַ��ͨ�׾��Ƕ�������ip+port��һ��
void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)); // TCP_NODELAY����ͷ�ļ� <netinet/tcp.h>
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)); // TCP_NODELAY����ͷ�ļ� <netinet/tcp.h>
}