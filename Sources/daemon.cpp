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

void log_message(const std::string &log_type, const std::string &username, const std::string &message)
{
    std::string timestamp = currentDateTime();
    std::string log_entry = "[" + timestamp + "] [" + log_type + "] - " + username + ": " + message;

    size_t lastCharPos = log_entry.find_last_not_of(" \t\n\r\f\v");
    if (lastCharPos != std::string::npos)
        log_entry = log_entry.substr(0, lastCharPos + 1);

    fs::path folderPath = "/var/log/matt_daemon";
    std::ofstream log_file("/var/log/matt_daemon/matt_daemon.log", std::ios::app);
    if (!fs::exists(folderPath))
    {
        if (!fs::create_directories(folderPath))
        {
            std::cout << "Folder created successfully." << std::endl;
        }
        else
        {
            std::cerr << "Failed to create folder." << std::endl;
        }
    }
    if (log_file.is_open() && !log_entry.empty())
    {
        log_file << log_entry << '\n';
        log_file.close();
    }
}

void signalMsg(int signum)
{
    log_message("INFO", "Matt_daemon", "Signal handler");
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
        if (!fs::create_directories(folderPath))
            std::cerr << "Folder created successfully." << std::endl;
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
            log_message("ERROR", "Matt_daemon", "Error file locked");
            log_message("INFO", "Matt_daemon", "Quitting");
            std::cerr << "Can't open :/var/lock/matt_daemon.lock" << std::endl;
        }
        else
            std::cerr << "Error acquiring exclusive lock." << std::endl;
        exit(1);
    }

    handle_signals();
    Matt_daemon *daemon = Matt_daemon::getInstance();
    pid_t daemonPid = daemon->getDaemonPid();
    daemon->log_message("INFO", "Matt_daemon", "Entering Daemon mode.");
    daemon->log_message("INFO", "Matt_daemon", "started. PID: " + std::to_string(daemonPid));
    daemon->run();

    if (flock(fd, LOCK_UN) == -1)
    {
        std::cerr << "Error releasing exclusive lock." << std::endl;
    }
    close(fd);
    std::remove("/var/lock/matt_daemon/matt_daemon.lock");
    return;
}