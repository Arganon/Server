/* SPDX-License-Identifier: GLWT(Good Luck With That) Public License
 * Copyright (c) Everyone, except Author
 * Everyone is permitted to copy, distribute, modify, merge, sell, publish,
 *sublicense or whatever they want with this software but at their OWN RISK.
 */
////////////////////////////////////////////////////////////////////////////////
/// @author Yuriy.Liahovskiy@gmail.com
/// @brief A singleton class to handle request
///        from the client and fill response.
/// @copyright Free to use.
////////////////////////////////////////////////////////////////////////////////
#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "storage.h"
#include "../../data/public/data_structures.h"

/// @brief Class to handle received http request from the client.
class RequestHandler {
private:
	/// @brief Class is singleton.
	///        Default Constructor moved to the private section.
	RequestHandler() = default;

	/// @brief Class is singleton.
	///        Default Destructor moved to the private section.
	~RequestHandler() = default;

public:
	/// @brief RequestHandler is a singleton. Removed copy constructor.
	RequestHandler(const RequestHandler &) = delete;

	/// @brief RequestHandler is a singleton. Removed move constructor.
	RequestHandler(RequestHandler &&) = delete;

	/// @brief RequestHandler is a singleton. Removed copy assignment.
	RequestHandler & operator = (const RequestHandler &) = delete;

	/// @brief RequestHandler is a singleton. Removed move assignment.
	RequestHandler & operator = (RequestHandler &&) = delete;

	/// @brief Method to take RequestHandler instance.
	/// return Reference to the static RequestHandler instance.
	static RequestHandler & instance(void);

	/// @brief Method to handle request from client.
	/// @param[in] _client_data Pointer to the structure which contains
	///            all values and data structures to prepare response.
	void handle_request(Client *_client_data);

private:
	/// @brief Method to handle GET http request from client.
	/// @param[in] _request Reference to the const request structure.
	/// @param[in] _response Reference to the structure to fill response.
	void handle_GET_request(const HTTPRequest & _request, HTTPResponse & _response);

	/// @brief Method to get a file from the storage.
	/// @param[in] _request Reference to the const request structure.
	/// @param[in] _response Reference to the structure to fill response.
	void get_file(const HTTPRequest & _request,
				  HTTPResponse & _response);

	/// @brief Method to prepare http header in response.
	/// @param[in] _response Reference to the structure to fill response.
	void prepare_header(HTTPResponse & _response);

	/// @brief Method to prepare response body and send it to the client.
	/// @param[in] _client_data Pointer to the structure which contains
	///            all values and data structures to prepare response.
	void prepare_and_send_body(Client *_client_data);

	/// @brief Method to handle chunked response body.
	/// @param[in] _client_data Pointer to the structure which contains
	///            all values and data structures to prepare response.
	void handle_chunked_response(Client *_client_data);

	/// @brief Method to prepare chunked response body and send it to the client.
	/// @param[in] _client_data Pointer to the structure which contains
	///            all values and data structures to prepare response.
	/// @param[in] _start_pos The start position to read data from the file.
	/// @param[in] _size_to_read The total size of the data to read from the file.
	void prepare_and_send_chunked_response(Client *_client_data,
										   size_t _start_pos,
										   size_t _size_to_read);

	/// @brief  Value for Storage class. Gives access to the files.
//	Storage m_file_storage;
};

#endif // REQUESTHANDLER_H
