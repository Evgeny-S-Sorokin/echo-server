#pragma once

#include "base_server/epoll_server.hpp"
#include "co_server/co_echo_socket.hpp"
#include "co_server/co_listener_socket.hpp"
#include "co_server/co_routines.hpp"

#include <coroutine>

namespace echo_servers
{

class CoServer final : public EpollServer
{
public:
    explicit CoServer();
    virtual ~CoServer() noexcept = default;

    void Run() override;

private:
    bool v_run = true;

private:
    template<typename FdWrapper>
    void AddToEpoll(FdWrapper& fdWrapper) noexcept
    {
        epoll_event newEvent;
        fdWrapper.fillEpollEvent(&newEvent);
        AddFdToEpoll(fdWrapper.getFd(), &newEvent);
    }

    template<typename FdWrapper>
    void ModifyInEpoll(FdWrapper& fdWrapper) noexcept
    {
        epoll_event modifiedEvent;
        fdWrapper.fillEpollEvent(&modifiedEvent);
        ModifyFdInEpoll(fdWrapper.getFd(), &modifiedEvent);
    }

    template<typename FdWrapper>
    void RemoveFromEpoll(const FdWrapper& fdWrapper) noexcept
    {
        RemoveFdFromEpoll(fdWrapper.getFd());
    }

    instruments::CoListenTask Listen();
    instruments::CoEchoTask EchoRecv(int fd);

}; // class CoServer

} // namespace echo_servers