#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#pragma once

#include "BS_thread_pool.hpp"

/// @brief Initializes to the number of hardware threads
class ThreadPool : public BS::thread_pool
{
public:
    /// @brief singleton Getter
    static ThreadPool& get();

protected:

private:
    ThreadPool() : BS::thread_pool(std::thread::hardware_concurrency()) {};
    ThreadPool(ThreadPool const&) = delete;
    void operator=(ThreadPool const&) = delete;
};

#endif
