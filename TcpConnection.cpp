#include "TcpConnection.hpp"

void TcpConnection::set_connection_callback(ConnectionCallback cb) {
    m_connection_cb = cb;
}
    
void TcpConnection::set_message_callback(MessageCallback cb) {
    m_message_cb = cb;
}

void TcpConnection::set_close_callback(CloseCallback cb) {
    m_close_cb = cb;
}
int TcpConnection::fd() {
    return m_client_socket->fd();
}

std::string TcpConnection::peer_address() const {
    return m_peer_addr.to_ip();
}

uint16_t TcpConnection::peer_port() const {
    return m_peer_addr.to_port();
}

TcpConnection::TcpConnection(EventLoop* loop, int client_fd, const InetAddress& peerAddr) : 
    m_loop(loop), m_peer_addr(peerAddr), m_client_socket(new Socket(client_fd)), 
        m_state(Connecting) { 
            m_client_socket->set_nonblocking();
        }

void TcpConnection::connection_established() {
    m_state = Connected;
    m_loop->add_fd(m_client_socket->fd(), EPOLLIN | EPOLLET, [self = shared_from_this()](uint32_t flags) {
        self->handle_read();
    });

   if (m_connection_cb) {
       m_connection_cb(shared_from_this());
   }
}

void TcpConnection::handle_read() {
    char buf[4096];
    int n;
    while (true) {
        n = read(m_client_socket->fd(), buf, sizeof(buf));
        if (n > 0) {
            m_input_buffer.append(buf, n);
        }
        else if (n == 0) {
            handle_close();
            return;
        }
        else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            else if (errno == EINTR) {
                continue;
            }
            else {
                handle_error();
                return;
            }
        }
    }

    if (!m_input_buffer.empty()) {
        if (m_message_cb) {
            m_message_cb(shared_from_this(), m_input_buffer);
        }
    }
}

void TcpConnection::send(const std::string& data) {
    if (!m_output_buffer.empty()) {
        m_output_buffer.append(data);
    }
    else {
        int n = ::write(m_client_socket->fd(), data.c_str(), data.length());
        if (n < data.length()) {
            m_output_buffer.append(data.data() + n, data.length() - n);
            m_loop->update_fd(m_client_socket->fd(), EPOLLIN | EPOLLOUT | EPOLLET);
        }
    }
}

void TcpConnection::handle_write() {
    while(true) {
        int n = ::write(m_client_socket->fd(), m_output_buffer.c_str(), 
                    m_output_buffer.length());
        if (n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            handle_error();
            return;
        }
        if (n < m_output_buffer.length()) {
            m_output_buffer.erase(0, n);
            break;
        }
        else {
            m_loop->update_fd(m_client_socket->fd(), EPOLLIN | EPOLLET);
            if (m_state == Disconnecting) {
                handle_close();
            }
            break;
        }
    }
}

void TcpConnection::handle_close() {
    m_state = Disconnected;
    m_loop->remove_fd(m_client_socket->fd());
    
    if (m_connection_cb) {
        m_connection_cb(shared_from_this());
    }

    if (m_close_cb) {
        m_close_cb(shared_from_this());
    }
}

void TcpConnection::connection_destroyed() {
    if (m_state == Connected) {
        m_state = Disconnected;
    }
    m_loop->remove_fd(m_client_socket->fd());

    if (m_connection_cb) {
        m_connection_cb(shared_from_this());
    }
}

void TcpConnection::shutdown() {
    if (m_state == Connected) {
        m_state = Disconnecting;
    }
    if (m_output_buffer.empty()) {
        handle_close();
    }
}

void TcpConnection::handle_error() {
    handle_close();
}