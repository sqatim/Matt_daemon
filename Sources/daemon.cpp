#include "daemon.hpp"
#include <chrono>
#include <thread>
#include "Matt_daemon.hpp"
#include <signal.h>
#include <filesystem>
#include <sys/file.h>

namespace fs = std::filesystem;

void signalMsg(int signum)
{
    std::ofstream file("matt_daemon.log", std::ios::app);
    if (file.is_open())
    {
        file << "Signal\n";
        file.close();
    }
    else
        throw std::runtime_error("Failed to open matt_daemon.log");
}

void handle_signals()
{

    // Ignore all signals
    for (int signum = 1; signum <= 31; signum++)
    {
        // if (signum != 9 && signum != 17)
        signal(signum, signalMsg);
    }
}

void startDaemon()
{
    // int lfp
    int pid;
    int i;
    int fd;
    int number;
    int stdError;
    if (geteuid() != 0 || getuid() != 0)
    {
        std::cout << "not root" << std::endl;
        exit(1);
    }
    pid = fork();
    if (pid < 0)
        exit(1);
    else if (pid > 0)
        exit(0);
    setsid();
    chdir("/var/lock");
    fs::path folderPath = "matt_daemon";
    if (!fs::exists(folderPath))
    {
        if (fs::create_directories(folderPath))
        {
            std::cout << "Folder created successfully." << std::endl;
        }
        else
        {
            std::cerr << "Failed to create folder." << std::endl;
        }
    }
    for (i = getdtablesize(); i >= 0; --i)
    {
        if (i != STDERR_FILENO)
            close(i); /* close all descriptors */
    }
    i = open("/dev/null", O_RDWR);
    dup2(i, 0);
    dup2(i, 1);
    dup2(STDERR_FILENO, 2);
    fd = open("/var/lock/matt_daemon/matt_daemon.lock", O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (flock(fd, LOCK_EX | LOCK_NB) == -1)
    {
        if (errno == EWOULDBLOCK)
        {
            std::cerr << "Can't open :/var/lock/matt_daemon.lock" << std::endl;
        }
        else
            std::cerr << "Error acquiring exclusive lock." << std::endl;
        exit(1);
    }

    handle_signals();
    Matt_daemon daemon;
    daemon.run();
    if (flock(fd, LOCK_UN) == -1)
    {
        std::cerr << "Error releasing exclusive lock." << std::endl;
    }
    else
        std::cout << "diana" << std::endl;
    close(fd);
    std::remove("/var/lock/matt_daemon/matt_daemon.lock");
    return;
}