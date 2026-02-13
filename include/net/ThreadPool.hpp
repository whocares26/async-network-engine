#pragma once

#include <vector>
#include <thread>
#include <memory>
#include <exception>
#include "net/EventLoop.hpp"
#include "net/TcpServer.hpp"

namespace net {

class ThreadPool {
public:
    using LoopCreator = std::function<std::unique_ptr<TcpServer>()>;
    ThreadPool(int num_threads, LoopCreator loop_creator);

    void start();

    void stop();

    ~ThreadPool();

private:
    const int m_num_threads;
    const LoopCreator m_loop_creator;

    std::vector<EventLoop*> m_loops;
    std::vector<std::thread> m_threads;
    bool m_started = false;
    bool m_stopped = false;
};

}