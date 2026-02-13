#include "net/ThreadPool.hpp"

namespace net {

ThreadPool::ThreadPool(int num_threads, LoopCreator loop_creator) 
        : m_num_threads(num_threads), m_loop_creator(loop_creator) {
            if (num_threads <= 0) {
                throw std::runtime_error{"ThreadPool: construct failed, negative num of threads"};
            }
}

void ThreadPool::start() {
    if (!m_started) {
        m_threads.reserve(m_num_threads);
        m_loops.reserve(m_num_threads);
        for (size_t i = 0; i < m_num_threads; i++) {
            auto tcp_server = m_loop_creator();
            m_loops.push_back(tcp_server->get_loop());
            m_threads.emplace_back([server = std::move(tcp_server)]() {
                server->start();
                server->get_loop()->run();
            });
        }
        m_started = true;
    }
}

void ThreadPool::stop() {
    if (m_started) {
        if (!m_stopped) {
            for (size_t i = 0; i < m_num_threads; i++) {
                m_loops[i]->stop();
            }
            for (size_t i = 0; i < m_num_threads; i++) {
                m_threads[i].join();
            }
        }
        m_stopped = true;
    }
}

ThreadPool::~ThreadPool() {
    if (m_started && !m_stopped) {
        stop();
    }
}

}