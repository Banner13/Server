// jackZ

#pragma once

#include <map>
#include <thread>
#include <mutex>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "abs_server.h"

class TCPServer : public Server {
public:
    TCPServer(int port);
    ~TCPServer();

    int Init() override final;
    int Start() override final;
    int Reboot() override final;
    int Stop() override final;
    int DeInit() override final;
protected:
    virtual int MyServer(int fd);

private:
    bool m_inited;
    bool m_running;

    int m_port;
    int m_socket;

    std::mutex m_client_mutex;
    std::map<int, struct sockaddr_in> m_clients;
    int AddClient(int fd, struct sockaddr_in sockaddr);
    int DelClient(int fd);

    std::mutex m_listen_mutex;
    int m_max_fd;
    fd_set m_listen_set;
    int AddToListen(int fd);
    int DelFormListen(int fd);
    int GetListen(fd_set& listen_set, int& max_fd);

    std::thread m_listen_thread;
    int Listen();
    int Process(int fd);

    std::thread m_monitor_thread;
    int Monitor();
};