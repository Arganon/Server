#include <set>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"
#include "logger.h"

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

    std::set<int> SlaveSocets;
    m_is_run = true;

    while (m_is_run) {
        fd_set Set;
        FD_ZERO(&Set);

        FD_SET(main_socket, &Set);

        for (auto Iter = SlaveSocets.begin(); Iter != SlaveSocets.end(); ++Iter) {
            FD_SET(*Iter, &Set);
        }

        int Max = std::max(main_socket, *std::max_element(SlaveSocets.begin(), SlaveSocets.end()));
        select(Max+1, &Set, nullptr, nullptr, nullptr);

        for (auto Iter = SlaveSocets.begin(); Iter != SlaveSocets.end(); ++Iter) {
            if (FD_ISSET(*Iter, &Set)) {
                static char Buffer[1024];
                int RecvSize = recv(*Iter,
                                    Buffer,
                                    1024,
                                    MSG_NOSIGNAL);

                if (RecvSize == 0 && errno != EAGAIN) {
                    shutdown(*Iter, SHUT_RDWR);
                    close(*Iter);
                    SlaveSocets.erase(Iter);
                }
                else if (RecvSize != 0) {
                    send(*Iter, Buffer, RecvSize, MSG_NOSIGNAL);
                }
            }
        }

        if (FD_ISSET(main_socket, &Set)) {
            int SlaveSocket = accept(main_socket, 0, 0);
            set_nonblock(SlaveSocket);
            SlaveSocets.insert(SlaveSocket);
        }
    }
}
