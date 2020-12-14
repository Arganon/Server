#ifndef SERVER_LOGGER_H
#define SERVER_LOGGER_H

#include <map>
#include <time.h>
#include <stdio.h>
#include <linux/kernel.h>

// TODO: сделать возможность добавления большого количества аргументов в LOG
#define LOG(loglevel, message) Logger::Log(loglevel, message, __FILE__, __LINE__)

///@brief Enum class with a few types of a log levels.
enum class LOG_LVL {
    DEBUG,
    TRACE,
    RELEASE
};

///@brief Convert LOG_LVL type to the string.
static std::string to_string(LOG_LVL _log_lvl) {
    std::map<LOG_LVL, std::string> lvls {
        { LOG_LVL::DEBUG, "DEBUG" },
        { LOG_LVL::TRACE, "TRACE" }
    };

    auto it = lvls.find(_log_lvl);
    if (it != lvls.end()) {
        return it->second;
    }
    return "";
}

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
        static void Log(const LOG_LVL _log_level,
                        const char* _message,
                        const char* _file,
                        const unsigned long int _line)
        {
            // TODO: printk(...)
            FILE *log_file = fopen("serverlogs.txt", "a+");
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            fprintf(log_file, "%s%sMessage: %s. File: %s. Line: %lu.\n",
                    asctime(tm), to_string(_log_level).c_str(), _message, _file, _line);

            fclose(log_file);
        }
};

#endif // SERVER_LOGGER_H
