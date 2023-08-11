#include "Tintin_reporter.hpp"

Tintin_reporter* Tintin_reporter::instance = nullptr;

Tintin_reporter::~Tintin_reporter()
{
    if (instance)
        delete instance;
}

Tintin_reporter &Tintin_reporter::getInstance()
{
    if (!instance)
        instance = new Tintin_reporter();
    return *instance;
}

std::string Tintin_reporter::currentDateTime()
{
    time_t now = time(nullptr);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M:%S", &tstruct);
    return buf;
}

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
         this->log_message_1("ERROR", "Matt_daemon", "Failed to open client_messages.log");
}

//  void Tintin_reporter::log_message_2(const std::string &log_type, const std::string &username, const std::string &message)
// {
//     std::string timestamp = currentDateTime();
//     std::string log_entry = "[" + timestamp + "] [" + log_type + "] - " + username + ": " + message;

//     size_t lastCharPos = log_entry.find_last_not_of(" \t\n\r\f\v");
//     if (lastCharPos != std::string::npos)
//         log_entry = log_entry.substr(0, lastCharPos + 1);

//     fs::path folderPath = "/var/log/matt_daemon";
//     std::ofstream log_file("/var/log/matt_daemon/matt_daemon.log", std::ios::app);
//     if (!fs::exists(folderPath))
//     {
//         if (!fs::create_directories(folderPath))
//         {
//             std::cout << "Folder created successfully." << std::endl;
//         }
//         else
//         {
//             std::cerr << "Failed to create folder." << std::endl;
//         }
//     }
//     if (log_file.is_open() && !log_entry.empty())
//     {
//         log_file << log_entry << '\n';
//         log_file.close();
//     }
// }

Tintin_reporter::Tintin_reporter() {}
