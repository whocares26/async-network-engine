#pragma once
#include <unistd.h>
#include <fcntl.h>
#include "net/InetAddress.hpp"
#include <system_error>
#include <exception>
#include <sys/socket.h>

namespace net {

class Socket {
public:
    virtual ~Socket();

    int fd() const;

    void set_nonblocking();
    void set_reuse_addr(bool on);
    void set_reuse_port(bool on);
    void bind(const InetAddress& addr);

protected:
    
    explicit Socket(int fd);
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    int m_fd;
};

}