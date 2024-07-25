#include "UpdateManager.hpp"
#include "ObjectManager.hpp"

std::unordered_set<UpdateInterface*> UpdateManager::m_objects;

void UpdateManager::addUpdateObject(UpdateInterface* obj)
{
    m_objects.insert({obj});
}

void UpdateManager::removeUpdateObject(UpdateInterface* obj)
{
    m_objects.erase(obj);
}

// std::unordered_set<UpdateInterface*>::iterator getIterator(std::unordered_set<UpdateInterface*>::iterator start, uint64_t n)
// {
//     for (uint64_t i = 0; i < n; i++)
//     {
//         start++;
//     }
//     return start;
// }

// void UpdateManager::Update(float deltaTime, BS::thread_pool& pool)
// {
//     std::list<std::future<void>> threads;

//     uint64_t partitionSize = m_objects.size()/pool.get_thread_count();
//     uint64_t extra = m_objects.size()%pool.get_thread_count();
//     uint64_t temp = 0;
//     std::unordered_set<UpdateInterface*>::iterator current = m_objects.begin();
//     std::unordered_set<UpdateInterface*>::iterator next;
//     for (int i = 0; i < pool.get_thread_count(); i++)
//     {
//         temp = (extra > 0 ? 1 : 0);
//         next = getIterator(current, partitionSize + temp);
//         threads.emplace_back(pool.submit_task([deltaTime, current, next]{ _update(deltaTime, current, next); }));
//         current = next;
//         extra -= temp;
//     }

//     for (auto& thread: threads)
//     {
//         thread.wait();
//     }

//     threads.clear();
// }

// void UpdateManager::LateUpdate(float deltaTime, BS::thread_pool& pool)
// {
//     std::list<std::future<void>> threads;

//     uint64_t partitionSize = m_objects.size()/pool.get_thread_count();
//     uint64_t extra = m_objects.size()%pool.get_thread_count();
//     uint64_t temp = 0;
//     std::unordered_set<UpdateInterface*>::iterator current = m_objects.begin();
//     std::unordered_set<UpdateInterface*>::iterator next;
//     for (int i = 0; i < pool.get_thread_count(); i++)
//     {
//         temp = (extra > 0 ? 1 : 0);
//         next = getIterator(current, partitionSize + temp);
//         threads.emplace_back(pool.submit_task([deltaTime, current, next]{ _lateUpdate(deltaTime, current, next); }));
//         current = next;
//         extra -= temp;
//     }

//     for (auto& thread: threads)
//     {
//         thread.wait();
//     }

//     threads.clear();
// }

// void UpdateManager::FixedUpdate(BS::thread_pool& pool)
// {
//     std::list<std::future<void>> threads;

//     uint64_t partitionSize = m_objects.size()/pool.get_thread_count();
//     uint64_t extra = m_objects.size()%pool.get_thread_count();
//     uint64_t temp = 0;
//     std::unordered_set<UpdateInterface*>::iterator current = m_objects.begin();
//     std::unordered_set<UpdateInterface*>::iterator next;
//     for (int i = 0; i < pool.get_thread_count(); i++)
//     {
//         temp = (extra > 0 ? 1 : 0);
//         next = getIterator(current, partitionSize + temp);
//         threads.emplace_back(pool.submit_task([current, next]{ _fixedUpdate(current, next); }));
//         current = next;
//         extra -= temp;
//     }

//     for (auto& thread: threads)
//     {
//         thread.wait();
//     }

//     threads.clear();
// }

// void UpdateManager::Start(BS::thread_pool& pool)
// {
//     std::list<std::future<void>> threads;

//     uint64_t partitionSize = m_objects.size()/pool.get_thread_count();
//     uint64_t extra = m_objects.size()%pool.get_thread_count();
//     uint64_t temp = 0;
//     std::unordered_set<UpdateInterface*>::iterator current = m_objects.begin();
//     std::unordered_set<UpdateInterface*>::iterator next;
//     for (int i = 0; i < pool.get_thread_count(); i++)
//     {
//         temp = (extra > 0 ? 1 : 0);
//         next = getIterator(current, partitionSize + temp);
//         threads.emplace_back(pool.submit_task([current, next]{ _start(current, next); }));
//         current = next;
//         extra -= temp;
//     }

//     for (auto& thread: threads)
//     {
//         thread.wait();
//     }

//     threads.clear();
// }

void UpdateManager::_update(float deltaTime, std::unordered_set<UpdateInterface*>::iterator begin, std::unordered_set<UpdateInterface*>::iterator end)
{
    while (begin != end)
    {
        if ((*begin)->isEnabled())
            (*begin)->Update(deltaTime);
        begin++;
    }
}

void UpdateManager::_lateUpdate(float deltaTime, std::unordered_set<UpdateInterface*>::iterator begin, std::unordered_set<UpdateInterface*>::iterator end)
{
    while (begin != end)
    {
        if ((*begin)->isEnabled())
            (*begin)->LateUpdate(deltaTime);
        begin++;
    }
}

void UpdateManager::_fixedUpdate(std::unordered_set<UpdateInterface*>::iterator begin, std::unordered_set<UpdateInterface*>::iterator end)
{
    while (begin != end)
    {
        if ((*begin)->isEnabled())
            (*begin)->FixedUpdate();
        begin++;
    }
}

void UpdateManager::_start(std::unordered_set<UpdateInterface*>::iterator begin, std::unordered_set<UpdateInterface*>::iterator end)
{
    while (begin != end)
    {
        (*begin)->Start();
        begin++;
    }
}

uint64_t UpdateManager::getNumberOfObjects()
{
    return m_objects.size();
}
