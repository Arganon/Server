#ifndef SERVER_H
#define SERVER_H

#include "types.h"

///@brief Our future server.
class Server
{
    public:
        Server(const Data &_data);
        ~Server() = default;

        void Run();
        void Stop();

    private:
        int set_nonblock(int fd) const;

        Data m_data;
        volatile bool m_is_run { false };
};

#endif // SERVER_H
