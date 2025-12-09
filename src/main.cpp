#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <cstddef>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cerrno>

int main()
{
    const int port = 12345;

    //创建socket连接
    int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd < 0)
    {
        std::cerr << "socket error" << std::strerror(errno) << std::endl;
        return 1;
    }

    //开启端口复用
    int opt = 1;
    //SO_REUSEADDR 的作用是什么？   端口快速复用    多个进程可以绑定同一个端口
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "setsocketopt error" << std::strerror(errno) << std::endl;
        ::close(listen_fd);
        return 1;
    }

    //源地址
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;      //ipv4
    addr.sin_addr.s_addr = INADDR_ANY;      //0.0.0.0
    addr.sin_port = htons(port);

    //bind源ip端口
    if(::bind(listen_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "bind error" << std::strerror(errno) << std::endl;
        ::close(listen_fd);
        return 1;
    }

    //开启监听listen
    if(::listen(listen_fd, 10) < 0)
    {
        std::cerr << "listen error" << std::strerror(errno) << std::endl;
        ::close(listen_fd);
        return 1;
    }
    std::cout << "Server listening on 0.0.0.0:" << port << "\n";

    while(true)
    {
        //accept建立连接
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int conn_fd = ::accept(listen_fd, (sockaddr *)&client_addr, &client_len);
        if(conn_fd < 0)
        {
            std::cerr << "accept error" << std::strerror(errno) << std::endl;
            continue;
        }

        //INET_ADDRSTRLEN 是用于存储 IPv4 文本形式的宏     等价于   cli_ip[16];
        char cli_ip[INET_ADDRSTRLEN];
        //二进制ip转化为字符ip
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, cli_ip, sizeof(cli_ip));
        std::cout << "Accepted connection from " << cli_ip << ":" << ntohs(client_addr.sin_port) << std::endl;

        //简单的阻塞读取循环（单连接处理）
        //定义一个 编译期常量,它在编译时就确定好，不会在运行时修改，也不能修改（否则编译报错）
        constexpr size_t Buf_SIZE = 4096;
        char buf[Buf_SIZE];
        while(true)
        {
            ssize_t n = ::recv(conn_fd, buf, Buf_SIZE - 1, 0);
            if(n < 0)
            {
                std::cerr << "revc error" << std::strerror(errno) << std::endl;
                break;
            }
            else if(n == 0)
            {
                std::cout << "client closed connection" << std::endl;
                break;
            }
            buf[n] = '\0';
            std::string req(buf);

            //去掉末尾换行
            while(!req.empty() && (req.back() == '\n' || req.back() == '\r'))
                req.pop_back();

            std::cout << "recv: ["<< req << "]" << std::endl;

            std::string resp;
            if(req == "ping")   resp = "pong\n";
            else resp = "unknown commmand\n";

            ssize_t sent = ::send(conn_fd, resp.data(), resp.size(), 0);
            if(sent < 0)
            {
                std::cerr << "send error" << std::strerror(errno) << std::endl;
                break;
            }

        }
        ::close(conn_fd);
    }
    ::close(listen_fd);
    return 0;
}
