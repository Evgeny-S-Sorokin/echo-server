#include "co_server/co_listener_socket.hpp"

#include <optional>
#include <stdexcept>

#include <sys/epoll.h>
#include <unistd.h>

namespace echo_servers
{

CoListenerSocket::CoListenerSocket(uint16_t port)
    : instruments::Socket(instruments::Socket::CreateListeningSocket(port))
{
    if (v_fd < 0)
    {
        throw std::runtime_error("failed to create listening socket");
    }
}

CoListenerSocket::~CoListenerSocket() noexcept
{
    close(v_fd);
}

int CoListenerSocket::PerformNextAction() noexcept
{
    return accept();
}

void CoListenerSocket::fillEpollEvent(epoll_event *newEvent) noexcept
{
    newEvent->events = EPOLLIN;
    newEvent->data.ptr = this;
}

} // namespace echo_servers