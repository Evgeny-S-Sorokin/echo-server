#pragma once

#include "co_instruments/co_socket.hpp"

#include <utility>

struct epoll_event;

namespace echo_servers
{

class CoListenerSocket final : public instruments::CoSocket
{
public:
    explicit CoListenerSocket(uint16_t port);
    virtual ~CoListenerSocket() noexcept = default;

    virtual int OnEvent() noexcept override;
    virtual void fillEpollEvent(epoll_event* newEvent) noexcept override;
};

} // namespace echo_servers