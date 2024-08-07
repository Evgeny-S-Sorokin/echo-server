#include "co_server/co_server.hpp"

#include "co_instruments/co_routines.hpp"
#include "co_server/co_echo_socket.hpp"
#include "co_server/co_listener_socket.hpp"
#include "co_instruments/co_socket_handle.hpp"
#include "co_instruments/socket_awaiter.hpp"
#include "instruments/socket.hpp"

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

    auto onEvent = [this](int totalEvents, epoll_event* events)
        {
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
        };
    
    while (v_run)
    {
        WaitForEvent(onEvent);
    }
}

instruments::CoListenTask CoServer::Listen()
{
    CoListenerSocket listener(12555);
    AddToEpoll(listener);
    // Case for single connection. No extra allocations
    /*
    while (true)
    {
        auto newSocketFd 
            = co_await instruments::SocketAwaiter(listener);
        std::cout << "Accepted new connection on fd {" << newSocketFd << "}\n";
        co_await EchoRecv(newSocketFd);
    }
    */
    std::unordered_map<int, instruments::CoEchoTask> tasks;
    while (true)
    {
        auto newSocketFd 
            = co_await instruments::SocketAwaiter(listener);
        std::cout << "Accepted new connection on fd {" << newSocketFd << "}\n";
        auto newTask = EchoRecv(newSocketFd);
        newTask.simple_resume();
        tasks.insert_or_assign(newSocketFd, std::move(newTask));
    }
    
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
        if (dataRecv > 0)
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
        continueRecv = (dataRecv > 0);
    }
    RemoveFromEpoll(echo);
    close(fd);
    co_return;
}

} // namespace echo_servers