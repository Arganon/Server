/* SPDX-License-Identifier: GLWT(Good Luck With That) Public License
 * Copyright (c) Everyone, except Author
 * Everyone is permitted to copy, distribute, modify, merge, sell, publish,
 *sublicense or whatever they want with this software but at their OWN RISK.
 */
////////////////////////////////////////////////////////////////////////////////
/// @author Yuriy.Liahovskiy@gmail.com
/// @brief Session class.
/// @copyright Free to use.
////////////////////////////////////////////////////////////////////////////////
#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <unistd.h>
#include <sys/socket.h>

/// @brief Session class. Contains a socket file descriptor.
struct Session {
/// @brief Removed Constructor. We don't need default constructor without arguments.
Session() = delete;

/// @brief Custom Constructor.
/// @param[in] _fd Socket file descriptor.
explicit Session(const int _fd)
	: m_fd(_fd) {}

/// @brief Custom Destructor.
~Session() {
	if (!m_is_closed)
		close_session();
}

/// @brief Method to check is socket/session closed or not.
/// return True if socket/session closed of False if not.
/// @throw No throw.
bool is_closed(void) const noexcept { return m_is_closed; }

/// @brief Method to close a socket/session.
/// @throw No throw.
void close_session(void) noexcept {
	shutdown(m_fd, SHUT_RDWR);
	close(m_fd);
	m_is_closed = true;
}

/// @brief The value to store a socket file descriptor.
const int m_fd;

private:
/// @brief The value to store a socket/session status.
bool m_is_closed { false };

};

#endif // SESSION_H
