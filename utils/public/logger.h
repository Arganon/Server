/* SPDX-License-Identifier: GLWT(Good Luck With That) Public License
 * Copyright (c) Everyone, except Author
 * Everyone is permitted to copy, distribute, modify, merge, sell, publish,
 *sublicense or whatever they want with this software but at their OWN RISK.
 */
////////////////////////////////////////////////////////////////////////////////
/// @author Yuriy.Liahovskiy@gmail.com
/// @brief A singleton class to log any situations.
/// @copyright Free to use.
////////////////////////////////////////////////////////////////////////////////
#ifndef SERVER_LOGGER_H
#define SERVER_LOGGER_H

#include <map>
#include <mutex>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <linux/kernel.h>
#include <iostream>

#define LOG(loglevel, message, ...) (Logger::instance().Log(loglevel, __FILE__, __LINE__, message))

#define CHECK_TO_ERROR(loglevel, error, message, exit_or_continue)\
	if (error < 0) {\
		LOG(LOG_LVL::LOGS::DEBUG,\
			"%s. Error: %s", message\
			strerror(errno));\
		exit_or_continue;\
	}\

struct LOG_LVL {
///@brief Enum class with a few types of a log levels.
enum class LOGS {
	DEBUG,
	TRACE,
	RELEASE
};

///@brief Convert LOG_LVL type to the string.
static std::string to_string(LOGS _log_lvl) {
	std::map < LOGS, std::string > lvls {
		{ LOGS::DEBUG, "DEBUG" },
		{ LOGS::TRACE, "TRACE" },
		{ LOGS::RELEASE, "RELEASE" }
	};

	auto it = lvls.find(_log_lvl);

	if (it != lvls.end())
		return it->second;

	return "";
}
};

/// @brief Class for handle logs messages. Singleton.
class Logger {
private:
	/// @brief The value to store a mutex to control access to the log file.
	std::mutex m_mutex;

	/// @brief Default Constructor.
	Logger() = default; // TODO: open file in constructor

	/// @brief Default Destructor.
	~Logger() = default; // TODO: close file in destr.

public:
	/// @brief Logger is a singleton. Removed copy constructor.
	Logger(const Logger &) = delete;

	/// @brief Logger is a singleton. Removed move constructor.
	Logger(Logger &&) = delete;

	/// @brief Logger is a singleton. Removed copy assignment.
	Logger & operator = (const Logger &) = delete;

	/// @brief Logger is a singleton. Removed move assignment.
	Logger & operator = (Logger &&) = delete;

	/// @brief Method to take Logger instance.
	/// return Reference to the static Logger instance.
	static Logger & instance(void)
	{
		static Logger logger;
		return logger;
	}

	/// @brief Method for printing logs into the file.
	/// @param[in]  _log_level Log level for.
	/// @param[in]  _message Log message.
	/// @param[in]  _file The file name where are from received the message.
	/// @param[in]  _line The line number in the file.
	void Log(const LOG_LVL::LOGS _log_level,
			 const char *_file,
			 const unsigned long _line,
			 const char *_message,
			 ...)
	{
		std::lock_guard < std::mutex > lock(m_mutex);
		FILE *m_log_file = fopen("serverlogs.txt", "a+");
		time_t t = time(NULL);
		struct tm *tm = localtime(&t);

		fprintf(m_log_file, "%s%s File: %s. Line: %lu.\n",
				asctime(tm),
				LOG_LVL::to_string(_log_level).c_str(), _file, _line);

		va_list args;

		va_start(args, _message);
		vfprintf(m_log_file, _message, args);
		va_end(args);
		fclose(m_log_file);
	}
};

#endif // SERVER_LOGGER_H
