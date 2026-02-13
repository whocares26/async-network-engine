#include "net/TcpSocket.hpp"
#include <sys/socket.h>

namespace net {

TcpSocket::TcpSocket()
    : Socket(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0))
{
    if (m_fd == -1) {
        throw std::system_error(errno, std::generic_category(), "socket TCP");
    }
}

TcpSocket::TcpSocket(int fd) : Socket(fd) {}

void TcpSocket::listen(int backlog) {
    int ec = ::listen(m_fd, backlog);
    if (ec == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to listen port with ::listen");
    }
}

int TcpSocket::accept(InetAddress& peer_addr) {
    socklen_t len = peer_addr.get_length();
    int client_fd = ::accept(m_fd, const_cast<sockaddr*>(peer_addr.get_sockaddr()), &len);
    if (client_fd == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return -1;
        }
        throw std::system_error(errno, std::generic_category(), "Failed to accept new connection with ::accept");
    }
    return client_fd;
}

} // namespace net