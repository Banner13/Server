// jackZ

#pragma once

#include "tcp_server.h"

class TestServer : public TCPServer {
public:
    TestServer(int port) :TCPServer(port) {};
    ~TestServer() {};

protected:
    int MyServer(int fd) override final;

private:

};


