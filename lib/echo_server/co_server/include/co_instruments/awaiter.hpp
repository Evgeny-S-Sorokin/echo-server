#pragma once

#include <concepts>
#include <coroutine>
#include <optional>
#include <type_traits>

namespace echo_servers
{
namespace instruments
{

template<typename Suspended, typename ReturnType>
class EpollAwaiter
{
public:
    EpollAwaiter()
        : v_returnValue()
    {
        static_assert(std::is_base_of_v<EpollAwaiter, Suspended>);
    }

    bool await_ready()
    {
        v_returnValue = static_cast<Suspended*>(this)->ShouldSuspend();
        return v_returnValue.has_value();
    }
    
    void await_suspend(std::coroutine_handle<> handle)
    {
        static_cast<Suspended*>(this)->Suspend(handle);
    }

    ReturnType await_resume()
    {
        return v_returnValue ? *v_returnValue
            : static_cast<Suspended*>(this)->Resume();
    }

protected:
    std::optional<ReturnType> v_returnValue;
};

} // namespace instruments
} // namespace echo_servers