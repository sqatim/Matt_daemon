#include "Tintin_reporter.hpp"

void Tintin_reporter::log_message_1(const std::string &log_type, const std::string &username, const std::string &message)
{
    std::string timestamp = currentDateTime();
    std::string log_entry = "[" + timestamp + "] [" + log_type + "] - " + username + ": " + message;

    size_t lastCharPos = log_entry.find_last_not_of(" \t\n\r\f\v");
    if (lastCharPos != std::string::npos)
        log_entry = log_entry.substr(0, lastCharPos + 1);

    fs::path folderPath = "/var/log/matt_daemon";
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
    std::ofstream log_file(this->_logFile, std::ios::app);
    if (log_file.is_open() && !log_entry.empty())
    {
        log_file << log_entry << '\n';
        log_file.close();
    }
    else
    {
        log_message_1("ERROR", "Matt_daemon", "Failed to open client_messages.log");
        for (size_t i = 1; i <= _clients.size(); i++)
            close(fds[i].fd);

        close_socket();
    }
}

 void Tintin_reporter::log_message_2(const std::string &log_type, const std::string &username, const std::string &message)
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