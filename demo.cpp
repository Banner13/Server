#include "tcp_server.h"

#include <stdio.h>

int main(void)
{
    TCPServer server(8088);

    server.Init();
    server.Start();
    printf("hello j\n");
    while (1);
    return 0;
}