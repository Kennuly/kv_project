#pragma once
#include "TcpConnection.h"
#include <memory>
#include <netinet/in.h>
#include <unordered_map>
/*
    TcpServer
    负责 listen + accept
    为每个连接创建线程（暂时仍是 Day2 模型）
*/
class TcpServer
{
public:
    TcpServer(const char*, uint16_t);
    ~TcpServer();
    void start();
private:
    void epollLoop();
    void handleAccept();
private:
    int listen_fd_;        
    sockaddr_in addr_;
    int epoll_fd_;
    std::unordered_map<int, std::shared_ptr<TcpConnection>> connections_;

};