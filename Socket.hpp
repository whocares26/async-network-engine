#pragma once
#include <unistd.h>
#include <fcntl.h>
#include "InetAddress.hpp"
#include <system_error>
#include <exception>
#include <sys/socket.h>
class Socket {
public:
    Socket();

    explicit Socket(int fd);

    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    int fd() const;

    void set_nonblocking();
    void set_reuse_addr(bool on);
    void set_reuse_port(bool on);
    void bind(const InetAddress& addr);
    void listen();
    int accept(InetAddress& peeraddr);

private:
    int m_fd = -1;
};