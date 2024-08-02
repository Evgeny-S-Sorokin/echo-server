#pragma once

#include "co_instruments/socket_awaiter.hpp"
#include "co_instruments/co_socket_handle.hpp"
#include "instruments/socket.hpp"

#include "co_socket.hpp"

#include <array>
#include <cstring>

namespace echo_servers
{

class CoRecvSocket final
    : public CoSocket
    , public instruments::Socket
{
public:
    explicit CoRecvSocket(int fd, char* buffer, std::size_t bufferSize) noexcept;
    virtual ~CoRecvSocket() noexcept = default;

    virtual int IsNextActionReady() noexcept override
    {
        return PerformNextAction();
    }
    virtual int PerformNextAction() noexcept override;
    virtual void fillEpollEvent(epoll_event* newEvent) noexcept override;

private:
    char* v_externalBufferPtr;
    std::size_t v_externalBufferLen;

    std::coroutine_handle<> v_handle;
};

class CoSendSocket final
    : public CoSocket
    , public instruments::Socket
{
public:
    explicit CoSendSocket(int fd, char* buffer, std::size_t bufferSize) noexcept;
    virtual ~CoSendSocket() noexcept = default;

    virtual int IsNextActionReady() noexcept override
    {
        return PerformNextAction();
    }
    virtual int PerformNextAction() noexcept override;
    virtual void fillEpollEvent(epoll_event* newEvent) noexcept override;

private:
    char* v_externalBufferPtr;
    std::size_t v_externalBufferLen;
};

template<std::size_t BufferSize>
class CoEchoSocket final : private instruments::Socket
{
public:
    explicit CoEchoSocket(int fd) noexcept
        : instruments::Socket(fd)
    {
        std::memset(v_buffer.data(), 0, v_buffer.size());
        
    }

    CoRecvSocket receive()
    {
        return CoRecvSocket(v_fd, v_buffer.data(), v_buffer.size());
    }

    CoSendSocket send(std::size_t dataLen)
    {
        return CoSendSocket(v_fd,
            v_buffer.data(),
            std::min(v_buffer.size(), dataLen));
    }

private:
    std::array<char, BufferSize> v_buffer;
};

} // namespace echo_servers