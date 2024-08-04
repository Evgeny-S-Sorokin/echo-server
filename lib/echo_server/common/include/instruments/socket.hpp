#pragma once

#include <cstdint>

namespace echo_servers
{
namespace instruments
{

class Socket
{
public:
    inline static constexpr int c_MAX_BACKLOG = 64;

public:
    explicit Socket(int fd) noexcept;
    virtual ~Socket() noexcept = default;
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    int accept() noexcept;
    int send(char* buffer, std::size_t len) noexcept;
    int recv(char* buffer, std::size_t len) noexcept;

    int getFd() const noexcept { return v_fd; }

    static int CreateListeningSocket(uint16_t port) noexcept;

protected:
    const int v_fd;

}; // class Socket

} // namespace instruments
} // namespace echo_servers