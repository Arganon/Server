#ifndef SESSION_H
#define SESSION_H

#include <string>

struct Session
{
    Session(const int _fd, const std::string _request)
        : m_fd(_fd)
        , m_request(_request) {}

    const int m_fd;
    const std::string m_request;
};

#endif // SESSION_H
