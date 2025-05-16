#include "ThreadPool.hpp"

ThreadPool& ThreadPool::get()
{
    static ThreadPool* threadPool = new ThreadPool();

    return *threadPool;
}
