#include "TcpServer.h"
int main()
{
    TcpServer server("0.0.0.0", 12345);
    server.start();
    return 0;
}