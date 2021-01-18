#include "../../utils/public/logger.h"
#include "../../utils/public/requestparser.h"
#include "../../data/public/data_structures.h"

RequestParser & RequestParser::instance(void) {
    static RequestParser parser;
    return parser;
}

void RequestParser::parse_request(Client *_client_data) {
	size_t filename_pos = parse_request_http_type(_client_data);

    if (filename_pos != std::string::npos) {
		_client_data->m_request.m_filename = get_file_name(_client_data->m_buffer_for_request, filename_pos);

		if (is_filename_valid(_client_data->m_request.m_filename)) {
			_client_data->m_response.m_content_type = get_content_type(
									_client_data->m_request.m_filename);
			_client_data->m_request.m_command = CMD_TYPE::TYPE::GET_FILE;
        }
    }
}

size_t RequestParser::parse_request_http_type(Client *_client_data) {
	std::size_t pos = _client_data->m_buffer_for_request.find(' ');

    if (pos != std::string::npos) {
		_client_data->m_request.m_type = REQUEST_TYPE::to_enum(
					_client_data->m_buffer_for_request.substr(0, pos));
        pos += 1;
    }

    return pos;
}

std::string RequestParser::get_file_name(std::string & _source, size_t _position) {
    std::string filename;

	size_t crlf_pos = _source.find("\r\n");
	std::string line_with_filename = _source.substr(_position, crlf_pos - _position);
	size_t file_name_end_pos = line_with_filename.rfind(' ');

	if (file_name_end_pos != std::string::npos) {
		filename = line_with_filename.substr(0, file_name_end_pos);
	}

    return filename;
}

std::string RequestParser::get_content_type(std::string & _filename) {
	std::string extension;

	size_t extension_pos = _filename.rfind('.');

	if (extension_pos != std::string::npos && (extension_pos + 1) < _filename.size()) {
		extension = _filename.substr(extension_pos + 1, _filename.size());
	}

	return CONTENT_TYPE::get_type(extension);
}

// TODO: Should be checked special symbols, not only '..' and '~'.
bool RequestParser::is_filename_valid(std::string & _filename) {
    return _filename.find("..") == std::string::npos &&
           _filename.find("~") == std::string::npos;
}
