#include "Matt_daemon.hpp"

Matt_daemon::Matt_daemon(const Matt_daemon &src)
{
    *this = src;
}

Matt_daemon &Matt_daemon::operator=(const Matt_daemon &src)
{
    if (this != &src)
    {
        this->_socket = src.getSocket();
        this->_addr = src.getSockAddr();
        this->_addrlen = src.getAddrLen();
        std::vector<int> copy = src.getClient();
        for (auto it = copy.begin(); it != copy.end(); it++)
        {
            this->_clients.push_back(*it);
        }
        this->_logFile = src.getLogFile();
        // this->fds = src.getFds();
        for (int index = 0; index < MAX_CLIENTS; index++)
        {
            this->fds[index].fd = src.getFds()[index].fd;
            this->fds[index].events = src.getFds()[index].events;
            this->fds[index].revents = src.getFds()[index].revents;
        }
        this->_daemonPid = src.getDaemonPid();
    }
    return *this;
}

int Matt_daemon::getSocket() const
{
    return this->_socket;
}

struct sockaddr_in Matt_daemon::getSockAddr() const
{
    return this->_addr;
}
socklen_t Matt_daemon::getAddrLen() const
{
    return this->_addrlen;
}
std::vector<int> Matt_daemon::getClient() const
{
    return this->_clients;
}

std::string Matt_daemon::getLogFile() const
{
    return this->_logFile;
}
const struct pollfd *Matt_daemon::getFds() const
{
    return this->fds;
}

pid_t Matt_daemon::getDaemonPid() const
{
    return this->_daemonPid;
}
void Matt_daemon::create_socket()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
    {
        Tintin_reporter::getInstance().log_message_1("ERROR", "Matt_daemon", "socket() failed");
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
        Tintin_reporter::getInstance().log_message_1("ERROR", "Matt_daemon", "setsockopt() failed");
        throw std::runtime_error("setsockopt() failed");
    }

    if (bind(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
    {
        Tintin_reporter::getInstance().log_message_1("ERROR", "Matt_daemon", "bind() failed");
        throw std::runtime_error("bind() failed");
    }

    if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1)
    {
        Tintin_reporter::getInstance().log_message_1("ERROR", "Matt_daemon", "fcntl() failed (O_NONBLOCK)");
        throw std::runtime_error("fcntl() failed");
    }
}

void Matt_daemon::listen_socket()
{
    if (listen(_socket, MAX_CLIENTS) == -1)
    {
        Tintin_reporter::getInstance().log_message_1("ERROR", "Matt_daemon", "listen() failed");
        throw std::runtime_error("listen() failed");
    }
}

bool Matt_daemon::accept_socket()
{
    int newClient = accept(_socket, (struct sockaddr *)&_addr, &_addrlen);
    if (newClient == -1)
    {
        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) // check this
        {
            Tintin_reporter::getInstance().log_message_1("ERROR", "Matt_daemon", "accept() Just for check");
            throw std::runtime_error("accept() Just for check");
            // return;
        }
        else
        {
            Tintin_reporter::getInstance().log_message_1("ERROR", "Matt_daemon", "accept() failed");
            throw std::runtime_error("accept() failed");
        }
    }
    if (!checkMaxClients())
    {
        close(newClient);
        return false;
    }
    _clients.push_back(newClient);
    return true;
}

void Matt_daemon::read_socket(int client, int *clientCount)
{
    char buffer[1024] = {0};
    int valread = read(client, buffer, sizeof(buffer));
    if (valread == -1)
    {
        Tintin_reporter::getInstance().log_message_1("ERROR", "Matt_daemon", "read() failed");
        throw std::runtime_error("read() failed");
    }
    if (valread == 0)
    {
        for (std::vector<int>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
        {
            if (*it == client)
            {
                this->_clients.erase(it);
                break;
            }
        }
        this->fds[*clientCount].fd = -1;
        this->shiftArray(clientCount);
        close(client);
    }
    else
    {
        std::string message = buffer;
        if (message == "quit\n")
        {
            for (auto it = this->_clients.begin(); it != this->_clients.end(); it++)
                close(*it);
            close(this->fds[0].fd);
            Tintin_reporter::getInstance().log_message_1("INFO", "Matt_daemon", "Quitting");
            std::remove("/var/lock/matt_daemon.lock");
            exit(1);
        }
        Tintin_reporter::getInstance().log_message_1("LOG", "Matt_daemon", "User input: " + message);
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

bool Matt_daemon::checkMaxClients()
{
    if (this->_clients.size() >= MAX_CLIENTS)
    {
        Tintin_reporter::getInstance().log_message_1("ERROR", "Matt_daemon", "Max clients reached");
        return false;
    }
    return true;
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
            if (errno == EINTR)
                continue;
            throw std::runtime_error("poll() failed");
        }
        if (fds[0].revents & POLLIN)
        {
            if (!accept_socket())
                continue;
            fds[++clientCount].fd = _clients.back();
            fds[clientCount].events = POLLIN;
        }

        for (int i = 1; i <= clientCount; i++)
        {
            if (fds[i].revents & POLLIN)
                read_socket(fds[i].fd, &clientCount);
        }
    }

    close_socket();
}

size_t Matt_daemon::getClientsCount() const
{
    return _clients.size();
}

void Matt_daemon::shiftArray(int *size)
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

Matt_daemon::~Matt_daemon()
{
}
