#pragma once
#include <netinet/in.h>
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
    int listen_fd_;
    sockaddr_in addr_;
};