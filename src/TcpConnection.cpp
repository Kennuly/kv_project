#include "TcpConnection.h"
#include <cerrno>
#include <cstdio>
#include <iterator>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>

constexpr size_t BUF_SIZE = 4096;
TcpConnection::TcpConnection(int conn_fd, sockaddr_in client_addr) :conn_fd_(conn_fd), client_addr_(client_addr)
{}

TcpConnection::~TcpConnection()
{
    ::close(conn_fd_);
}

void TcpConnection::start()
{
    handleRead();
}

void TcpConnection::handleRead()
{
    char buf[BUF_SIZE];
    while(true)
    {
        ssize_t n = recv(conn_fd_, buf, BUF_SIZE - 1, 0);
        if(n == 0)
        {
            //客户端关闭
            std::cout << "client disconnected" << std::endl;
            close(conn_fd_);
            break;
        }
        else if(n > 0)
        {
            std::string msg(buf, n);
            std::cout << "recv: " << msg;
            std::string resq = msg;
            send(conn_fd_, resq.data(), resq.size(), 0);
        }
        else 
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                //边缘触发完了
                break;
            }
            std::cerr << "Read error" << std::endl;
            close(conn_fd_);
            return;
        }
    }
}