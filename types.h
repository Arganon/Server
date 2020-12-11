#ifndef TYPES_H
#define TYPES_H

///@brief Data structure for port and ip.
///       Has default value for port (80) and ip (127.0.0.1)
struct Data {
    Data() : port(12345), host("127.0.0.1") {}
    int port;
    std::string host;
};

#endif // TYPES_H
