#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <iomanip>
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
    int _port;
    struct sockaddr_in _addr;
    socklen_t _addrlen;
    std::vector<int> _clients;
    // char _buffer[MAX_BUFFER + 1];
    std::string _log;
    struct pollfd fds[MAX_CLIENTS + 1];

public:
    Matt_daemon(void);
    Matt_daemon(Matt_daemon const & src);
    ~Matt_daemon(void);
    void create_socket(void);
    void bind_socket(void);
    void listen_socket(void);
    void accept_socket(void);
    void read_socket(int client);
    void write_socket(int client);
    void close_socket(void);
    void checkMaxClients(int clientCount);
    void run(void);

    Matt_daemon & operator=(Matt_daemon const & rhs);
};



