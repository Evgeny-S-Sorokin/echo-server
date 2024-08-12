#pragma once

#include <concepts>
#include <cstring>

#include <sys/epoll.h>

struct epoll_event;

namespace echo_servers
{

class EpollServer
{
public:
    EpollServer() noexcept
    {
        std::memset(events, 0, sizeof(epoll_event) * c_MAX_EVENTS);
    }
    virtual ~EpollServer() noexcept
    {
        ShutdownEpoll();
    }
    EpollServer(const EpollServer&) = delete;
    EpollServer& operator=(const EpollServer&) = delete;

    virtual void Run() = 0;

protected:
    inline static constexpr int c_MAX_EVENTS = 128; // maximum amount of events returned by epoll

protected:
    bool SetupEpoll() noexcept;
    void ShutdownEpoll() noexcept;

    bool AddFdToEpoll(int fd, epoll_event* event) noexcept;
    bool ModifyFdInEpoll(int fd, epoll_event* event) noexcept;
    bool RemoveFdFromEpoll(int fd) noexcept;

    template<typename Action>
        requires std::invocable<Action&&, int, epoll_event*>
    void WaitForEvent(Action&& onEvent)
    {
        auto totalEvents = WaitOnEpoll(events);
        onEvent(totalEvents, events);
    }

private:
    int v_epollFd = -1;
    epoll_event events[c_MAX_EVENTS];

private:
    int WaitOnEpoll(epoll_event* events) noexcept;
};

} // namespace echo_servers