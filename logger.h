#ifndef SERVER_LOGGER_H
#define SERVER_LOGGER_H

#include <map>
#include <time.h>
#include <stdio.h>
#include <linux/kernel.h>

/*
 TODO: check the speed in case 'if/else' blocks and switch/case bloks used in the server logic during any checks to nullptr or smthng else.

EXAMPLE:

class enum ERROR_TYPE {
    CRITICAL,
    WARNING,
    EXCEPTION,
    etc...
};

void error_handler(const ERROR_TYPE _error_type, const std::string &_message) {
    switch(_error_type) {
    case CRITICAL:
        throw std::runtime_error(_message.c_str());

    case ...:
    }
}

void check_to_error(const int _returned_descriptor,
                    const ERROR_TYPE _error_type,
                    const std::string &_message) {
    switch (_returned_descriptor) {
    case: 0
        LOG("it's ok");
        break;

    default:
        error_handler(_error_type, _message);

    }

}

const static std::string CONNECTION_ERROR = "Socket wasn't opened!"

int socet = create_and_listen_socket();
check_to_error(socet, ERROR_TYPE::CRITICAL, CONNECTION_ERROR);

*/


// TODO: сделать возможность добавления большого количества аргументов в LOG
#define LOG(loglevel, message) Logger::Log(loglevel, message, __FILE__, __LINE__)

struct LOG_LVL {
    ///@brief Enum class with a few types of a log levels.
    enum class LOGS {
        DEBUG,
        TRACE,
        RELEASE
    };

    ///@brief Convert LOG_LVL type to the string.
    static std::string to_string(LOGS _log_lvl) {
        std::map<LOGS, std::string> lvls {
            { LOGS::DEBUG, "DEBUG" },
            { LOGS::TRACE, "TRACE" },
            { LOGS::RELEASE, "RELEASE" }
        };

        auto it = lvls.find(_log_lvl);
        if (it != lvls.end()) {
            return it->second;
        }
        return "";
    }
};

///@brief Class for handle logs messages
class Logger
{
    public:
        Logger() = delete;

        /// @brief For print logs into the file.
        /// @param[in]  _log_level Log level for.
        /// @param[in]  _message Log message.
        /// @param[in]  _file The file name where are from received the message.
        /// @param[in]  _line The line number in the file.
        static void Log(const LOG_LVL::LOGS _log_level,
                        const char* _message,
                        const char* _file,
                        const unsigned long int _line)
        {
            // TODO: printk(...)
            FILE *log_file = fopen("serverlogs.txt", "a+");
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            fprintf(log_file, "%s%sMessage: %s. File: %s. Line: %lu.\n",
                    asctime(tm), LOG_LVL::to_string(_log_level).c_str(), _message, _file, _line);

            fclose(log_file);
        }
};

#endif // SERVER_LOGGER_H
