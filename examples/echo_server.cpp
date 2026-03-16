#include <iostream>
#include <memory>
#include "net/EventLoop.hpp"
#include "net/TcpServer.hpp"
#include "net/InetAddress.hpp"

using namespace net;

int main(int argc, char* argv[]) {
    try {
        int port = 8080;
        if (argc > 1) {
            port = std::stoi(argv[1]);
        }
        
        EventLoop loop;
        InetAddress listenAddr(port, "0.0.0.0");
        auto server = std::make_unique<TcpServer>(&loop, listenAddr);
        
        server->set_connection_callback([](const std::shared_ptr<TcpConnection>& conn) {
            if (conn->state() == TcpConnection::Connected) {
                std::cout << "[+] Client connected from " 
                          << conn->peer_address() << ":" << conn->peer_port() << std::endl;
                
                // Эхо - отправляем данные обратно клиенту
                conn->set_message_callback([](const std::shared_ptr<TcpConnection>& c, std::string& data) {
                    // Возвращаем данные отправителю (эхо)
                    c->send(data);
                });
            } else {
                std::cout << "[-] Client disconnected" << std::endl;
            }
        });
        
        std::cout << "📡 Echo Server starting on port " << port << "...\n";
        std::cout << "────────────────────────────────────────────\n";
        
        server->start();
        
        std::cout << "Server is running. Press Ctrl+C to stop.\n";
        
        loop.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
