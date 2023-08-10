#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class Tintin_reporter
{
public:
    ~Tintin_reporter();

    // Tintin_reporter &operator=(const Tintin_reporter &other);
    // Tintin_reporter(const Tintin_reporter &other);
    static Tintin_reporter& getInstance();

    void log_message_1(const std::string &log_type, const std::string &username, const std::string &message);
    void log_message_2(const std::string &log_type, const std::string &username, const std::string &message);
    std::string currentDateTime();

private:
    Tintin_reporter();
    static Tintin_reporter* instance;
    std::string _logFile = "/var/log/matt_daemon/matt_daemon.log";
};