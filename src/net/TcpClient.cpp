#include "net/TcpClient.hpp"
#include <sys/socket.h>
#include <errno.h>

namespace net {

    TcpClient::Connector::Connector(EventLoop* loop) : m_loop(loop) {}

    TcpClient::Connector::~Connector() {
        removeAndClose();
    }

    void TcpClient::Connector::start(const InetAddress& addr, 
        TcpClient::ConnectionCallback onConnect,
        TcpClient::MessageCallback onMessage, TcpClient::CloseCallback onClose,
        TcpClient::ErrorCallback onError) {
            m_serverAddr = addr;
            m_onConnect = onConnect;
            m_onMessage = onMessage;
            m_onClose = onClose;
            m_onError = onError;

            m_sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
            if (m_sockfd == -1) {
                if (m_onError) m_onError();
                return;
            }

            int ret = ::connect(m_sockfd, addr.get_sockaddr(), addr.get_length());
            if (ret == 0) {
                auto conn = std::make_shared<TcpConnection>(m_loop, m_sockfd, addr);
                conn->set_message_callback(m_onMessage);
                conn->set_close_callback(m_onClose);
                conn->connection_established();
                m_onConnect(conn);
                m_sockfd = -1;
                m_connecting = false;
                return;
            }

            else if (errno == EINPROGRESS) { 
                m_loop->add_fd(m_sockfd, EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLET,
                        [this](uint32_t events) {
                            if (events & (EPOLLERR | EPOLLHUP))
                                handleError();
                            else
                                handleWrite();
                        });
                m_connecting = true;
            } else {
                if (m_onError) m_onError();
                ::close(m_sockfd);
                m_sockfd = -1;
                m_connecting = false;
            }
        }

    void TcpClient::Connector::handleError() {
        if (m_onError) m_onError();
        if (m_sockfd != -1) {
            ::close(m_sockfd);
            m_sockfd = -1;
        }
        m_connecting = false;
    }

    void TcpClient::Connector::handleWrite() {
        if (m_sockfd == -1) return;

        m_loop->remove_fd(m_sockfd);

        int err;
        socklen_t len = sizeof(err);
        if (::getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &err, &len) == -1) {
            handleError();
            return;
        }

        if (err != 0) {
            handleError();
            return;
        }

        auto conn = std::make_shared<TcpConnection>(m_loop, m_sockfd, m_serverAddr);
        conn->set_message_callback(m_onMessage);
        conn->set_close_callback(m_onClose);
        conn->connection_established();
        m_onConnect(conn);
        m_sockfd = -1;
        m_connecting = false;
    }
    void TcpClient::Connector::cancel() {
        if (!m_connecting) return;
        removeAndClose();
    }

    void TcpClient::Connector::removeAndClose() {
        if (m_sockfd != -1) {
            m_loop->remove_fd(m_sockfd);
            ::close(m_sockfd);
            m_sockfd = -1;
        }    
        m_connecting = false;
    }

    TcpClient::TcpClient(EventLoop* loop) : 
        m_connector(std::make_unique<Connector>(loop)) { }

    TcpClient::~TcpClient() = default;

    void TcpClient::connect(const InetAddress& serverAddr, ConnectionCallback onConnect, MessageCallback onMessage,
        CloseCallback onClose, ErrorCallback onError) {
            m_connector->start(serverAddr, onConnect,
                onMessage, onClose, onError);
            
        }
    
    void TcpClient::cancel() {
        m_connector->cancel();
    }
} // namespace net