#include "co_server/co_server.hpp"

#include "co_server/co_echo_socket.hpp"
#include "co_server/co_listener_socket.hpp"
#include "co_instruments/socket_awaiter.hpp"
#include "instruments/socket.hpp"

#include <coroutine>
#include <iostream>
#include <vector>

#include <errno.h>
#include <stdexcept>
#include <sys/epoll.h>

namespace echo_servers
{

CoServer::CoServer()
{
    if (! SetupEpoll())
    {
        throw std::runtime_error("failed to setup Epoll");
    }
}

void CoServer::Run()
{
    Listen();

    epoll_event events[c_MAX_EVENTS];
    while (v_run)
    {
        auto totalEvents = WaitOnEpoll(events);
        for (auto i = 0; i < totalEvents; ++i)
        {
            static_cast<std::coroutine_handle<>*>(events[i].data.ptr)->resume();
        }
    }
}

template<class Socket>
void CoServer::AddToEpoll(Socket& socket)
{
    std::cout << "Adding socket to epoll\n";
    epoll_event newEvent;
    socket.fillEpollEvent(&newEvent);
    std::cout << newEvent.data.fd << '\n';
    AddSocketToEpoll(&newEvent);
}

CoListenTask CoServer::Listen()
{
    CoListenerSocket listener(12555);
    AddToEpoll(listener);
    std::vector<CoEchoTask> echoTasks;
    while (true)
    {
        auto newSocketFd = co_await instruments::SocketAwaiter(listener);
        echoTasks.push_back(EchoRecv(newSocketFd));
        // TODO:: Memory handling
    }
}

CoEchoTask CoServer::EchoRecv(int fd)
{
    bool continueRecv = true;
    CoEchoSocket<1024> echo(fd);
    auto echoRecv = echo.receive();
    AddToEpoll(echoRecv);
    while (continueRecv)
    {
        auto dataRecv = co_await instruments::SocketAwaiter(echoRecv);
        // TODO: Error handling
        auto dataSendTotal = 0;
        auto echoSend = echo.send(dataRecv);
        AddToEpoll(echoSend);
        while (dataSendTotal < dataRecv)
        {
            auto dataSend = co_await instruments::SocketAwaiter(echoSend);
            // TODO: Error handling
            dataSendTotal += dataSend;
        }
        // TODO: Handle echoSend deletion
        continueRecv = dataRecv != 0;
    }
    // TODO: Handle echoRecv deletion
}

} // namespace echo_servers