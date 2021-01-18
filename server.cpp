#include <fcntl.h>
#include <netdb.h>
#include <thread>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>

#include <mbedtls/net.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>

#include "server.h"
#include "utils/public/logger.h"
#include "utils/public/requestparser.h"
#include "common/public/requesthandler.h"

Server::Server(const ServerConnectInfo & _data) : m_data(_data) {}

int Server::set_nonblock(const int _fd) const noexcept {
	int flags;
#if defined(O_NONBLOCK)
	if (-1 == (flags = fcntl(_fd, F_GETFL, 0))) {
		LOG(LOG_LVL::LOGS::DEBUG, "Nonblock setting causes an Error: %s", strerror(errno));
		flags = 0;
	}

	return fcntl(_fd, F_SETFL, flags | O_NONBLOCK);
#else
	flags = 1;
	return ioctl(fd, FIOBIO, &flags);

#endif
}

void Server::stop() {
	LOG(LOG_LVL::LOGS::DEBUG, "The server is stoped.");
	m_is_run = false;
}

void Server::receive_and_handle_data(Client *_client) {
	std::thread t
	(
		[](Client * _client_data) {
			// We use poll instead of select to avoid undefined behavior
			// during using with multiple threads.
			struct pollfd poll_fds[1];

			poll_fds[0].fd = _client_data->m_session.m_fd;
			poll_fds[0].events = POLLIN;

			while (true) {
				int result = _client_data->receive_data();

				if (!result)
					break;

				RequestParser::instance().parse_request(_client_data);
				RequestHandler::instance().handle_request(_client_data);

				int ret = poll(poll_fds, 1, MAGIC_TIME_LIMIT);
				if (ret < 0) {
					LOG(LOG_LVL::LOGS::DEBUG, "Poll the file descriptors causes an Error: %s", strerror(errno));
					break;
				}

				if ( ret == 0 ) {
					// timeout is elapsed
					break;
				}
				else {
					if (poll_fds[0].revents & POLLIN) {
						poll_fds[0].revents = 0;
					}
				}
				_client_data->clear();
			}

			delete _client_data;
		},
		_client
	);

	t.detach();
}

void Server::process_listening(const int _fd_regular, const int _fd_encrypted) {
	struct pollfd poll_fds[LISTEN_SOCKETS_COUNT];

	poll_fds[0].fd = _fd_regular;
	poll_fds[0].events = POLLIN;

	poll_fds[1].fd = _fd_encrypted;
	poll_fds[1].events = POLLIN;

	while (m_is_run) {
		poll(poll_fds, LISTEN_SOCKETS_COUNT, -1);

		for (int i = 0; i < LISTEN_SOCKETS_COUNT; ++i) {
			if (poll_fds[i].revents & POLLIN) {
				if (poll_fds[i].fd == _fd_regular) {
					int slave_socket = accept(_fd_regular, 0, 0);
					Client *client = new ClientRegular(slave_socket);
					receive_and_handle_data(client);
				}
				if (poll_fds[i].fd == _fd_encrypted) {
					int slave_socket = accept(_fd_encrypted, 0, 0);
					Client *client = new ClientEncrypted(slave_socket);
					receive_and_handle_data(client);
				}
				poll_fds[i].revents = 0;
			}
		}
	}
}

int Server::receive_regular_socket(int _port) const {
	struct sockaddr_in6 sockaddr;
	sockaddr.sin6_family = AF_INET6;
	sockaddr.sin6_addr = in6addr_any;
	sockaddr.sin6_port = htons(_port);

	const int opt = 1;

	int main_socket = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);

	if (main_socket < 0) {
		LOG(LOG_LVL::LOGS::DEBUG,
			"Creating a new socket causes an Error: %s.",
			strerror(errno));
		return -1;
	}

	set_nonblock(main_socket);

	if (setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		LOG(LOG_LVL::LOGS::DEBUG,
			"Setting options for the socket causes an Error: %s.",
			strerror(errno));
		return -1;
	}

	if (bind(main_socket, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in6)) < 0) {
		LOG(LOG_LVL::LOGS::DEBUG,
			"Socket binding returns an Error: %s.",
			strerror(errno));

		close(main_socket);
		return -1;
	}

	if (listen(main_socket, SOMAXCONN) < 0) {
		LOG(LOG_LVL::LOGS::DEBUG,
			"Try to port listening returned an Error: %s",
			strerror(errno));

		return -1;
	}

	return main_socket;
}

// This method for future possible differenes betwen
// regular and encrypted socket initializations during 'mbedtls' realization.
int Server::receive_encrypted_socket(int _port) const {
	return receive_regular_socket(_port);
}

void Server::run(void) {
	if (!m_data.m_folder.empty()) {
		 Storage::instance().set_root_folder(m_data.m_folder);
	}

	int main_regular_socket = receive_regular_socket(m_data.m_port_regular);

	if (main_regular_socket < 0) {
		LOG(LOG_LVL::LOGS::DEBUG, "The regular socket descriptor returned an Error.");
		return;
	}

	int main_encrypted_socket = receive_encrypted_socket(m_data.m_port_encrypted);

	if (main_encrypted_socket < 0) {
		LOG(LOG_LVL::LOGS::DEBUG, "The encrypted socket descriptor returned an Error.");
		return;
	}

	process_listening(main_regular_socket, main_encrypted_socket);

	close(main_regular_socket);
}
