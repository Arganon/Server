#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"
#include "logger.h"

#define MAX_EVENTS 32

// poll or epoll - faster?
// блокирующий и не блокирующий

Server::Server(const Data &_data) : m_data(_data) {}

int Server::set_nonblock(const int _fd) const {
    int flags;
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(_fd, F_GETFL, 0))) {
        flags = 0;
    }
    return fcntl(_fd, F_SETFL, flags | O_NONBLOCK);
#else
    flag = 1;
    return ioctl(fd, FIOBIO, &flags);

#endif
}

void Server::stop() {
    LOG(LOG_LVL::DEBUG, "The server is stoped.");
    m_is_run = false;
}

void Server::process_listening(const int _socket, const int _epoll) {
    while (m_is_run) {
        struct epoll_event events[MAX_EVENTS];
        int N = epoll_wait(_epoll, events, MAX_EVENTS, -1);

        for (unsigned int i = 0; i < N; ++i) {
            if (events[i].data.fd == _socket) {
                // TODO: Create a class Session and put to the queue/vector for future deleting.
                // TODO: Optimize deleting from the queue or vector
                int slave_socket = accept(_socket, 0, 0);
                set_nonblock(slave_socket);
                struct epoll_event event;
                event.data.fd = slave_socket;
                event.events = EPOLLIN;
                epoll_ctl(_epoll, EPOLL_CTL_ADD, slave_socket, &event);
            }
            else {
                static char buff[1024];
                int receive_result = recv(events[i].data.fd, buff, 1024, MSG_NOSIGNAL);

                if ((receive_result == 0 ) && (errno != EAGAIN)) {
                    shutdown(events[i].data.fd, SHUT_RDWR);
                    close(events[i].data.fd);
                }
                else if (receive_result > 0) {
                    send(events[i].data.fd, buff, receive_result, MSG_NOSIGNAL);
                }
            }
        }
    }
}

int Server::create_epoll(int _socket, epoll_event &_event) const {
    int e_poll = epoll_create1(0);
    _event.data.fd = _socket;
    _event.events = EPOLLIN;

    if (epoll_ctl(e_poll, EPOLL_CTL_ADD, _socket, &_event) < 0) {
        LOG(LOG_LVL::DEBUG, "The epoll_ctl returned an Error.");
        close(e_poll);
        return -1;
    }

    return e_poll;
}

int Server::create_and_listen_socket() const {
    int main_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

    if (main_socket < 0) {
        LOG(LOG_LVL::DEBUG, "The main_socket socket descriptor returned an Error.");
        return -1;
    }

    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET6;
    SockAddr.sin_port = htons(m_data.port);
    SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(main_socket, (struct sockaddr*)(&SockAddr), sizeof (SockAddr)) < 0) {
        LOG(LOG_LVL::DEBUG, "Socket binding return an Error.");
        return -1;
    }

    set_nonblock(main_socket);

    if (listen(main_socket, SOMAXCONN) < 0) {
        LOG(LOG_LVL::DEBUG, "Try to port listening returned an Error.");
        return -1;
    }

    return main_socket;
}


void Server::run() {
    int main_socket = create_and_listen_socket();

    if (main_socket < 0) {
        LOG(LOG_LVL::DEBUG, "The socket descriptor returned an Error.");
        return;
    }

    struct epoll_event event;
    int epoll = create_epoll(main_socket, event);

    if (epoll < 0) {
        LOG(LOG_LVL::DEBUG, "Try of the e_poll creation returned an Error.");
        return;
    }

    process_listening(main_socket, epoll);

    close(epoll);
}
