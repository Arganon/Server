/* SPDX-License-Identifier: GLWT(Good Luck With That) Public License
 * Copyright (c) Everyone, except Author
 * Everyone is permitted to copy, distribute, modify, merge, sell, publish,
 *sublicense or whatever they want with this software but at their OWN RISK.
 */
////////////////////////////////////////////////////////////////////////////////
/// @author Yuriy.Liahovskiy@gmail.com
/// @brief The Server class.
/// @copyright Free to use.
////////////////////////////////////////////////////////////////////////////////
#ifndef SERVER_H
#define SERVER_H

#include <set>
#include <mutex>

#include "data/public/types.h"
#include "data/public/data_structures.h"

struct epoll_event;

/// @brief Server class.
class Server {
public:
	/// @brief Constructor. The commented out text shows the possibility
	///        to implement a few external modules.
	/// @param[in] _data The structure which contains port and host.
	Server(const ServerConnectInfo & _data);

	~Server() = default;

	/// @brief Run the server.
	void run(void);

	/// @brief To stop the server when it works in demon mode.
	void stop(void);

private:
	/// @brief Set the socket to non block mode using his file descriptor.
	/// @param[in] _fd The socket file desriptor.
	/// return 0 if OK or -1 if ERROR.
	int set_nonblock(const int _fd) const noexcept;

	/// @brief Listen the open socket to receive the response.
	/// @param[in] _fd_regular The regular socket file desriptor.
	/// @param[in] _fd_encrypted The encrypted socket file desriptor.
	void process_listening(const int _fd_regular,
						   const int _fd_encrypted);

	/// @brief Receive regular socket file descriptor.
	/// return The socket file descriptor or -1 if ERROR.
	int receive_regular_socket(int _port) const;

	/// @brief Receive encrypted socket file descriptor.
	/// return The socket file descriptor or -1 if ERROR.
	int receive_encrypted_socket(int _port) const;

	/// @brief Receive and handle response from the client in the keep-alive mode.
	/// @param[in] _fd The pointer to the Client structure.
	void receive_and_handle_data(Client *_client);

	/// @brief The reference value to structure with port and host for the server.
	const ServerConnectInfo & m_data;

	std::mutex m_mutex;

	std::set < int > m_active_sockets;

	/// @brief The value to store server status.
	volatile bool m_is_run { true };
};

#endif // SERVER_H
