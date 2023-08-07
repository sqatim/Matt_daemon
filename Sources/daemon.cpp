#include "daemon.hpp"
#include <chrono>
#include <thread>
#include "Matt_daemon.hpp"
#include <signal.h>

void signalMsg(int signum)
{
    std::ofstream file("client_messages.log", std::ios::app);
    if (file.is_open())
    {
        file << "Signal\n";
        file.close();
    }
    else
        throw std::runtime_error("Failed to open client_messages.log");
}

void handle_signals()
{

    struct sigaction sa;
    sa.sa_handler = signalMsg;
    // sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;

    // Ignore all signals
    for (int signum = 1; signum <= 31; signum++)
    {
        // if (signum != 9 && signum != 17)
        sigaction(signum, &sa, NULL);
    }
}

void startDaemon()
{
    int pid;
    pid = fork();
    if (pid == 0)
    {
        // struct sigaction sa = {
        //     .sa_handler = signalMsg,
        //     // .sa_flags = SA_RESTART | SA_NOCLDSTOP,
        // };
        handle_signals();
        // if (sigemptyset(&sa.sa_mask) < 0)
        // {
        //     perror("sigemptyset");
        //     return;
        // }

        // if (sigaction(SIGQUIT, &sa, NULL) < 0)
        // {
        //     perror("sigaction");
        //     return;
        // }
        // signal(SIGTERM, signalMsg);
        // std::ofstream myFile("Matt_daemon.log");
        Matt_daemon daemon;
        daemon.run();
    }
    return;
}