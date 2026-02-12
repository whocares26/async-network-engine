#include "net/EventLoop.hpp"
#include "net/TcpServer.hpp"
#include "net/InetAddress.hpp"
#include <iostream>

int main() {
    try {
        EventLoop loop;
        InetAddress listenAddr(8080, "0.0.0.0");
        
        TcpServer server(&loop, listenAddr);
        
        server.set_connection_callback([](const std::shared_ptr<TcpConnection>& conn) {
            if (conn->state() == TcpConnection::Connected) {
                std::cout << "Клиент подключился: "
                          << conn->peer_address() << ":" << conn->peer_port() << std::endl;
                
                conn->send("HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/plain\r\n"
                          "Content-Length: 13\r\n"
                          "\r\n"
                          "Hello, World!");
                conn->shutdown();
            } else {
                std::cout << "Клиент отключился" << std::endl;
            }
        });
        
        server.start();
        std::cout << "Сервер запущен на http://localhost:8080" << std::endl;
        
        loop.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}