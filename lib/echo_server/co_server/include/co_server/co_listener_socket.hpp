#pragma once

#include "co_instruments/socket_awaiter.hpp"
#include "instruments/socket.hpp"

#include "co_socket.hpp"

#include <utility>

struct epoll_event;

namespace echo_servers
{

class CoListenerSocket final
    : public CoSocket
    , public instruments::Socket
{
public:
    explicit CoListenerSocket(uint16_t port);
    virtual ~CoListenerSocket() noexcept;

    virtual int IsNextActionReady() noexcept override 
    { 
        return -1; 
    }
    virtual int PerformNextAction() noexcept override;
    virtual void fillEpollEvent(epoll_event* newEvent) noexcept override;
};

} // namespace echo_servers