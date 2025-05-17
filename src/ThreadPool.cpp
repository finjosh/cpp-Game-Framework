#include "ThreadPool.hpp"

ThreadPool& ThreadPool::get()
{
    static ThreadPool threadPool;

    return threadPool;
}
