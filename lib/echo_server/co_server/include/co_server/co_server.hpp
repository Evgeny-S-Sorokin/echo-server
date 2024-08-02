#pragma once

#include "base_server/epoll_server.hpp"
#include "co_instruments/co_routines.hpp"
#include "co_server/co_echo_socket.hpp"
#include "co_server/co_listener_socket.hpp"

#include <array>
#include <coroutine>

namespace echo_servers
{

struct CoListenTask;
struct CoEchoTask;

class CoServer final : public EpollServer
{
public:
    explicit CoServer();
    virtual ~CoServer() noexcept = default;

    void Run() override;

private:
    bool v_run = true;

private:
    template<class Socket>
    void AddToEpoll(Socket& socket);

    instruments::CoListenTask Listen();
    instruments::CoEchoTask EchoRecv(int fd);

}; // class CoServer

} // namespace echo_servers