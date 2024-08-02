#pragma once

#include "co_instruments/co_socket_handle.hpp"

class epoll_event;

namespace echo_servers
{

class CoSocket : public instruments::CoSocketHandle
{
public:
    virtual int IsNextActionReady() = 0;
    virtual int PerformNextAction() = 0;
    virtual void fillEpollEvent(epoll_event* newEvent) noexcept = 0;
};

} // namespace echo_server