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
    static Matt_daemon *instance;
    static std::mutex mutex;

    int _socket;
    struct sockaddr_in _addr;
    socklen_t _addrlen;
    std::vector<int> _clients;
    std::string _logFile = "matt_daemon.lock";
    struct pollfd fds[MAX_CLIENTS + 1];
    pid_t _daemonPid; // pid of the daemon process

    Matt_daemon()
    {
        // Private constructor to prevent direct instantiation
        initialize();   
    }

    void initialize()
    {
        _addrlen = sizeof(_addr);

        log_message("INFO", "Matt_daemon", "Started.");
        log_message("INFO", "Matt_daemon", "Creating Server.");
        log_message("INFO", "Matt_daemon", "Server created.");

        _daemonPid = getpid();
    }

public:
    Matt_daemon(const Matt_daemon &) = delete;
    Matt_daemon &operator=(const Matt_daemon &) = delete;

    static Matt_daemon *getInstance()
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!instance)
        {
            instance = new Matt_daemon();
        }

        return instance;
    }

    // Matt_daemon(void);
    // Matt_daemon(Matt_daemon const &src);
    ~Matt_daemon(void);
    void create_socket(void);
    void bind_socket(void);
    void listen_socket(void);
    void accept_socket(void);
    void read_socket(int client);
    void write_socket(int client);
    void close_socket(void);
    void checkMaxClients(void);
    void log_message(const std::string &log_type, const std::string &username, const std::string &message);
    std::string currentDateTime(void);

    void run(void);

    pid_t getDaemonPid() const
{
    return _daemonPid;
}
};

// Initialize static member variables
Matt_daemon *Matt_daemon::instance = nullptr;
std::mutex Matt_daemon::mutex;


