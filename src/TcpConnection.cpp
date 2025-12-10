#include "TcpConnection.h"
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
        if(n <= 0)
        {
            std::cout << "client disconnected" << std::endl;
            break;
        }
        buf[n] = '\0';
        std::string req(buf);
        while(!req.empty() && (req.back() == '\n' || req.back() == '\r'))   req.pop_back();
        std::string reqs = (req == "ping") ? "ping\n" : "unknow\n";
        send(conn_fd_, reqs.data(), reqs.size(), 0);
    }
}