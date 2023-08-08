
#include "Matt_daemon.hpp"

Matt_daemon::~Matt_daemon()
{
}

void Matt_daemon::create_socket()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
    {
        log_message("ERROR", "Matt_daemon", "socket() failed");
        throw std::runtime_error("socket() failed");
    }
}

void Matt_daemon::bind_socket()
{
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(LISTEN_PORT);
    _addr.sin_addr.s_addr = INADDR_ANY;

    // Set SO_REUSEADDR option
    int reuseAddr = 1;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) == -1)
    {
        log_message("ERROR", "Matt_daemon", "setsockopt() failed");

        throw std::runtime_error("setsockopt() failed");
    }

    if (bind(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
    {
        log_message("ERROR", "Matt_daemon", "bind() failed");
        throw std::runtime_error("bind() failed");
    }

    if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1)
    {
        log_message("ERROR", "Matt_daemon", "fcntl() failed (O_NONBLOCK)");
        throw std::runtime_error("fcntl() failed");
    }
}

void Matt_daemon::listen_socket()
{
    if (listen(_socket, MAX_CLIENTS) == -1)
    {
        log_message("ERROR", "Matt_daemon", "listen() failed");
        throw std::runtime_error("listen() failed");
    }
}

void Matt_daemon::accept_socket()
{
    int newClient = accept(_socket, (struct sockaddr *)&_addr, &_addrlen);
    if (newClient == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK) // check this
            return;
        else
        {
            log_message("ERROR", "Matt_daemon", "accept() failed");
            throw std::runtime_error("accept() failed");
        }
    }
    _clients.push_back(newClient);
}

void Matt_daemon::log_message(const std::string &log_type, const std::string &username, const std::string &message)
{
    std::string timestamp = currentDateTime();
    std::string log_entry = "[" + timestamp + "] [" + log_type + "] - " + username + ": " + message;

    size_t lastCharPos = log_entry.find_last_not_of(" \t\n\r\f\v");
    if (lastCharPos != std::string::npos)
        log_entry = log_entry.substr(0, lastCharPos + 1);

    std::ofstream log_file(this->_logFile, std::ios::app);
    if (log_file.is_open() && !log_entry.empty())
    {
        log_file << log_entry << '\n';
        log_file.close();
    }
    else
    {
        log_message("ERROR", "Matt_daemon", "Failed to open client_messages.log");
        for (size_t i = 1; i <= _clients.size(); i++)
            close(fds[i].fd);

        close_socket();
    }
}

std::string Matt_daemon::currentDateTime()
{
    time_t now = time(nullptr);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M:%S", &tstruct);
    return buf;
}

void Matt_daemon::read_socket(int client)
{
    char buffer[1024] = {0};
    int valread = read(client, buffer, sizeof(buffer));
    if (valread == -1)
    {
        log_message("ERROR", "Matt_daemon", "read() failed");
        throw std::runtime_error("read() failed");
    }
    std::string message = buffer;
    log_message("LOG", "Matt_daemon", "User input: " + message);
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

void Matt_daemon::checkMaxClients()
{
    if (this->_clients.size() > MAX_CLIENTS)
        log_message("ERROR", "Matt_daemon", "Max clients reached");
}

void Matt_daemon::run()
{
    create_socket();
    bind_socket();
    listen_socket();

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

        checkMaxClients();

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
    Matt_daemon *daemon = Matt_daemon::getInstance();
    // Matt_daemon* daemon2 = Matt_daemon::getInstance();

    // std::cout << "daemon1 address: " << daemon1 << std::endl;
    // std::cout << "daemon2 address: " << daemon2 << std::endl;

    daemon->run();

    daemon->log_message("INFO", "Matt_daemon", "Entering Daemon mode.");
    pid_t daemonPid = daemon->getDaemonPid();
    daemon->log_message("INFO", "Matt_daemon", "started. PID: " + std::to_string(daemonPid));

    return 0;
}