#include "daemon.hpp"

#include <chrono>
#include <thread>
#include "Matt_daemon.hpp"
#include <signal.h>
#include <filesystem>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>


namespace fs = std::filesystem;

std::string currentDateTime()
{
    time_t now = time(nullptr);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M:%S", &tstruct);
    return buf;
}

void signalMsg(int signum)
{
    (void) signum;
    Tintin_reporter::getInstance().log_message_1("INFO", "Matt_daemon", "Signal handler");
    Tintin_reporter::getInstance().log_message_1("INFO", "Matt_daemon", "Quitting");
    std::remove("/var/lock/matt_daemon.lock");
    for (int i = getdtablesize(); i >= 0; --i)
    {
        close(i); /* close all descriptors */
    }
    exit(1);
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
    int pid;
    int i;
    int fd;
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
    // fs::path folderPath = "matt_daemon";
    // if (!fs::exists(folderPath))
    // {
        // if (!fs::create_directories(folderPath))
            // std::cerr << "Folder created successfully." << std::endl;
    // }

    for (i = getdtablesize(); i >= 0; --i)
    {
        if (i != STDERR_FILENO)
            close(i); /* close all descriptors */
    }
    i = open("/dev/null", O_RDWR);
    dup2(i, 0);
    dup2(i, 1);
    dup2(STDERR_FILENO, 2);
    fd = open("/var/lock/matt_daemon.lock", O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (flock(fd, LOCK_EX | LOCK_NB) == -1)
    {
        if (errno == EWOULDBLOCK)
        {
            Tintin_reporter::getInstance().log_message_1("ERROR", "Matt_daemon", "Error file locked");
            Tintin_reporter::getInstance().log_message_1("INFO", "Matt_daemon", "Quitting");
            std::cerr << "Can't open :/var/lock/matt_daemon.lock" << std::endl;
        }
        else
            std::cerr << "Error acquiring exclusive lock." << std::endl;
        exit(1);
    }

    handle_signals();
    Matt_daemon *daemon = Matt_daemon::getInstance();
    pid_t daemonPid = daemon->getDaemonPid();
    Tintin_reporter::getInstance().log_message_1("INFO", "Matt_daemon", "Entering Daemon mode.");
    Tintin_reporter::getInstance().log_message_1("INFO", "Matt_daemon", "started. PID: " + std::to_string(daemonPid));

    daemon->run();

    if (flock(fd, LOCK_UN) == -1)
    {
        std::cerr << "Error releasing exclusive lock." << std::endl;
    }
    close(fd);
    std::remove("/var/lock/matt_daemon.lock");
    return;
}