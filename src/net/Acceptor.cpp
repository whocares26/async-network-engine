#include "net/Acceptor.hpp"

namespace net {

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr)
    : m_loop(loop), m_accept_socket()
{
    m_accept_socket.set_reuse_addr(true);
    m_accept_socket.set_reuse_port(true);
    m_accept_socket.bind(listenAddr);
}

Acceptor::~Acceptor() {
    if (m_listening) {
        m_loop->remove_fd(m_accept_socket.fd());
    }
}

void Acceptor::set_new_connection_callback(NewConnectionCallback cb) {
    m_new_connection_callback = cb;
}

void Acceptor::handle_read() {
    InetAddress peer_addr(0);
    while (true) {
        int client_fd = m_accept_socket.accept(peer_addr);
        if (client_fd != -1) {
            if (m_new_connection_callback) {
                m_new_connection_callback(client_fd, peer_addr);
            } else {
                ::close(client_fd);
            }
        } else {
            break;
        }
    }
}

void Acceptor::listen() {
    if (!m_listening) {
        m_listening = true;
        m_accept_socket.listen();
        m_loop->add_fd(m_accept_socket.fd(), EPOLLIN, [this](uint32_t flags) {
            handle_read();
        });
    }
}

}