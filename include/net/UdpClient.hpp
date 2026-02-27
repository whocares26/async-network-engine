#pragma once

#include "net/EventLoop.hpp"
#include "net/InetAddress.hpp"
#include "net/UdpSocket.hpp"
#include <functional>
#include <memory>

namespace net { 
    class UdpClient {
    public:
        using MessageCallback = std::function<void(const std::string&, const InetAddress&)>;
        explicit UdpClient(EventLoop*);
        UdpClient(const UdpClient&) = delete;
        UdpClient& operator=(const UdpClient&) = delete;

        void setServerAddr(const InetAddress&);
        ssize_t send(const std::string&);
        ssize_t sendTo(const std::string&, const InetAddress&);
        void setBroadcast(bool);
        
        void setMessageCallback(MessageCallback);
        void startReading();
    private:

        void handleRead();

        EventLoop* m_loop;
        std::unique_ptr<UdpSocket> m_socket;
        InetAddress m_serverAddr;
        MessageCallback m_msgCallback;
        bool m_reading{false};
    };
} // namespace net