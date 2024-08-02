#include "co_server/co_server.hpp"

#include "co_server/co_echo_socket.hpp"
#include "co_server/co_listener_socket.hpp"
#include "co_instruments/co_socket_handle.hpp"
#include "co_instruments/socket_awaiter.hpp"
#include "instruments/socket.hpp"

#include <coroutine>
#include <iostream>
#include <vector>

#include <errno.h>
#include <stdexcept>
#include <sys/epoll.h>
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

template<class Socket>
void CoServer::AddToEpoll(Socket& socket)
{
    std::cout << "Adding socket to epoll\n";
    epoll_event newEvent;
    socket.fillEpollEvent(&newEvent);
    AddSocketToEpoll(socket.getFd(), &newEvent);
}

instruments::CoListenTask CoServer::Listen()
{
    CoListenerSocket listener(12555);
    AddToEpoll(listener);
    std::vector<instruments::CoEchoTask> echoTasks;
    while (true)
    {
        auto newSocketFd 
            = co_await instruments::SocketAwaiter(listener);
        std::cout << "Accepted new connection on fd {" << newSocketFd << "}\n";
        echoTasks.push_back(EchoRecv(newSocketFd));
        // TODO:: Memory handling
    }
}

instruments::CoEchoTask CoServer::EchoRecv(int fd)
{
    bool continueRecv = true;
    CoEchoSocket<1024> echo(fd);
    auto receiver = echo.receive();
    AddToEpoll(receiver);
    while (continueRecv)
    {
        auto dataRecv = co_await instruments::SocketAwaiter(receiver);
        // TODO: Error handling
        auto dataSendTotal = 0;
        std::cout << "Receveived [" << dataRecv << "] bytes\n";
        auto sender = echo.send(dataRecv);
        while (dataSendTotal < dataRecv)
        {
            auto dataSend = co_await instruments::SocketAwaiter(sender);
            // TODO: Error handling
            dataSendTotal += dataSend;
            std::cout << "Sent [" << dataSend << "] bytes\n";
        }
        // TODO: Handle echoSend deletion
        continueRecv = (dataRecv != 0);
    }
    // TODO: Handle echoRecv deletion
}

} // namespace echo_servers