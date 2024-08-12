#pragma once

#include "co_server/co_listener_socket.hpp"

#include <coroutine>
#include <exception>
#include <iostream>
#include <new>

namespace echo_servers
{
namespace instruments
{

struct CoListenTask
{
    struct CoListenTaskPromise
    {
        using handleType = std::coroutine_handle<CoListenTaskPromise>;

        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        CoListenTask get_return_object() 
        { 
            return CoListenTask(handleType::from_promise(*this)); 
        }
        void return_void() {}

        void operator delete(void *ptr)
        {
            if (ptr)
            {
                std::cout << "CoListenTask deleted...\n";
                free(ptr);
            }
        }

        template<typename... Args>
        void *operator new(std::size_t size, Args &&...args)
        {
            auto buffer = std::malloc(size);
            std::cout << "CoListenTask allocated...\n";
            return buffer;
        }
    };
    using promise_type = CoListenTaskPromise;

    explicit CoListenTask(std::coroutine_handle<> handle)
        : v_handle(handle)
    {}
    CoListenTask(const CoListenTask&) = delete;
    CoListenTask& operator=(const CoListenTask&) = delete;
    CoListenTask(CoListenTask&&) = delete;
    CoListenTask& operator=(CoListenTask&&) = delete;
    ~CoListenTask() noexcept
    {
        v_handle.destroy();
    }

private:
    std::coroutine_handle<> v_handle = nullptr;
};

struct CoEchoTask
{
    struct CoEchoTaskPromise
    {
        using handle_type = std::coroutine_handle<CoEchoTaskPromise>;

        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        CoEchoTask get_return_object() 
        { 
            return CoEchoTask(handle_type::from_promise(*this)); 
        }
        void return_void() {}

        void operator delete(void *ptr)
        {
            if (ptr)
            {
                std::cout << "CoEchoTask deleted...\n";
                free(ptr);
            }
        }

        template<typename... Args>
        void *operator new(std::size_t size, Args &&...args) noexcept
        {
            auto buffer = std::malloc(size);
            std::cout << "CoEchoTask allocated...\n";
            return buffer;
        }
    };
    using promise_type = CoEchoTaskPromise;
    using handle_type = CoEchoTaskPromise::handle_type;

    explicit CoEchoTask(handle_type handle)
        : v_selfHandle(handle)
    {
        std::cout << "Echo task initialized...\n";
    }
    CoEchoTask(CoEchoTask&& other)
        : v_selfHandle(std::move(other.v_selfHandle))
    {
        other.v_selfHandle = nullptr;
    }
    CoEchoTask& operator=(CoEchoTask&& other)
    {
        v_selfHandle = std::move(other.v_selfHandle);
        other.v_selfHandle = nullptr;
        return *this;
    }
    ~CoEchoTask() noexcept
    {
        std::cout << "Echo task destroyed...\n";
        if (v_selfHandle)
        {
            v_selfHandle.destroy();
        }
    }

    CoEchoTask(const CoEchoTask&) = delete;
    CoEchoTask& operator=(const CoEchoTask&) = delete;

private:
    handle_type v_selfHandle = nullptr;    
};

} // namespace instruments
} // namespace echo_servers