#include <string.h>
#include <iostream>


#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <algorithm>
#include <sys/types.h>

#include "types.h"
#include "requesthandler.h"

RequestHandler::RequestHandler(const Session &_session)
    : m_session(_session) {}

void RequestHandler::handle_request() {
    HTTPRequest request;
    parse_request(request, m_session);
    std::string response;
    response.reserve(MAX_RESPONSE_LENGTH);
    m_file_storage.get_files_list();


    switch (request.m_type) {
    case REQUEST_TYPE::GET:
        handle_GET_request(request.m_command, response);
        break;

    case REQUEST_TYPE::POST:
        break;

    case REQUEST_TYPE::UNDEFINED:
        break;

    default:
        break;
    }

    std::cout << "Response: " << response << std::endl;
    send(m_session.m_fd, response.c_str(), response.size(), MSG_NOSIGNAL);
//    close(m_session.m_fd);

}

void RequestHandler::parse_request(HTTPRequest &_request, Session &_session) {
    std::size_t pos = _session.m_request.find_first_of(' ');
    if (pos != std::string::npos) {
        _request.m_type = REQUEST_TYPE::to_enum(_session.m_request.substr(0, pos));
        pos += 1;
        sscanf(&_session.m_request[pos], "%s", _request.m_command);
    }
}

void RequestHandler::parse_command() {

}

void RequestHandler::handle_GET_request(const char *_command, std::string &_response) {
    _response.append("HTTP/1.1 200 OK\r\n");
    _response.append("text/html; charset=UTF-8\r\n\r\n");
    _response.append("<h1>");
    _response.append("Received command: ");
    _response.append(_command);
    _response.append("</h1>");
}










