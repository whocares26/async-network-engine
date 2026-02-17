#include "net/UdpServer.hpp"
#include "net/EventLoop.hpp"
#include "net/InetAddress.hpp"
#include "net/UdpSocket.hpp"
#include <memory>

namespace net {

UdpServer::UdpServer(EventLoop* loop, const InetAddress& listenAddr) : m_listenAddr(listenAddr), m_loop(loop) {
    m_socket = std::make_unique<UdpSocket>();
    m_socket->set_reuse_addr(true);
    m_socket->set_reuse_port(true);
    m_started = false;
}

UdpServer::~UdpServer() {
    if (m_socket) {
        m_loop->remove_fd(m_socket->fd());
    }
}

void UdpServer::set_message_callback(MessageCallback cb) {
    m_messageCallback = cb;
}

void UdpServer::start() {
    m_socket->bind(m_listenAddr);
    m_loop->add_fd(m_socket->fd(), EPOLLIN, [this](uint32_t) {
        handleRead();
    });
    m_started = true;
}

ssize_t UdpServer::sendTo(const std::string& data, const InetAddress& clientAddr) {
    ssize_t bytes = m_socket->sendto(data.data(), data.size(), clientAddr);
    return bytes;
}

UdpSocket& UdpServer::socket() {
    return *m_socket;
}

void UdpServer::handleRead() {
    char buffer[65536];
    InetAddress clientAddr{0};
    while (true) {
        ssize_t n = m_socket->recvfrom(buffer, sizeof(buffer), clientAddr);
        if (n > 0) {
            if (m_messageCallback) {
                m_messageCallback(std::string(buffer, n), clientAddr);
            }
        } else if (n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                throw std::system_error(errno, std::generic_category(),
                                        "UdpServer::handleRead recvfrom");
            }
        }
    }
}

} // namespace net