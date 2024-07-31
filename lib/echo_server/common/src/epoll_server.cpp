#include "base_server/epoll_server.hpp"

#include <cstring>
#include <iostream>
#include <string>

#include <unistd.h>
#include <sys/epoll.h>

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
    return epoll_wait(v_epollFd, events, c_MAX_EVENTS, -1);
}

bool EpollServer::AddSocketToEpoll(epoll_event* event) noexcept
{
    if (epoll_ctl(v_epollFd, EPOLL_CTL_ADD, event->data.fd, event) < 0)
    {
        std::cout << "Failed to add socket {" << event->data.fd << "} to epoll.\n"
            << "Errno: {" << errno << "}\n";
        return false;
    }

    return true;
}

void EpollServer::ShutdownEpoll() noexcept
{
    if (-1 != v_epollFd)
    {
        close(v_epollFd);
    }
}

} // namespace echo_servers