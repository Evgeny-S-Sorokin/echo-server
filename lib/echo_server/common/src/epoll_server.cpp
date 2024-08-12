#include "base_server/epoll_server.hpp"

#include <cerrno>
#include <iostream>
#include <string>

#include <unistd.h>

namespace echo_servers
{

bool EpollServer::SetupEpoll() noexcept
{
    v_epollFd = epoll_create(c_MAX_EVENTS);
    if (v_epollFd < 0)
    {
        std::cout << "Failed to create epoll\n"
            << "Errno: {" << errno << "}\n";
        return false;
    }
    std::cout << "Successfully created an epoll fd: {" << v_epollFd << "}\n";

    return true;
}

int EpollServer::WaitOnEpoll(epoll_event* events) noexcept
{
    std::cout << "Awaiting ...\n";
    auto result = epoll_wait(v_epollFd, events, c_MAX_EVENTS, -1);
    return result < 0 && (errno == EAGAIN || errno == EWOULDBLOCK) ? 0 : result;
}

bool EpollServer::AddFdToEpoll(int fd, epoll_event* event) noexcept
{
    if (epoll_ctl(v_epollFd, EPOLL_CTL_ADD, fd, event) < 0)
    {
        std::cout << "Failed to add fd {" << fd << "} to epoll.\n"
            << "Errno: {" << errno << "}\n";
        return false;
    }

    std::cout << "Fd {" << fd << "} added to epoll.\n";
    return true;
}

bool EpollServer::ModifyFdInEpoll(int fd, epoll_event* event) noexcept
{
    if (epoll_ctl(v_epollFd, EPOLL_CTL_MOD, fd, event) < 0)
    {
        std::cout << "Failed to modify fd {" << fd << "} in epoll.\n"
            << "Errno: {" << errno << "}\n";
        return false;
    }

    std::cout << "Fd {" << fd << "} modified in epoll.\n";
    return true;
}

bool EpollServer::RemoveFdFromEpoll(int fd) noexcept
{
    if (epoll_ctl(v_epollFd, EPOLL_CTL_DEL, fd, nullptr) < 0)
    {
        std::cout << "Failed to remove fd {" << fd << "} from epoll.\n"
            << "Errno: {" << errno << "}\n";
        return false;
    }

    std::cout << "Fd {" << fd << "} removed from epoll.\n";
    return true;
}

void EpollServer::ShutdownEpoll() noexcept
{
    if (-1 != v_epollFd)
    {
        close(v_epollFd);
        v_epollFd = -1;
    }
}

} // namespace echo_servers