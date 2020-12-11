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

Server::Server(const Data &_data) : m_data(_data) {}

int Server::set_nonblock(int fd) const {
    int flags;
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(fd, F_GETFL, 0))) {
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    flag = 1;
    return ioctl(fd, FIOBIO, &flags);

#endif
}

void Server::Stop() {
    LOG(LOG_LVL::DEBUG, "The server is stoped.");
    m_is_run = false;
}


// TODO: Split this method for a few less.
void Server::Run() {
    int main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (main_socket < 0) {
        LOG(LOG_LVL::DEBUG, "The socket descriptor returned an Error.");
        return;
    }

    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(m_data.port);
    SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(main_socket, (struct sockaddr*)(&SockAddr), sizeof (SockAddr));

    set_nonblock(main_socket);

    listen(main_socket, SOMAXCONN);

    int e_poll = epoll_create1(0);
    struct epoll_event event;
    event.data.fd = main_socket;
    event.events = EPOLLIN;
    epoll_ctl(e_poll, EPOLL_CTL_ADD, main_socket, &event);

    m_is_run = true;
    while (m_is_run) {
        struct epoll_event events[MAX_EVENTS];
        int N = epoll_wait(e_poll, events, MAX_EVENTS, -1);

        for (unsigned int i = 0; i < N; ++i) {
            if (events[i].data.fd == main_socket) {
                int slave_socket = accept(main_socket, 0, 0);
                set_nonblock(slave_socket);
                struct epoll_event event;
                event.data.fd = slave_socket;
                event.events = EPOLLIN;
                epoll_ctl(e_poll, EPOLL_CTL_ADD, slave_socket, &event);
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
