#pragma once

#include "net/EventLoop.hpp"
#include "net/InetAddress.hpp"
#include "net/TcpConnection.hpp"
#include <functional>
#include <memory>

namespace net {

class TcpClient {
public: 
    using ConnectionCallback = std::function<void(const std::shared_ptr<TcpConnection>&)>;
    using MessageCallback = std::function<void(const std::shared_ptr<TcpConnection>&, std::string&)>;
    using CloseCallback = std::function<void(const std::shared_ptr<TcpConnection>&)>;
    using ErrorCallback = std::function<void()>;

    explicit TcpClient(EventLoop* loop);

    ~TcpClient();

    TcpClient(const TcpClient&) = delete;
    TcpClient& operator=(const TcpClient&) = delete;

    void connect(const InetAddress& serverAddr, ConnectionCallback onConnect, MessageCallback onMessage,
                CloseCallback onClose, ErrorCallback onError = nullptr);

    void cancel();

private:
    class Connector {
    public:
        Connector(EventLoop* loop);
        ~Connector();
        
        void start(const InetAddress& addr,
                   ConnectionCallback onConnect,
                   MessageCallback onMessage,
                   CloseCallback onClose,
                   ErrorCallback onError);
        
        void cancel();
        
    private:
        void handleWrite();
        void handleError();
        void removeAndClose();
        
        EventLoop* m_loop;
        int m_sockfd{ -1 };
        InetAddress m_serverAddr;
        ConnectionCallback m_onConnect;
        MessageCallback m_onMessage;
        CloseCallback m_onClose;
        ErrorCallback m_onError;
        bool m_connecting{ false };
    };
    
    std::unique_ptr<Connector> m_connector;
};

} // namespace net