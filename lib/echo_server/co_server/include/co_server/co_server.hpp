#pragma once

#include "base_server/epoll_server.hpp"
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

    CoListenTask Listen();
    CoEchoTask EchoRecv(int fd);

}; // class CoServer

struct CoListenTask
{
    struct CoListenTaskPromise
    {
        CoListenTask get_return_object() 
        { 
            return {}; 
        }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };
    using promise_type = CoListenTaskPromise;
};

struct CoEchoTask
{
    struct CoEchoTaskPromise
    {
        CoEchoTask get_return_object() 
        { 
            return {}; 
        }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };
    using promise_type = CoEchoTaskPromise;
};

} // namespace echo_servers