/* SPDX-License-Identifier: GLWT(Good Luck With That) Public License
 * Copyright (c) Everyone, except Author
 * Everyone is permitted to copy, distribute, modify, merge, sell, publish,
 *sublicense or whatever they want with this software but at their OWN RISK.
 */
////////////////////////////////////////////////////////////////////////////////
/// @author Yuriy.Liahovskiy@gmail.com
/// @brief File with defined simple types and enums status codes.
/// @copyright Free to use.
////////////////////////////////////////////////////////////////////////////////
#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <string>

#define LISTEN_SOCKETS_COUNT 2
#define MAX_EVENTS 10000
#define MAX_BUFF_SIZE 1024
#define MAX_COMMAND_LENGTH 1024
#define MAX_RESPONSE_LENGTH 256
#define MAGIC_TIME_LIMIT 42000
#define MAX_SIZE_FOR_READ ((size_t)(1024*256))

/// @brief Stucture with http response types.
///        Implemented translate enum to string.
struct RESPONSE_STATUS {
enum CODE {
OK = 200,
BAD_REQUEST = 400,
NOT_FOUND = 404,
NOT_IMPLEMENTED = 501
};

static std::string to_string(CODE _http_type) noexcept {
	std::map < CODE, std::string > types {
		{ CODE::OK, "OK 200" },
		{ CODE::BAD_REQUEST, "400 Bad Request" },
		{ CODE::NOT_FOUND, "404 Not Found" },
		{ CODE::NOT_IMPLEMENTED, "501 Not Implemented" }
	};

	auto it = types.find(_http_type);

	if (it != types.end())
		return it->second;

	return "";
}
};

/// @brief Stucture with http request types.
///        Implemented translate enum to string and string to enum.
struct REQUEST_TYPE {
enum HTTP_TYPE {
GET = 0,
POST,
UNDEFINED
};

static std::string to_string(HTTP_TYPE _http_type) noexcept {
	std::map < HTTP_TYPE, std::string > types {
		{ HTTP_TYPE::GET, "GET" },
		{ HTTP_TYPE::POST, "POST" }
	};

	auto it = types.find(_http_type);

	if (it != types.end())
		return it->second;

	return "";
}

static HTTP_TYPE to_enum(std::string _http_type) noexcept {
	std::map < std::string, HTTP_TYPE > types {
		{ "GET", HTTP_TYPE::GET },
		{ "POST", HTTP_TYPE::POST}
	};

	auto it = types.find(_http_type);

	if (it != types.end())
		return it->second;

	return HTTP_TYPE::UNDEFINED;
}
};

/// @brief Stucture with request command types.
struct CMD_TYPE {
enum TYPE {
	GET_FILE = 0,
	EMPTY,
	ERROR,
	UNDEFINED
};
};

/// @brief Stucture with content types.
struct CONTENT_TYPE {
static std::string get_type(std::string & _type) {
	const std::map < std::string, std::string > content_types {
		{ "mpeg", "audio/mpeg" },

		{ "gif", "image/gif" },
		{ "jpeg", "image/jpeg" },
		{ "png", "image/png" },
		{ "tiff", "image/tiff" },

		{ "html", "text/html; charset=utf-8;" },

		{ "mpeg", "video/mpeg" },
		{ "mp4", "video/mp4" }
	};

	auto it = content_types.find(_type);

	if (it != content_types.end())
		return it->second;

	return "";
}
};

#endif // TYPES_H
