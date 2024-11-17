// jackZ

#include <unistd.h>
#include <strings.h>

#include <sys/time.h>
#include <sys/types.h>


#include "tcp_server.h"


TCPServer::TCPServer(int port)
{
    m_inited = false;
    m_running = false;
    m_socket = -1;
    m_max_fd = 0;
    m_port = port;

    FD_ZERO(&m_listen_set);
}

TCPServer::~TCPServer()
{
    m_inited = false;
    m_running = false;
    m_socket = -1;
    m_max_fd = 0;
    m_port = -1;

    FD_ZERO(&m_listen_set);
}

int TCPServer::Init()
{
    struct sockaddr_in servaddr;

    if (m_inited)
        return 0;

    bzero(&servaddr, sizeof(servaddr));

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == m_socket) {
        perror("socket failed");
        return -1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(m_port);

    bind(m_socket, (struct sockaddr *)&servaddr, sizeof(servaddr));
    m_inited = true;
    return 0;
}

int TCPServer::Start()
{
    if (m_running || !m_inited)
        return 0;

    m_running = true;
    m_listen_thread = std::thread( [this]() {this->Listen();});
    m_monitor_thread = std::thread( [this]() {this->Monitor();});

    return 0;
}

int TCPServer::Reboot()
{

    return 0;
}

int TCPServer::Stop()
{
    m_running = false;

    if (m_monitor_thread.joinable())
        m_monitor_thread.join();

    if (m_listen_thread.joinable())
        m_listen_thread.join();

    for (auto it = m_clients.begin(); it != m_clients.end();)
        DelClient(it->first);

    return 0;
}

int TCPServer::DeInit()
{
    if (!m_inited)
        return 0;

    close(m_socket);

    m_inited = false;
    return 0;
}

int TCPServer::AddToListen(int fd)
{
    std::lock_guard<std::mutex> lock(m_listen_mutex);
    FD_SET(fd, &m_listen_set);

    if (m_max_fd <= fd)
        m_max_fd = fd + 1;

    return 0;
}

int TCPServer::DelFormListen(int fd)
{
    std::lock_guard<std::mutex> lock(m_listen_mutex);
    FD_CLR(fd, &m_listen_set);

    return 0;
}

int TCPServer::GetListen(fd_set& listen_set, int& max_fd)
{
    std::lock_guard<std::mutex> lock(m_listen_mutex);
    listen_set = m_listen_set;
    max_fd = m_max_fd;
    return 0;
}

int TCPServer::AddClient(int fd, struct sockaddr_in sockaddr)
{
    std::lock_guard<std::mutex> lock(m_client_mutex);
    m_clients[fd] = sockaddr;

    AddToListen(fd);

    return 0;
}

int TCPServer::DelClient(int fd)
{
    std::lock_guard<std::mutex> lock(m_client_mutex);
    m_clients.erase(fd);

    close(fd);

    return 0;
}

int TCPServer::Process(int fd)
{
    char buffer[1];
    int bytes = recv(fd, buffer, sizeof(buffer), MSG_PEEK);
    if (bytes == 0) {
        printf("Disable %d\n", fd);
        DelClient(fd);
        return 0;
    }

    MyServer(fd);

    FD_SET(fd, &m_listen_set);
    return 0;
}

int TCPServer::Listen()
{
    int ret, fd, max_fd;
    fd_set set;
    struct timeval timeout;

    while (m_running)
    {
        FD_ZERO(&set);
        timeout.tv_sec = 5;
        timeout.tv_usec = 50000;

        GetListen(set, max_fd);

        ret = select(max_fd, &set, NULL, NULL, &timeout);

        if (ret < 0) {
            perror("select error");
        } else if (0 == ret) {
            printf("timeout\n");
        } else {

            for (auto const& it : m_clients) {
                if (FD_ISSET(it.first, &set)) {
                    fd = it.first;
                    break;
                }
            }
            printf("Processing %d\n", fd);
            DelFormListen(fd);
            
            auto process = [this](int fds) {return this->Process(fds);};
            std::thread(process, fd).detach();
        }
        printf("\n");
    }

    return 0;
}

int TCPServer::Monitor()
{
    struct sockaddr_in cliaddr;
    socklen_t cliaddr_len;
    int fd;

    listen(m_socket, 0xFF);

    while (m_running)
    {
        fd = accept(m_socket, (struct sockaddr *)&cliaddr, &cliaddr_len);
        if (fd < 0) {
            perror("accept failed");
            return -1;
        }

        printf("get accept %d\n", fd);

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cliaddr.sin_addr, client_ip, sizeof(client_ip));
        printf("Connection accepted from %s:%d\n", client_ip, ntohs(cliaddr.sin_port));
        
        AddClient(fd, cliaddr);
    }

    return 0;
}


int TCPServer::MyServer(int fd)
{
    char buffer[256];
    int bytes_read = read(fd, buffer, sizeof(buffer));
    printf("MyServer(%d) get %s\n", fd, buffer);
    return 0;
}

