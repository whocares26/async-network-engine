#include "net/EventLoop.hpp"
#include "net/TcpServer.hpp"
#include "net/InetAddress.hpp"
#include "net/ThreadPool.hpp"
#include <iostream>
#include <memory>
#include <vector>

int main() {
    try {
        unsigned int pool_size = std::thread::hardware_concurrency();
        if (pool_size == 0) pool_size = 4;
        
        std::vector<std::unique_ptr<EventLoop>> worker_loops(pool_size);
        for (auto& loop_ptr : worker_loops) {
            loop_ptr = std::make_unique<EventLoop>();
        }
        
        ThreadPool pool(pool_size, [&worker_loops]() {
            static std::atomic<size_t> index{0};
            size_t i = index.fetch_add(1) % worker_loops.size();
            EventLoop* loop = worker_loops[i].get();
            
            InetAddress listenAddr(8080, "0.0.0.0");
            auto server = std::make_unique<TcpServer>(loop, listenAddr);
            
            server->set_connection_callback([](const std::shared_ptr<TcpConnection>& conn) {
                if (conn->state() == TcpConnection::Connected) {
                    std::cout << std::this_thread::get_id() 
                              << " -> Клиент подключился: "
                              << conn->peer_address() << ":" << conn->peer_port() 
                              << std::endl;
                    
                    conn->send("HTTP/1.1 200 OK\r\n"
                              "Content-Type: text/plain\r\n"
                              "Content-Length: 13\r\n"
                              "\r\n"
                              "Hello, World!");
                    conn->shutdown();
                } else {
                    std::cout << std::this_thread::get_id() 
                              << " -> Клиент отключился" << std::endl;
                }
            });
            
            return server;
        });
        
        pool.start();
        std::cout << "TCP-движок запущен на порту 8080" << std::endl;
        std::cout << pool_size << " потоков" << std::endl;
        std::cout << "────────────────────────────────────────────" << std::endl;
        std::cout << "Нажмите Enter для остановки..." << std::endl;
        
        std::cin.get();
        
        std::cout << "Останавливаем TCP-движок" << std::endl;
        pool.stop();
        std::cout << "TCP-движок остановлен" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}