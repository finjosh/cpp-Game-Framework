#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#pragma once

#include "BS_thread_pool.hpp"

class ThreadPool : public BS::thread_pool
{
public:
    inline ThreadPool() = default;
    /// @brief singleton Getter
    static ThreadPool& get();

protected:

private:
    ThreadPool(ThreadPool const&) = delete;
    void operator=(ThreadPool const&) = delete;
};

#endif
