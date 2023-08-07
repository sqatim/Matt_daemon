#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <cctype>
#include <cstdio>
#include <ctime>
#include <cstdarg>
#include <netdb.h>
#include <pwd.h>

#define MAX_CLIENTS 10
#define MAX_BUFFER 1024
#define MAX_PATH 1024
#define MAX_LOG 1024
#define MAX_LOG_SIZE 1024

class Matt_daemon
{
private:
    int _socket;
    int _port;
    struct sockaddr_in _addr;
    int _addrlen;
    int _client;
    // char _buffer[MAX_BUFFER + 1];
    std::string _log;


public:
    Matt_daemon(void);
    Matt_daemon(Matt_daemon const & src);
    ~Matt_daemon(void);
    void create_socket(void);
    void bind_socket(void);
    void listen_socket(void);
    void accept_socket(void);
    void read_socket(void);
    void write_socket(void);
    void close_socket(void);

    Matt_daemon & operator=(Matt_daemon const & rhs);

    void run(void);

private:


};

std::ostream & operator<<(std::ostream & o, Matt_daemon const & rhs);



