#pragma once

struct epoll_event;

namespace echo_servers
{

class EpollServer
{
public:
    explicit EpollServer() noexcept = default;
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

    int WaitOnEpoll(epoll_event* events) noexcept;
    bool AddFdToEpoll(int fd, epoll_event* event) noexcept;
    bool ModifyFdInEpoll(int fd, epoll_event* event) noexcept;
    bool RemoveFdFromEpoll(int fd) noexcept;

private:
    int v_epollFd = -1;
};

} // namespace echo_servers