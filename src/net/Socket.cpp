#include "net/Socket.hpp"

namespace net {

Socket::Socket(int fd) : m_fd(fd) {}

int Socket::fd() const {
    return m_fd;
}

void Socket::set_nonblocking() {
    auto current_flags = fcntl(m_fd, F_GETFL);
    if (current_flags == -1) {
        throw std::system_error(errno, 
            std::generic_category(), "Failed to get current_flags with fcntl");
    }
    auto new_flags = current_flags | O_NONBLOCK;
    auto ec = fcntl(m_fd, F_SETFL, new_flags);
    if (ec == -1) {
        throw std::system_error(errno, 
            std::generic_category(), "Failed to set new_flags with fcntl");
    }
}

void Socket::set_reuse_addr(bool on) {
    int optval = on ? 1 : 0;
    int ec = setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, static_cast<const void*>(&optval), sizeof(optval));
    if (ec == -1) {
        throw std::system_error(errno, 
            std::generic_category(), "Failed to reuse address with setsockopt");  
    }
}

void Socket::set_reuse_port(bool on) {
    int optval = on ? 1 : 0;
    int ec = setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, static_cast<const void*>(&optval), sizeof(optval));
    if (ec == -1) {
        throw std::system_error(errno, 
            std::generic_category(), "Failed to reuse port with setsockopt");  
    }
}

void Socket::bind(const InetAddress& addr) {
    int ec = ::bind(m_fd, addr.get_sockaddr(), addr.get_length());
    if (ec == -1) {
        throw std::system_error(errno, 
            std::generic_category(), "Failed to bind port with ::bind");  
    }
}

Socket::~Socket() {
    if (m_fd != -1) {
        close(m_fd);
    }
}

}