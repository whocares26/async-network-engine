#pragma once

#include <unordered_map>
#include <functional>
#include <cstdint>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <exception>
#include <system_error>
#include <vector>
#include <atomic>
class EventLoop {
public:
    // 1. Публичные типы (интерфейс для пользователя)
    using Callback = std::function<void(uint32_t)>;

    // 2. Жизненный цикл (создание/удаление)
    EventLoop();
    ~EventLoop();

    // 3. Запрет копирования (защита ресурсов)
    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;

    // 4. Основное API
    void add_fd(int fd, uint32_t events, Callback cb);
    void remove_fd(int fd);
    void update_fd(int fd, uint32_t events);
    void run();
    void stop();

private:
    // 5. Детали реализации (всегда в конце)
    int m_epoll_fd = -1;
    int m_stop_fd = -1;
    std::atomic<bool> m_running;
    std::unordered_map<int, Callback> m_callbacks;
};
