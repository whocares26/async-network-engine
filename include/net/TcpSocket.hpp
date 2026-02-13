#pragma once

#include "net/Socket.hpp"

namespace net {

class TcpSocket : public Socket {
public:
    TcpSocket();
    explicit TcpSocket(int fd);

    void listen(int backlog = SOMAXCONN);
    int accept (InetAddress& peer_addr);
};

}