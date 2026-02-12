#include "net/TcpServer.hpp"

void TcpServer::set_connection_callback(ConnectionCallback cb) {
    m_connection_cb = cb;
}

void TcpServer::set_message_callback(MessageCallback cb) {
    m_message_cb = cb;
}

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr) 
    : m_loop(loop) {
    m_acceptor = std::make_unique<Acceptor>(m_loop, listenAddr);
    
    m_acceptor->set_new_connection_callback([this](int fd, const InetAddress& peerAddr) {
        new_connection(fd, peerAddr);
    });
}

void TcpServer::new_connection(int fd, const InetAddress& peerAddr) {
    auto conn = std::make_shared<TcpConnection>(m_loop, fd, peerAddr);
    m_connections[fd] = conn;

    conn->set_connection_callback(m_connection_cb);
    conn->set_message_callback(m_message_cb);
    conn->set_close_callback([this](auto c) {
        remove_connection(c);
    });

    conn->connection_established();
}

void TcpServer::remove_connection(const std::shared_ptr<TcpConnection>& conn) {
    m_connections.erase(conn->fd());
}

void TcpServer::start() {
    m_acceptor->listen();
}

TcpServer::~TcpServer() {
    for (const auto& conn : m_connections) {
        conn.second->connection_destroyed();
    }
}
EventLoop* TcpServer::get_loop() {
    return m_loop;
}