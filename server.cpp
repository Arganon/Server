#include <fcntl.h>
#include <netdb.h>
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
#include <netinet/tcp.h>

#include "types.h"
#include "server.h"
#include "logger.h"
#include "requesthandler.h"

// poll or epoll - faster?
// блокирующий и не блокирующий

Server::Server(const Data &_data) : m_data(_data) {
    m_thread_pool.set_thread_quantity(42); // TODO: calculate processors total count, multiply it on 2 and minus 1
}

Server::~Server() {}

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
    LOG(LOG_LVL::LOGS::DEBUG, "The server is stoped.");
    m_is_run = false;
}

void Server::receive_and_handle_data(const int _fd) {
    static char buff[MAX_BUFF_SIZE + 1];
    int receive_res = recv(_fd, buff, MAX_BUFF_SIZE, MSG_NOSIGNAL);

    if ((receive_res == 0) && (errno != EAGAIN)) {
        LOG(LOG_LVL::LOGS::DEBUG, "Recieving of the request rising an Error.");
        shutdown(_fd, SHUT_RDWR);
        close(_fd);
    }
    else if (receive_res > MAX_BUFF_SIZE) {
        LOG(LOG_LVL::LOGS::DEBUG, "Request size is bigger than MAX_BUFF_SIZE.");
        shutdown(_fd, SHUT_RDWR);
        close(_fd);
    }
    else if (receive_res > 0) {
        m_thread_pool.add_task([&]() {
                Session s {_fd, buff};
                RequestHandler handler(s);
                handler.handle_request();
            }
        );
    }
}

void Server::process_listening(const int _socket, const int _epoll) {
    while (m_is_run) {
        struct epoll_event events[MAX_EVENTS];
        int N = epoll_wait(_epoll, events, MAX_EVENTS, -1);

        for (int i = 0; i < N; ++i) {
            if (events[i].data.fd == _socket) {
                // TODO: Create a class Session and put to the queue/vector for future deleting.
                // TODO: Optimize deleting from the queue or vector
                int slave_socket = accept(_socket, NULL, NULL);
                enable_keepalive(slave_socket);
                set_nonblock(slave_socket);
                struct epoll_event event;
                event.data.fd = slave_socket;
                event.events = EPOLLIN;
                epoll_ctl(_epoll, EPOLL_CTL_ADD, slave_socket, &event);
            }
            else {
                receive_and_handle_data(events[i].data.fd);
            }
        }
    }
}

int Server::create_epoll(int _socket, epoll_event &_event) const {
    int e_poll = epoll_create1(0);
    _event.data.fd = _socket;
    _event.events = EPOLLIN;

    if (epoll_ctl(e_poll, EPOLL_CTL_ADD, _socket, &_event) < 0) {
        LOG(LOG_LVL::LOGS::DEBUG, "The epoll_ctl returned an Error.");
        close(e_poll);
        return -1;
    }

    return e_poll;
}

void Server::enable_keepalive(int sock) const {
    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));

    int idle = 1;
    setsockopt(sock, SOL_SOCKET, TCP_KEEPIDLE, &idle, sizeof(int));

    int interval = 1;
    setsockopt(sock, SOL_SOCKET, TCP_KEEPINTVL, &interval, sizeof(int));

    int maxpkt = 10;
    setsockopt(sock, SOL_SOCKET, TCP_KEEPCNT, &maxpkt, sizeof(int));
}

int Server::create_socket() const {
    int main_socket;
    struct addrinfo settings;
    struct addrinfo *servinfo;

    memset(&settings, 0, sizeof (settings));

    settings.ai_family = AF_UNSPEC;
    settings.ai_socktype = SOCK_STREAM;
    settings.ai_flags = AI_PASSIVE;

    int addr_info = getaddrinfo(NULL, std::to_string(m_data.port).c_str(), &settings, &servinfo);

    if (addr_info < 0) {
        LOG(LOG_LVL::LOGS::DEBUG, "getaddrinfo return an Error.");
        return -1;
    }

    struct addrinfo *it;
    const int opt = 1;

    for (it = servinfo; it != NULL; it = it->ai_next) {
        main_socket = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        set_nonblock(main_socket);
        setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt));
        setsockopt(main_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof (opt));
        enable_keepalive(main_socket);

        if (bind(main_socket, it->ai_addr, it->ai_addrlen) != 0) {
            LOG(LOG_LVL::LOGS::DEBUG, "Socket binding return an Error.");
            close(main_socket);
            continue;
        }
        break;
    }

    if (it == NULL) {
        LOG(LOG_LVL::LOGS::DEBUG, "The free socket wasn't found.");
        return -1;
    }

    if (listen(main_socket, SOMAXCONN) != 0) {
        LOG(LOG_LVL::LOGS::DEBUG, "Try to port listening returned an Error.");
        return -1;
    }

    return main_socket;
}


void Server::run() {
    int main_socket = create_socket();

    if (main_socket < 0) {
        LOG(LOG_LVL::LOGS::DEBUG, "The socket descriptor returned an Error.");
        return;
    }

    struct epoll_event event;
    int epoll = create_epoll(main_socket, event);

    if (epoll < 0) {
        LOG(LOG_LVL::LOGS::DEBUG, "Try of the e_poll creation returned an Error.");
        return;
    }

    process_listening(main_socket, epoll);

    close(epoll);
}
