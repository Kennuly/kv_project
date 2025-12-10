#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>

constexpr size_t BUF_SIZE = 4096;

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "socket error: " << strerror(errno) << "\n";
        return 1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(12345);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "inet_pton error\n";
        return 1;
    }

    if (connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "connect error: " << strerror(errno) << "\n";
        close(sockfd);
        return 1;
    }

    std::cout << "Connected to server. Type message and press Enter.\n";
    std::cout << "Type 'quit' to exit.\n";

    char buf[BUF_SIZE];

    while (true) {
        std::cout << "> ";
        std::string msg;
        std::getline(std::cin, msg);

        if (msg == "quit")
            break;

        msg += "\n";

        // 发送消息
        if (send(sockfd, msg.data(), msg.size(), 0) < 0) {
            std::cerr << "send error: " << strerror(errno) << "\n";
            break;
        }

        // 接收消息
        ssize_t n = recv(sockfd, buf, BUF_SIZE - 1, 0);
        if (n <= 0) {
            std::cerr << "server closed connection\n";
            break;
        }

        buf[n] = '\0';
        std::cout << "server: " << buf;
    }

    close(sockfd);
    std::cout << "Client exit.\n";

    return 0;
}
