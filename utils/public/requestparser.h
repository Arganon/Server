/* SPDX-License-Identifier: GLWT(Good Luck With That) Public License
 * Copyright (c) Everyone, except Author
 * Everyone is permitted to copy, distribute, modify, merge, sell, publish,
 *sublicense or whatever they want with this software but at their OWN RISK.
 */
////////////////////////////////////////////////////////////////////////////////
/// @author Yuriy.Liahovskiy@gmail.com
/// @brief A singleton class to parse request from the client.
/// @copyright Free to use.
////////////////////////////////////////////////////////////////////////////////
#ifndef REQUESTPARSER_H
#define REQUESTPARSER_H

#include <string>

#include "../../data/public/types.h"
#include "../../utils/public/requestparser.h"

struct Client;

/// @brief Singleton class to parse request from the client
class RequestParser {
private:
	/// @brief Class is singleton.
	///        Default Constructor moved to the private section.
	RequestParser() = default;

	/// @brief Class is singleton.
	///        Default Destructor moved to the private section.
	~RequestParser() = default;

public:
	/// @brief RequestParser is a singleton. Removed copy constructor.
	RequestParser(const RequestParser &) = delete;

	/// @brief RequestParser is a singleton. Removed move constructor.
	RequestParser(RequestParser &&) = delete;

	/// @brief RequestParser is a singleton. Removed copy assignment.
	RequestParser & operator = (const RequestParser &) = delete;

	/// @brief RequestParser is a singleton. Removed move assignment.
	RequestParser & operator = (RequestParser &&) = delete;

	/// @brief Method to take RequestParser instance.
	/// return Reference to the static RequestParser instance.
	static RequestParser & instance(void);

	/// @brief Method to parse request from client.
	/// @param[in] _client_data Pointer to the structure which contains
	///            all values and data structures to fill request.
	void parse_request(Client *_client_data);

private:
	/// @brief Method tooks http type from the request.
	/// @param[in] _client_data Pointer to the structure which contains
	///            request structure.
	/// return The position where the file name starts.
	size_t parse_request_http_type(Client *_client_data);

	/// @brief The method extract the file name from the request.
	/// @param[in] _source The string with request received from the client.
	/// @param[in] _position The position where the file name starts.
	/// return The string with file name.
	std::string get_file_name(std::string & _source, size_t _position);

	/// @brief The method takes the file extension and
	///        trying to find it in the content type list.
	/// @param[in] _filename Filename with extension.
	/// return File extension or empty string if extension not exist.
	std::string get_content_type(std::string & _filename);

	/// @brief Check the file name to specific symbols.
	/// @param[in] _filename The file name wich need to check.
	/// return True if the file name is valid or False if not.
	bool is_filename_valid(std::string & _filename);
};

#endif // REQUESTPARSER_H
