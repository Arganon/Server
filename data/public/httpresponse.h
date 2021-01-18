/* SPDX-License-Identifier: GLWT(Good Luck With That) Public License
 * Copyright (c) Everyone, except Author
 * Everyone is permitted to copy, distribute, modify, merge, sell, publish,
 *sublicense or whatever they want with this software but at their OWN RISK.
 */
////////////////////////////////////////////////////////////////////////////////
/// @author Yuriy.Liahovskiy@gmail.com
/// @brief A structure for storing and processing data to
///        form a response for a client.
/// @copyright Free to use.
////////////////////////////////////////////////////////////////////////////////
#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include "types.h"

/// @brief Struct is agregate all what we need to store and handle http response.
struct HTTPResponse {
/// @brief Default Constructor.
HTTPResponse() = default;

/// @brief Default Destructor.
~HTTPResponse() = default;

/// @brief Method reset all values to default.
void clear(void) {
	m_http_header.clear();
	m_body.clear();
	m_content_type.clear();
	m_status_code = RESPONSE_STATUS::CODE::OK;
	m_is_chunk = false;
	m_file_size = 0;
}

/// @brief The value to store a formed http header.
std::string m_http_header;

/// @brief The value to store a formed response data body.
std::string m_body;

/// @brief The value to store a content type of the response.
std::string m_content_type;

/// @brief The value to keep a Enum http response status code.
RESPONSE_STATUS::CODE m_status_code { RESPONSE_STATUS::CODE::OK };

/// @brief The value to keep a response setting - is response will be chunked.
bool m_is_chunk { false };

/// @brief The value to store a file total size in bytes.
size_t m_file_size { 0 };
};

#endif // HTTPRESPONSE_H
