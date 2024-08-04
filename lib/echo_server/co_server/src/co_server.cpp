#include "co_server/co_server.hpp"

#include "co_server/co_echo_socket.hpp"
#include "co_server/co_listener_socket.hpp"
#include "co_instruments/co_socket_handle.hpp"
#include "co_instruments/socket_awaiter.hpp"
#include "instruments/socket.hpp"

#include <coroutine>
#include <iostream>
#include <unordered_map>

#include <errno.h>
#include <stdexcept>
#include <unistd.h>

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
    auto task = Listen();

    epoll_event events[c_MAX_EVENTS];
    auto counter = 0;
    while (v_run)
    {
        auto totalEvents = WaitOnEpoll(events);
        if (totalEvents < 0)
        {
            std::cout << "EPOLL FAILED!\n";
            v_run = false;
            return;
        }

        std::cout << "Got events: {" << totalEvents << "}\n";
        for (auto i = 0; i < totalEvents; ++i)
        {
            std::cout << "Event [" << i << ":" << events[i].events << "]\n";
            static_cast<CoSocket*>(events[i].data.ptr)->ResumeNextAction();
        }
    }
    
}

instruments::CoListenTask CoServer::Listen()
{
    CoListenerSocket listener(12555);
    AddToEpoll(listener);
    while (true)
    {
        auto newSocketFd 
            = co_await instruments::SocketAwaiter(listener);
        std::cout << "Accepted new connection on fd {" << newSocketFd << "}\n";
        co_await EchoRecv(newSocketFd);
        close(newSocketFd);
    }
    /*
    std::unordered_map<int, instruments::CoEchoTask> echoTasks;
    while (true)
    {
        auto newSocketFd 
            = co_await instruments::SocketAwaiter(listener);
        std::cout << "Accepted new connection on fd {" << newSocketFd << "}\n";
        echoTasks.insert_or_assign(newSocketFd,
            std::move(EchoRecv(newSocketFd)));
        // TODO:: Memory handling
    }
    */
}

instruments::CoEchoTask CoServer::EchoRecv(int fd)
{
    bool continueRecv = true;
    CoEchoSocket<1024> echo(fd);
    auto& receiver = echo.GetReceiverPart();
    AddToEpoll(receiver);
    while (continueRecv)
    {
        auto dataRecv = co_await instruments::SocketAwaiter(receiver);
        auto dataSendTotal = 0;
        std::cout << "Received [" << dataRecv << "] bytes\n";
        if (dataRecv > 1)
        {
            auto sender = echo.GetSenderPart(dataRecv);
            ModifyInEpoll(sender);
            while (dataSendTotal < dataRecv)
            {
                auto dataSend 
                    = co_await instruments::SocketAwaiter(sender);
                // TODO: Error handling
                dataSendTotal += dataSend;
                std::cout << "Sent [" << dataSend << "] bytes\n";
            }
            ModifyInEpoll(receiver);
        }
        continueRecv = (dataRecv > 1);
    }
    RemoveFromEpoll(echo);
    co_return;
}

} // namespace echo_servers