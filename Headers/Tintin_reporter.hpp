#pragma once

#include "Matt_daemon.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class Tintin_reporter
{
public:
    ~Tintin_reporter();

    Tintin_reporter &operator=(const Tintin_reporter &other);
    Tintin_reporter(const Tintin_reporter &other);

    static Tintin_reporter *getInstance()
    {
        return new Tintin_reporter();
    }

    void log_message_1(const std::string &log_type, const std::string &username, const std::string &message, Matt_daemon &Mt_daemon);
    void log_message_2(const std::string &log_type, const std::string &username, const std::string &message);
    std::string currentDateTime();

private:
    Tintin_reporter();
};