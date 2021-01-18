/* SPDX-License-Identifier: GLWT(Good Luck With That) Public License
 * Copyright (c) Everyone, except Author
 * Everyone is permitted to copy, distribute, modify, merge, sell, publish,
 *sublicense or whatever they want with this software but at their OWN RISK.
 */
////////////////////////////////////////////////////////////////////////////////
/// @author Yuriy.Liahovskiy@gmail.com
/// @brief File contains different simple data structures.
/// @copyright Free to use.
////////////////////////////////////////////////////////////////////////////////
#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <string>

#include <sys/socket.h>

#include <signal.h>
#include <netinet/in.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/crypto.h>

#include "session.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "../../utils/public/logger.h"

/// @brief ServerConnectInfo structure for port and ip.
///        Has default value for port (12345) and ip (127.0.0.1)
struct ServerConnectInfo {
public:
/// @brief Custom Constructor.
ServerConnectInfo() : m_port_regular(12345), m_port_encrypted(12346), m_host("127.0.0.1") {}

/// @brief The value to store a port. Default port is 12345.
///        At this moment port 80 is busy.
int m_port_regular;

/// @brief The value to store a port for encrypted connections. Default port is 12346.
///        At this moment port 443 is busy.
int m_port_encrypted;

/// @brief The value to store a host. Default host is 127.0.0.1
std::string m_host;

/// @brief The value to store a root files folder.
std::string m_folder;
};

/// @brief Abstract Client struct.
struct Client {
Client(int _fd)
	: m_session(_fd) {}

/// @brief Virtual Destructor.
virtual ~Client() = default;

/// @brief Virtual method to receive client request.
/// return True if request is received or False if not.
virtual bool receive_data(void) = 0;

/// @brief Virtual method to send server response header.
/// return True if response is sent or False if not.
virtual bool send_header(void) = 0;

/// @brief Virtual method to send server response body.
/// return True if response is sent or False if not.
virtual bool send_body(void) = 0;

/// @brief Virtual method reset all values to default.
virtual void clear(void) = 0;

/// @brief String buffer for http request.
std::string m_buffer_for_request;

/// @brief Value for Session structure to store opened socket file descriptor.
Session m_session;

/// @brief Value for Request structure.
HTTPRequest m_request;

/// @brief Value for Response structure.
HTTPResponse m_response;
};


/// @brief ClientRegular structure contains all values and data structures
///        to handle request and response.
struct ClientRegular : public Client {
/// @brief Custom Constructor.
/// @param[in] _fd Socket file descriptor for Session structure.
explicit ClientRegular(int _fd)
	: Client(_fd) {}

/// @brief Method to receive client request.
/// return True if request is received or False if not.
bool receive_data(void) override {
	char buff[MAX_BUFF_SIZE];
	bool result = false;
	int receive_res = recv(m_session.m_fd, buff, MAX_BUFF_SIZE, MSG_NOSIGNAL);

	if ((receive_res <= 0) && (errno != EAGAIN)) {
		LOG(LOG_LVL::LOGS::DEBUG, "Request recieving is rising an Error: %s", strerror(errno));
	}
	else {
		result = true;
		m_buffer_for_request = buff;
	}

	return result;
}

/// @brief Virtual method to send server response header.
/// return True if response is sent or False if not.
bool send_header(void) override {
	int result = send(m_session.m_fd,
		 m_response.m_http_header.c_str(),
		 m_response.m_http_header.size(),
		 MSG_NOSIGNAL);

	return result != -1;
}

/// @brief Virtual method to send server response.
/// return True if response is sent or False if not.
bool send_body(void) override {
	int result = send(m_session.m_fd,
		 m_response.m_body.c_str(),
		 m_response.m_body.size(),
		 MSG_NOSIGNAL);

	return result != -1;
}

/// @brief Method reset all values to default.
void clear(void) override {
	m_request.clear();
	m_response.clear();
}
};


/// @brief ClientEncrypted structure contains all values and data structures
///        to handle request and response.
struct ClientEncrypted : public Client {
/// @brief Custom Constructor.
/// @param[in] _fd Socket file descriptor for Session structure.
explicit ClientEncrypted(int _fd,
						 std::string _path_to_cert = "",
						 std::string _path_to_key = "")
	: Client(_fd)
{
	if (_path_to_cert.empty()) {
		_path_to_cert = get_current_dir() + "/selfsigned.crt"; // TODO: create crt by code
	}

	if (_path_to_key.empty()) {
		_path_to_key = get_current_dir() + "/selfsigned.key"; // TODO: create key by code
	}

	sigemptyset(&m_set);
	sigaddset(&m_set, SIGPIPE);

	init_ssl(_path_to_cert, _path_to_key);
}

/// @brief Custom Destructor.
~ClientEncrypted() {
	SSL_shutdown(m_ssl);
	if (m_ssl_ctx != nullptr) {
		SSL_CTX_free(m_ssl_ctx);
	}

	if (m_ssl != nullptr) {
		SSL_shutdown(m_ssl);
		SSL_free(m_ssl);
	}
}
/// @brief Method return the string with path to executed binary file.
/// return String with path.
std::string get_current_dir(void) const {
	char curr_folder_path[PATH_MAX];
	std::string execution_folder;

	if (getcwd(curr_folder_path, sizeof(curr_folder_path)) == nullptr) {
		LOG(LOG_LVL::LOGS::DEBUG, "Can't receive path to execution folder.");
	}
	else {
		execution_folder = curr_folder_path;
	}
	return execution_folder;
}
/// @brief Method to initialize Open SSL library.
/// @param[in] _path_to_cert String with path to the sertificate file.
/// @param[in] _path_to_key String with path to the private key file.
/// return True if initializing was successful and False if not.
bool init_ssl(std::string &_path_to_cert, std::string &_path_to_key) {
	SSL_library_init();
	SSL_load_error_strings();

	m_ssl_ctx = SSL_CTX_new(TLSv1_2_server_method());

	if (m_ssl_ctx == NULL) {
		ERR_print_errors_fp(stderr);
		LOG(LOG_LVL::LOGS::DEBUG, "SSL context initializing is failed: %s", strerror(errno));
		return false;
	}

	if (SSL_CTX_use_certificate_file(m_ssl_ctx, _path_to_cert.c_str(),
									 SSL_FILETYPE_PEM) <= 0)
	{
		LOG(LOG_LVL::LOGS::DEBUG, "SSL_CTX_use_certificate_file cause an Error: %s", strerror(errno));
		return false;
	}
	if (SSL_CTX_use_PrivateKey_file(m_ssl_ctx, _path_to_key.c_str(),
									SSL_FILETYPE_PEM) <= 0)
	{
		LOG(LOG_LVL::LOGS::DEBUG, "SSL_CTX_use_PrivateKey_file cause an Error: %s", strerror(errno));
		return false;
	}

	if (!SSL_CTX_check_private_key(m_ssl_ctx)) {
		LOG(LOG_LVL::LOGS::DEBUG, "SSL_CTX_check_private_key cause an Error: %s", strerror(errno));
		return false;
	}

	m_ssl = SSL_new(m_ssl_ctx);
	SSL_set_fd(m_ssl, m_session.m_fd);

	return true;
}

/// @brief Method to receive client request.
/// return True if request is received or False if not.
bool receive_data(void) override {
	char buff[MAX_BUFF_SIZE] = { 0 };

	if (!m_is_ssl_accepted) {
		m_is_ssl_accepted = true;
		if (SSL_accept(m_ssl) == -1) {
			ERR_print_errors_fp(stderr);
			return false;
		}
	}

	int result = SSL_read(m_ssl, buff, sizeof(buff));

	if (pthread_sigmask(SIG_BLOCK, &m_set, NULL) != 0)
		return false;

	if ((result <= 0) && (errno != EAGAIN)) {
		LOG(LOG_LVL::LOGS::DEBUG, "Request recieving is rising an Error: %s", strerror(errno));
		m_is_ssl_accepted = false;
		return false;
	}
	else {
		m_buffer_for_request = buff;
	}

	return true;
}

/// @brief Virtual method to send server response header.
/// return True if response is sent or False if not.
bool send_header(void) override {
	int result = 0;

	result = SSL_write(m_ssl, m_response.m_http_header.c_str(), m_response.m_http_header.size());

	if (pthread_sigmask(SIG_BLOCK, &m_set, NULL) != 0)
		return -1;

	return result != -1;
}

/// @brief Virtual method to send server response.
/// return True if response is sent or False if not.
bool send_body(void) override {
	int result = 0;

	result = SSL_write(m_ssl, m_response.m_body.c_str(), m_response.m_body.size());

	if (pthread_sigmask(SIG_BLOCK, &m_set, NULL) != 0)
		return -1;

	return result != -1;
}

/// @brief Method reset all values to default.
void clear(void) override {
	m_request.clear();
	m_response.clear();
	m_buffer_for_request.clear();
}

private:
	/// @brief Pointer to the ssl struct.
	SSL *m_ssl { nullptr };

	/// @brief Pointer to the ssl context struct.
	SSL_CTX *m_ssl_ctx { nullptr };

	/// @brief Value which describe was ssl accepted or not.
	bool m_is_ssl_accepted { false };

	/// @brief Value to avoid calls of the SIGPIPE signal.
	sigset_t m_set;
};

/// @brief Structure which contains value for cache.
struct Cache {

/// @brief Value to cache files.
std::map < std::string, std::string > m_files_cache;
};

#endif // DATA_STRUCTURES_H
