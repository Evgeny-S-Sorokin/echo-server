#include "co_server/co_echo_socket.hpp"

#include <stdexcept>

#include <sys/epoll.h>

namespace echo_servers
{

CoRecvSocket::CoRecvSocket(int fd, char* buffer, std::size_t bufferSize) noexcept
    : instruments::Socket(fd)
    , v_externalBufferPtr(buffer)
    , v_externalBufferLen(bufferSize)
{}

int CoRecvSocket::PerformNextAction()  noexcept
{
    return recv(v_externalBufferPtr, v_externalBufferLen);
}

void CoRecvSocket::fillEpollEvent(epoll_event *newEvent) noexcept
{
    newEvent->events = EPOLLIN | EPOLLET;
    newEvent->data.ptr = this;
}

CoSendSocket::CoSendSocket(int fd, char* buffer, std::size_t bufferSize) noexcept
    : instruments::Socket(fd)
    , v_externalBufferPtr(buffer)
    , v_externalBufferLen(bufferSize)
{}

int CoSendSocket::PerformNextAction()  noexcept
{
    return send(v_externalBufferPtr, v_externalBufferLen);
}

void CoSendSocket::fillEpollEvent(epoll_event *newEvent) noexcept
{
    newEvent->events = EPOLLOUT | EPOLLET;
    newEvent->data.ptr = this;
}

} // namespace echo_servers