#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
/*
    TcpConnection
    封装一个连接的读写逻辑
    read → parse → execute → write
*/
class TcpConnection
{
public:
    TcpConnection(int, sockaddr_in);
    ~TcpConnection();
    void start();
private:
    void handleRead();
private:
    int conn_fd_;
    sockaddr_in client_addr_;
};