#pragma once

class Tintin_reporter
{
    public:
        Tintin_reporter();

        ~Tintin_reporter();

        void log_message_1(const std::string &log_type, const std::string &username, const std::string &message);
        void log_message_2(const std::string &log_type, const std::string &username, const std::string &message);

    private:
        
};