#pragma once


#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fstream>
#include <mutex>
#include <string.h>
#include <fcntl.h>
#include <filesystem>
#include "daemon.hpp"

#define MAX_CLIENTS 3
#define LISTEN_PORT 4242
namespace fs = std::filesystem;


class Matt_daemon
{
private:

    int _socket;
    struct sockaddr_in _addr;
    socklen_t _addrlen;
    std::vector<int> _clients;
    std::string _logFile = "/var/log/matt_daemon/matt_daemon.log";
    struct pollfd fds[MAX_CLIENTS + 1];
    pid_t _daemonPid;

    Matt_daemon()
    {
        // Private constructor to prevent direct instantiation
        initialize();
    }

    void initialize()
    {
        _addrlen = sizeof(_addr);

        Tintin_reporter::getInstance().log_message_1("INFO", "Matt_daemon", "Starting.");
        Tintin_reporter::getInstance().log_message_1("INFO", "Matt_daemon", "Creating Server.");
        Tintin_reporter::getInstance().log_message_1("INFO", "Matt_daemon", "Server created.");

        _daemonPid = getpid();
    }

public:
    Matt_daemon(const Matt_daemon &);
    Matt_daemon &operator=(const Matt_daemon &);

    static Matt_daemon *getInstance()
    {
        return new Matt_daemon();
    }

    ~Matt_daemon(void);
    void create_socket(void);
    void bind_socket(void);
    void listen_socket(void);
    bool accept_socket(void);
    void read_socket(int client, int *clientCount);
    void write_socket(int client);
    void close_socket(void);
    bool checkMaxClients(void);

    void run(void);


    int getSocket() const;
    struct sockaddr_in getSockAddr() const;
    socklen_t getAddrLen() const;
    std::vector<int> getClient() const;
    std::string getLogFile() const;
    const struct pollfd *getFds() const ; 
    pid_t getDaemonPid() const;
    size_t getClientsCount() const;
    void shiftArray(int *size);
};
