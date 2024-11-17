
#include <iostream>
#include <string>

#include "tcp_server.h"

#include <stdio.h>

int main(void)
{
    std::string userInput;
    TCPServer server(8088);

    server.Init();
    server.Start();
    printf("hello j\n");
    while (1) {
        std::getline(std::cin, userInput);
        if (userInput == "stop") {
            std::cout << "程序已退出。" << std::endl;
            break;
        }
    }
    server.Stop();
    server.DeInit();
    return 0;
}