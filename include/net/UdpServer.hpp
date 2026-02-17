#pragma once

#include "net/EventLoop.hpp"
#include "net/UdpSocket.hpp"
#include "net/InetAddress.hpp"
#include <functional>
#include <memory>

namespace net {

class UdpServer {
public:
    using MessageCallback = std::function<void(const std::string& data, const InetAddress& clientAddr)>;

    UdpServer(EventLoop* loop, const InetAddress& listenAddr);

    UdpServer(const UdpServer&) = delete;
    UdpServer& operator=(const UdpServer&) = delete;

    virtual ~UdpServer();

    void set_message_callback(MessageCallback cb);
    void start();
    ssize_t sendTo(const std::string& data, const InetAddress& clientAddr);

    UdpSocket& socket();

private:
    void handleRead();

    EventLoop* m_loop;
    std::unique_ptr<UdpSocket> m_socket;
    InetAddress m_listenAddr;
    MessageCallback m_messageCallback;
    bool m_started;
};

} // namespace net