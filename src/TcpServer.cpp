#include "TcpServer.h"
#include "TcpConnection.h"
#include <asm-generic/socket.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <thread>
#include <arpa/inet.h>
#include <sys/epoll.h>

TcpServer::TcpServer(const char *ip, uint16_t port)
{
    listen_fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(listen_fd_ < 0)
    {
        std::cerr << "socket error" << std::strerror(errno) << std::endl;
    }
    int opt = 1;
    setsockopt(listen_fd_, SOCK_STREAM, SO_REUSEADDR, &opt, sizeof(opt));

    addr_.sin_port = htons(port);
    addr_.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &addr_.sin_addr);

    if(bind(listen_fd_, (sockaddr *)&addr_, sizeof(addr_)) < 0)
    {
        std::cerr << "bind error" << strerror(errno) << std::endl;
    }
    
    if(listen(listen_fd_, 1024) < 0)
    {
        std::cerr << "listen error" << strerror(errno) << std::endl;
    }
}
TcpServer::~TcpServer()
{
    close(listen_fd_);
}
void TcpServer::start()
{
    //epoll_create1 是新接口，可以设置 EPOLL_CLOEXEC 等标志，推荐使用。
    epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);

    //事件
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd_;
    epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listen_fd_, &ev);

    //epollLoop事件主循环
    epollLoop();
}
void TcpServer::epollLoop()
{
    const int Max_Events = 1024;
    epoll_event events[Max_Events];
    while(true)
    {
        int n = epoll_wait(epoll_fd_, events, Max_Events, -1);
        for(int i = 0; i < n; i++)
        {
            int fd = events[i].data.fd;
            if(fd == listen_fd_)
            {
                //有新的连接
                handleAccept();
            }
            else
            {
                //连接有数据可读
                auto conn = connections_[fd];
                conn->start();
            }
        }
    }
}
void TcpServer::handleAccept()
{
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int conn_fd = accept4(listen_fd_, (sockaddr *)&client_addr, &len, SOCK_NONBLOCK);

    //一个accept对应一个TcpConnection
    auto conn = std::make_shared<TcpConnection>(conn_fd, client_addr);
    connections_[conn_fd] = conn;

    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;  //边缘触发
    ev.data.fd = conn_fd;

    epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, conn_fd, &ev);
    std::cout << "New connection: fd=" << conn_fd << std::endl;
}