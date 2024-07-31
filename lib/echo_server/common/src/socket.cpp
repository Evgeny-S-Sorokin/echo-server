#include "instruments/socket.hpp"

#include <iostream>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace echo_servers
{
namespace instruments
{

Socket::Socket(int fd) noexcept
    : v_fd(fd)
{}

Socket::~Socket() noexcept
{
    if (v_fd > 0)
    {
        close(v_fd);
    }
}

int Socket::accept() noexcept
{
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    return accept4(v_fd, 
        reinterpret_cast<sockaddr*>(&clientAddr),
        &clientAddrLen,
        SOCK_NONBLOCK);
}

int Socket::send(char *buffer, std::size_t len) noexcept
{
    return ::send(v_fd, buffer, len, 0);
}

int Socket::recv(char *buffer, std::size_t len) noexcept
{
    return ::recv(v_fd, buffer, len, 0);
}

int Socket::CreateListeningSocket(uint16_t port) noexcept
{
    auto fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (fd < 0)
    {
        std::cout << "Failed to create a socket.\n"
            << "Errno: {" << errno << "}\n";
        return -1;
    }
    std::cout << "Successfully created a socket fd: {" << fd << "}\n";

    sockaddr_in serverAddr{0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0)
    {
        std::cout << "Failed to bind a socket.\n"
            << "Errno: {" << errno << "}\n";
        return -1;
    }
    std::cout << "Successfully binded socket to port: {" << port << "}\n";

    if (listen(fd, c_MAX_BACKLOG) < 0)
    {
        std::cout << "Failed to start listening a socket.\n"
            << "Errno: {" << errno << "}\n";
        return -1;
    }
    std::cout << "Socket " << fd << " is listening for connections...\n";

    return fd;
}

} // namespace instruments
} // namespace echo_servers