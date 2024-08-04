#pragma once

#include <array>
#include <queue>

namespace echo_servers
{
namespace instruments
{

template<typename Task, std::size_t Size>
class TaskStorage
{
    TaskStorage() = default;
    ~TaskStorage() noexcept = default;

    TaskStorage(const TaskStorage&) = delete;
    TaskStorage& operator=(const TaskStorage&) = delete;
    TaskStorage(TaskStorage&&) = delete;
    TaskStorage& operator=(TaskStorage&&) = delete;

private:
    std::array<char, sizeof(Task) * Size> v_buffer;
    std::queue<void*> v_readyBuffer;
};

} // namespace instruments
} // namespace echo_servers