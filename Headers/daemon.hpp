#ifndef DAEMON_HPP
#define DAEMON_HPP

#include <iostream>
#include <fstream>
#include <unistd.h>

void startDaemon();
void log_message(const std::string &log_type, const std::string &username, const std::string &message);

#endif