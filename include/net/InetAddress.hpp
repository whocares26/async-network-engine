#pragma once
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <exception>
#include <system_error>

namespace net {

class InetAddress {
public:
    InetAddress(uint16_t port, std::string ip = "0.0.0.0");

    explicit InetAddress(const struct sockaddr_in& addr);

    const sockaddr* get_sockaddr() const;
    sockaddr* get_sockaddr();

    socklen_t get_length() const;

    std::string to_ip() const;

    uint16_t to_port() const;

private:
    sockaddr_in m_addr{};
};

}