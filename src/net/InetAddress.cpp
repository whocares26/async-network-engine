#include <net/InetAddress.hpp>

namespace net {

InetAddress::InetAddress(uint16_t port, std::string ip) {
    m_addr.sin_family = AF_INET;
    int ec = inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr);
    if (ec == -1) {
        throw std::system_error(errno, 
            std::generic_category(), "Failed to translate ip into big endian");
    }
    if (ec == 0) {
        throw std::runtime_error{"InetAddress: IP is not valid"};
    }
    m_addr.sin_port = htons(port);
}

InetAddress::InetAddress(const struct sockaddr_in& addr) : m_addr(addr) { }

const sockaddr* InetAddress::get_sockaddr() const {
    return reinterpret_cast<const sockaddr*>(&m_addr);
}

sockaddr* InetAddress::get_sockaddr() {
    return reinterpret_cast<sockaddr* >(&m_addr);
}

socklen_t InetAddress::get_length() const {
    return sizeof(m_addr);
}

std::string InetAddress::to_ip() const {
    char buffer[INET_ADDRSTRLEN];
    auto ec = inet_ntop(AF_INET, &m_addr.sin_addr, buffer, INET_ADDRSTRLEN);
    if (ec == nullptr) {
        throw std::system_error(errno, 
            std::generic_category(), "Failed to translate sin_addr bytes into ip");      
    }
    return buffer;
}

uint16_t InetAddress::to_port() const {
    return ntohs(m_addr.sin_port);
}

}