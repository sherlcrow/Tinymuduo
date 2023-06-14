#pragma once
#ifndef INET_ADDRESS_H
#define INET_ADDRESS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <string.h>

// TODO: 网络字节序和主机字节序的转换函数
class InetAddress
{
public:
    explicit InetAddress(uint16_t port = 0, std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in& addr)
        : addr_(addr)
    {
    }

    //ip
    std::string toIp() const;
    //ip:port
    std::string toIpPort() const;
    //port
    uint16_t toPort() const;

    const sockaddr_in* getSockAddr() const { return &addr_; }
    void setSockAddr(const sockaddr_in& addr) { addr_ = addr; }
private:
    sockaddr_in addr_;
};
#endif
