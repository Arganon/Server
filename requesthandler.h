#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "session.h"
#include "storage.h"

class HTTPRequest;

class RequestHandler
{
    public:
        RequestHandler() = default;
        RequestHandler(const Session &_session);
        ~RequestHandler() = default;

        void handle_request();
    private:

        static void parse_request(HTTPRequest &_request, Session &_session);
        static void parse_command();
        static void handle_GET_request(const char *_command, std::string &_response);

        Storage m_file_storage;
        Session m_session;
};

#endif // REQUESTHANDLER_H
