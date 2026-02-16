#include "net/UdpSocket.hpp"
#include "net/InetAddress.hpp"
#include <asm-generic/socket.h>
#include <cerrno>
#include <sys/socket.h>

namespace net {

    UdpSocket::UdpSocket() : Socket(::socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0)) {
        if (m_fd == -1) {
            throw std::system_error(errno, std::generic_category(), "socket UDP construction failed");
        }
    }

    UdpSocket::UdpSocket(int fd) : Socket(fd) { }

    ssize_t UdpSocket::sendto(const void* data, size_t length, const InetAddress& dest_addr) {
        ssize_t bytes = ::sendto(m_fd, data, length, 0, dest_addr.get_sockaddr(), dest_addr.get_length());
        if (bytes == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return -1;
            }
            throw std::system_error(errno, 
                std::generic_category(), "Failed to ::sendto udp socket");
        }
        return bytes;
    }

    ssize_t UdpSocket::recvfrom(void* buffer, size_t length, InetAddress& src_addr) {
        sockaddr* sock_addr = src_addr.get_sockaddr();
        socklen_t size = src_addr.get_length();
        ssize_t bytes = ::recvfrom(m_fd, buffer, length, 0, sock_addr, &size);
        if (bytes == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return -1;
            }
            throw std::system_error(errno, 
                std::generic_category(), "Failed to ::recvfrom udp socket");
        }
        return bytes;
    }

    void UdpSocket::set_broadcast(bool on) {
        int optval = on ? 1 : 0;
        int ec = ::setsockopt(m_fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int));
        if (ec == -1) {
            throw std::system_error(errno, 
                std::generic_category(), "Failed to set broadcast udp");
        }   
    }
} // namespace net