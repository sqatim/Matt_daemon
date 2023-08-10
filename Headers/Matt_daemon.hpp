#pragma once

#include "Tintin_reporter.hpp"

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

#define MAX_CLIENTS 3
#define LISTEN_PORT 4242
namespace fs = std::filesystem;

class Matt_daemon
{
private:
    // static Matt_daemon *instance;
    // static std::mutex mutex;

    int _socket;
    struct sockaddr_in _addr;
    socklen_t _addrlen;
    std::vector<int> _clients;
    std::string _logFile = "/var/log/matt_daemon/matt_daemon.log";
    struct pollfd fds[MAX_CLIENTS + 1];
    pid_t _daemonPid; // pid of the daemon process
    Tintin_reporter *tintin_reporter = Tintin_reporter::getInstance();

    Matt_daemon()
    {
        // Private constructor to prevent direct instantiation
        initialize();
    }

    void initialize()
    {
        _addrlen = sizeof(_addr);

        tintin_reporter->log_message_1("INFO", "Matt_daemon", "Starting.");
        tintin_reporter->log_message_1("INFO", "Matt_daemon", "Creating Server.");
        tintin_reporter->log_message_1("INFO", "Matt_daemon", "Server created.");

        _daemonPid = getpid();
    }

public:
    Matt_daemon(const Matt_daemon &) = delete;
    Matt_daemon &operator=(const Matt_daemon &) = delete;

    static Matt_daemon *getInstance()
    {
        return new Matt_daemon();
    }

    ~Matt_daemon(void);
    void create_socket(void);
    void bind_socket(void);
    void listen_socket(void);
    void accept_socket(void);
    void read_socket(int client, int *clientCount);
    void write_socket(int client);
    void close_socket(void);
    void checkMaxClients(void);
    // void log_message(const std::string &log_type, const std::string &username, const std::string &message);
    std::string currentDateTime(void);

    void run(void);

    pid_t getDaemonPid() const
    {
        return _daemonPid;
    }

    size_t getClientsCount() const
    {
        return _clients.size();
    }

    // int getFD(int fd)
    // {
    //     for (int i = 0; i < MAX_CLIENTS + 1; ++i)
    //     {
    //         if (fds[i].fd == fd)
    //             return i;
    //     }
    //     return -1;
    // }
    void shiftArray(int *size)
    {
        int i = 0;
        int n = *size;

        while (i < n)
        {
            if (fds[i].fd == -1)
            {
                for (int j = i; j < n - 1; j++)
                {
                    fds[j].fd = fds[j + 1].fd;
                    fds[j].events = fds[j + 1].events;
                }
                n--;
            }
            else
            {
                i++;
            }
        }

        *size = n;
    }
};
