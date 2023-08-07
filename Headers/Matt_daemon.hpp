#ifndef MATT_DAEMON_HPP
#define MATT_DAEMON_HPP

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fstream>
#include <fcntl.h>
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
#endif