#pragma once
#include "Socket.hpp"
#include "InetAddress.hpp"
#include "EventLoop.hpp"
#include "functional"

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
    Socket m_accept_socket{};
    NewConnectionCallback m_new_connection_callback{};
    bool m_listening{};
};