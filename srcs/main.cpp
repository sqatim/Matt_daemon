#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

#define MAX_CLIENTS 3
#define LISTEN_PORT 4242

class Matt_daemon
{
private:
    int _socket;
    struct sockaddr_in _addr;
    socklen_t _addrlen;
    std::vector<int> _clients;

public:
    Matt_daemon();
    ~Matt_daemon();

    void create_socket();
    void bind_socket();
    void listen_socket();
    void accept_socket();
    void read_socket(int client);
    void write_socket(int client);
    void close_socket();
    void run();
};

Matt_daemon::Matt_daemon()
{
    _addrlen = sizeof(_addr);
}

Matt_daemon::~Matt_daemon()
{
}

void Matt_daemon::create_socket()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
        throw std::runtime_error("socket() failed");
}

void Matt_daemon::bind_socket()
{
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(LISTEN_PORT);
    _addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
        throw std::runtime_error("bind() failed");
}

void Matt_daemon::listen_socket()
{
    if (listen(_socket, MAX_CLIENTS) == -1)
        throw std::runtime_error("listen() failed");
}

void Matt_daemon::accept_socket()
{
    int newClient = accept(_socket, (struct sockaddr *)&_addr, &_addrlen);
    if (newClient == -1)
        throw std::runtime_error("accept() failed");
    _clients.push_back(newClient);
}

void Matt_daemon::read_socket(int client)
{
    char buffer[1024] = {0};
    int valread = read(client, buffer, sizeof(buffer));
    if (valread == -1)
        throw std::runtime_error("read() failed");
    std::cout << buffer << std::endl;
    if (buffer[0] == 'q')
    {
        close(client);
        exit(0);
    }
}

void Matt_daemon::write_socket(int client)
{
    std::string hello = "Hello from server";
    send(client, hello.c_str(), hello.length(), 0);
}

void Matt_daemon::close_socket()
{
    close(_socket);
}

void Matt_daemon::run()
{
    create_socket();
    bind_socket();
    listen_socket();

    struct pollfd fds[MAX_CLIENTS + 1];
    memset(fds, 0, sizeof(fds));

    fds[0].fd = _socket;
    fds[0].events = POLLIN;

    int clientCount = 0;

    while (true)
    {
        int result = poll(fds, clientCount + 1, -1);
        if (result == -1)
        {
            throw std::runtime_error("poll() failed");
        }

        if (fds[0].revents & POLLIN)
        {
            accept_socket();
            fds[++clientCount].fd = _clients.back();
            fds[clientCount].events = POLLIN;
        }

        for (int i = 1; i <= clientCount; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                read_socket(fds[i].fd);
                // write_socket(fds[i].fd);
            }
        }
    }

    close_socket();
}

int main()
{
    Matt_daemon daemon;
    daemon.run();
    return 0;
}