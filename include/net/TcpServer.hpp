#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include "net/Acceptor.hpp"
#include "net/TcpConnection.hpp"

class TcpServer {
public:
    using ConnectionCallback = TcpConnection::ConnectionCallback;
    using MessageCallback = TcpConnection::MessageCallback;

    TcpServer(EventLoop* loop, const InetAddress& listenAddr);
    ~TcpServer();

    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    void start();

    void set_connection_callback(ConnectionCallback cb);
    void set_message_callback(MessageCallback cb);

private:
    void new_connection(int fd, const InetAddress& peerAddr);

    void remove_connection(const std::shared_ptr<TcpConnection>& conn);

    EventLoop* m_loop;
    std::unique_ptr<Acceptor> m_acceptor;

    std::unordered_map<int, std::shared_ptr<TcpConnection>> m_connections;

    ConnectionCallback m_connection_cb;
    MessageCallback m_message_cb;

    int m_next_conn_id = 1;
};