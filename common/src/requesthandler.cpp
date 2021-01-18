#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <netdb.h>
#include <sstream>
#include <signal.h>
#include <unistd.h>
#include <algorithm>
#include <sys/types.h>

#include "../../data/public/types.h"
#include "../../data/public/httpresponse.h"
#include "../../common/public/requesthandler.h"

RequestHandler& RequestHandler::instance() {
    static RequestHandler handler;
    return handler;
}

void RequestHandler::handle_request(Client *_client_data) {

    switch (_client_data->m_request.m_type) {
    // There is can be used pattern 'factory method' or 'abstract factory'.
    // It depends on the complexity of the task.
    case REQUEST_TYPE::GET:
        handle_GET_request(_client_data->m_request, _client_data->m_response);
        break;

    case REQUEST_TYPE::POST:
        _client_data->m_response.m_status_code = RESPONSE_STATUS::CODE::NOT_IMPLEMENTED;
        _client_data->m_response.m_body.append(RESPONSE_STATUS::to_string(
						_client_data->m_response.m_status_code).append("\r\n\r\n"));
        break;

    case REQUEST_TYPE::UNDEFINED:
        _client_data->m_response.m_status_code = RESPONSE_STATUS::CODE::BAD_REQUEST;
        _client_data->m_response.m_body.append(RESPONSE_STATUS::to_string(
						_client_data->m_response.m_status_code).append("\r\n\r\n"));
        break;

    default:
        _client_data->m_response.m_status_code = RESPONSE_STATUS::CODE::NOT_IMPLEMENTED;
        _client_data->m_response.m_body.append(RESPONSE_STATUS::to_string(
						_client_data->m_response.m_status_code).append("\r\n\r\n"));
        break;
    }

    prepare_header(_client_data->m_response);

	_client_data->send_header();

    prepare_and_send_body(_client_data);
}

void RequestHandler::prepare_and_send_body(Client *_client_data) {
    if (!_client_data->m_response.m_is_chunk) {
		_client_data->send_body();
    }
    else {
        handle_chunked_response(_client_data);
    }
}

void RequestHandler::handle_chunked_response(Client *_client_data) {
    size_t modulo = _client_data->m_response.m_file_size % MAX_SIZE_FOR_READ;
    size_t start_pos = 0;

    for (size_t i = _client_data->m_response.m_file_size / MAX_SIZE_FOR_READ;
         i > 0;
		 --i, start_pos += MAX_SIZE_FOR_READ)
	{
        prepare_and_send_chunked_response(_client_data, start_pos, MAX_SIZE_FOR_READ);
    }

    if (modulo > 0) {
        prepare_and_send_chunked_response(_client_data, start_pos, modulo);
    }

    prepare_and_send_chunked_response(_client_data, 0, 0);
}

void RequestHandler::prepare_and_send_chunked_response(Client *_client_data,
                                                       size_t _start_pos,
													   size_t _size_to_read)
{
    auto dec_to_hex = [](size_t d) -> std::string {
        std::stringstream ss;
        ss << std::hex << d;
        return ss.str();
    };

    _client_data->m_response.m_body = dec_to_hex(_size_to_read);
    _client_data->m_response.m_body.append(_size_to_read > 0 ? "\r\n" : "\r\n\r\n");

	_client_data->send_body();

    if (_size_to_read > 0) {
        _client_data->m_response.m_body.clear();

		Storage::instance().get_file(_client_data->m_request.m_filename,
                                _client_data->m_response.m_body,
                                _start_pos,
                                _size_to_read);

        _client_data->m_response.m_body.append("\r\n");

		_client_data->send_body();

        _client_data->m_response.m_body.clear();
    }
}

void RequestHandler::handle_GET_request(const HTTPRequest & _request,
										HTTPResponse & _response)
{
    switch (_request.m_command) {
    case CMD_TYPE::TYPE::GET_FILE:
        get_file(_request, _response);
        break;

    default:
        _response.m_status_code = RESPONSE_STATUS::CODE::NOT_IMPLEMENTED;
        _response.m_body.append(RESPONSE_STATUS::to_string(_response.m_status_code));
        break;
    }
}

void RequestHandler::get_file(const HTTPRequest & _request,
							  HTTPResponse & _response)
{

	_response.m_file_size = Storage::instance().get_file_size(_request.m_filename);

    if (_response.m_file_size > MAX_SIZE_FOR_READ) {
		_response.m_status_code = RESPONSE_STATUS::CODE::OK;
        _response.m_is_chunk = true;
    }
	else if (Storage::instance().get_file(_request.m_filename,
										  _response.m_body,
										  0,
										  _response.m_file_size))
	{
        _response.m_status_code = RESPONSE_STATUS::CODE::OK;
    }
    else {
        _response.m_status_code = RESPONSE_STATUS::CODE::NOT_FOUND;
		_response.m_body.append(
					RESPONSE_STATUS::to_string(
						_response.m_status_code).append("\r\n\r\n"));
    }
}

void RequestHandler::prepare_header(HTTPResponse & _response) {
    _response.m_http_header.append("HTTP/1.1");
    _response.m_http_header.append(RESPONSE_STATUS::to_string(_response.m_status_code));
    _response.m_http_header.append("\r\n");
    _response.m_http_header.append("Connection: keep-alive\r\n");

	if (!_response.m_content_type.empty()) {
		_response.m_http_header.append("Content-Type: ");
		_response.m_http_header.append(_response.m_content_type);
		_response.m_http_header.append("\r\n");
	}

    if (!_response.m_is_chunk) {
        _response.m_http_header.append("Content-Length:");
        _response.m_http_header.append(std::to_string(_response.m_body.size()));
    }
    else {
        _response.m_http_header.append("Transfer-Encoding: chunked");
    }

    _response.m_http_header.append("\r\n\r\n");
}
