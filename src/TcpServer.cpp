#include "TcpServer.h"
#include "TcpConnection.h"
#include <asm-generic/socket.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <thread>
#include <arpa/inet.h>

TcpServer::TcpServer(const char *ip, uint16_t port) 
{
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listen_fd_, SOCK_STREAM, SO_REUSEADDR, &opt, sizeof(opt));

    addr_.sin_port = htons(port);
    addr_.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &addr_.sin_addr);
}
TcpServer::~TcpServer()
{
    close(listen_fd_);
}
void TcpServer::start()
{
    if(bind(listen_fd_, (sockaddr *)&addr_, sizeof(addr_)) < 0)
    {
        std::cerr << "bind error: " << std::strerror(errno) << std::endl;
        return;
    }

    if(listen(listen_fd_, 10) < 0)
    {
        std::cerr << "listen error: " << std::strerror(errno) << std::endl;
        return;
    }

    std::cout << "Server started..." << std::endl;
    while(true)
    {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int connfd = accept(listen_fd_, (sockaddr *)&client_addr, &client_len);

        //创建连接对象
        TcpConnection *conn = new TcpConnection(connfd, client_addr);

        //每个连接一个线程
        std::thread([conn](){
            conn->start();
            delete conn;
        }).detach();
    }
}