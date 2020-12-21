#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <string>

#define MAX_EVENTS 42
#define MAX_BUFF_SIZE 4242
#define MAX_COMMAND_LENGTH 1024
#define MAX_RESPONSE_LENGTH 256

///@brief Data structure for port and ip.
///       Has default value for port (80) and ip (127.0.0.1)
struct Data {
    Data() : port(12346), host("127.0.0.1") {}
    int port;
    std::string host;
};

struct REQUEST_TYPE {
    enum HTTP_TYPE
    {
        GET = 0,
        POST,
        UNDEFINED
    };

    static std::string to_string(HTTP_TYPE _http_type) noexcept {
        std::map<HTTP_TYPE, std::string> types {
            { HTTP_TYPE::GET, "GET" },
            { HTTP_TYPE::POST, "POST" }
        };

        auto it = types.find(_http_type);
        if (it != types.end()) {
            return it->second;
        }
        return "";
    }

    static HTTP_TYPE to_enum(std::string _http_type) noexcept {
        std::map<std::string, HTTP_TYPE> types {
            { "GET", HTTP_TYPE::GET },
            { "POST", HTTP_TYPE::POST}
        };
        auto it = types.find(_http_type);
        if (it != types.end()) {
            return it->second;
        }
        return HTTP_TYPE::UNDEFINED;
    }
};

struct CMD_TYPE {
    enum TYPE
    {
        GET_FILE = 0,
        GET_LIST,
        UNDEFINED
    };

    static std::string to_string(TYPE _http_type) noexcept {
        std::map<TYPE, std::string> types {
            { TYPE::GET_FILE, "get_file" },
            { TYPE::GET_LIST, "get_list" }
        };

        auto it = types.find(_http_type);
        if (it != types.end()) {
            return it->second;
        }
        return "";
    }

    static TYPE to_enum(std::string _http_type) noexcept {
        std::map<std::string, TYPE> types {
            { "get_file", TYPE::GET_FILE },
            { "get_list", TYPE::GET_LIST}
        };
        auto it = types.find(_http_type);
        if (it != types.end()) {
            return it->second;
        }
        return TYPE::UNDEFINED;
    }
};


struct HTTPRequest {
    REQUEST_TYPE::HTTP_TYPE m_type { REQUEST_TYPE::HTTP_TYPE::UNDEFINED };
    char m_command[MAX_COMMAND_LENGTH + 1];
};

#endif // TYPES_H
