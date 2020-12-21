#ifndef SERVER_H
#define SERVER_H

#include "types.h"
#include "threadpool.h"

struct epoll_event;

///@brief Our future server.
class Server
{
    public:
        Server(const Data &_data);
        ~Server();

        void run();
        void stop();

    private:
        int set_nonblock(const int _fd) const;
        void process_listening(const int _socket, const int _epoll) ;
        int create_epoll(int _socket, epoll_event &_event) const;
        int create_socket() const;
        void receive_and_handle_data(int _fd);
        void enable_keepalive(int sock) const;

        ThreadPool m_thread_pool;
        Data m_data;
        volatile bool m_is_run { true };
};

#endif // SERVER_H
