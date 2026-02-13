#pragma once

#include "net/TcpSocket.hpp"
#include "net/InetAddress.hpp"
#include "net/EventLoop.hpp"
#include <functional>

namespace net {

class Acceptor {
public:
    using NewConnectionCallback = std::function<void(int fd, const InetAddress& addr)>;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr);
    ~Acceptor();

    Acceptor(const Acceptor&) = delete;
    Acceptor operator=(const Acceptor&) = delete;

    void set_new_connection_callback(NewConnectionCallback cb);
    void listen();

private:
    void handle_read();

    EventLoop* m_loop{};
    TcpSocket m_accept_socket;
    NewConnectionCallback m_new_connection_callback{};
    bool m_listening{};
};

}