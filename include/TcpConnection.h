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
    TcpConnection(int conn_fd, sockaddr_in client_addr);
    ~TcpConnection();
    void start();
    
    //返回连接对应的文件描述符fd
    int fd() const {return conn_fd_;}
private:
    void handleRead();
private:
    int conn_fd_;
    sockaddr_in client_addr_;
};