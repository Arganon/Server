/* SPDX-License-Identifier: GLWT(Good Luck With That) Public License
 * Copyright (c) Everyone, except Author
 * Everyone is permitted to copy, distribute, modify, merge, sell, publish,
 *sublicense or whatever they want with this software but at their OWN RISK.
 */
////////////////////////////////////////////////////////////////////////////////
/// @author Yuriy.Liahovskiy@gmail.com
/// @brief Structure to keep and handle data received from client side.
/// @copyright Free to use.
////////////////////////////////////////////////////////////////////////////////
#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>

#include "types.h"

/// @brief Struct is agregate all what we need to store and handle http request.
struct HTTPRequest {
/// @brief Method reset all values to default.
void clear(void) {
	m_filename.clear();
	m_type = REQUEST_TYPE::HTTP_TYPE::UNDEFINED;
	m_command = CMD_TYPE::TYPE::UNDEFINED;
}

/// @brief String with file name.
std::string m_filename;

/// @brief The value to keep Enum http request type.
REQUEST_TYPE::HTTP_TYPE m_type { REQUEST_TYPE::HTTP_TYPE::UNDEFINED };

/// @brief The value to keep Enum command type.
CMD_TYPE::TYPE m_command { CMD_TYPE::TYPE::UNDEFINED };
};

#endif // HTTPREQUEST_H
