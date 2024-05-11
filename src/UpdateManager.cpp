#include "UpdateManager.hpp"
#include "ObjectManager.hpp"

std::unordered_set<UpdateInterface*> UpdateManager::m_objects;
std::unordered_set<UpdateInterface*>::iterator UpdateManager::m_iterator;
std::mutex UpdateManager::m_iteratorLock;

void UpdateManager::addUpdateObject(UpdateInterface* obj)
{
    m_objects.insert({obj});
}

void UpdateManager::removeUpdateObject(UpdateInterface* obj)
{
    m_objects.erase(obj);
}

// Different ways to try
// - give each thread a range of objects to update (could be bad if all the objects in one thread have long updates)
// ! the more threads there are the longer smaller updates are
//* faster large updates
//* only 2 loops through the list no matter the # of threads
    // - give each thread a range then allow them to continue if done early (would be difficult to implement properly)
    //! requires more memory for each object (probably not worth the performance boost)
// - give each object a atomic<bool> for if it was updated this frame (would have to reset the var after each update)
//! probably bad option (# of threads loops through the objects set no matter what)
void UpdateManager::Update(float deltaTime, int numThreads, BS::thread_pool& pool)
{
    m_iterator = m_objects.begin();

    if (numThreads != 0)
    {
        std::list<std::future<void>> threads;

        for (int i = 0; i < numThreads; i++)
        {
            threads.emplace_back(pool.submit_task([deltaTime]{ _update(deltaTime); }));
        }

        for (auto& thread: threads)
        {
            thread.wait();
        }

        threads.clear();
    }
    else
    {
        _update(deltaTime);
    }

    // std::unordered_set<UpdateInterface*>::iterator i = m_objects.begin();
    // while (i != m_objects.end())
    // {
    //     // in the case that the obj is destroyed during call
    //     UpdateInterface* temp = *i;
    //     i++;
    //     if (temp->isEnabled())
    //         temp->Update(deltaTime);
    // }

    ObjectManager::ClearDestroyQueue();
}

std::unordered_set<UpdateInterface*>::iterator getIterator(std::unordered_set<UpdateInterface*>::iterator start, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        start++;
    }
    return start;
}

void UpdateManager::Update2(float deltaTime, int numThreads, BS::thread_pool& pool)
{
    if (numThreads != 0)
    {
        std::list<std::future<void>> threads;

        size_t partitionSize = m_objects.size()/numThreads;
        size_t extra = m_objects.size()%numThreads;
        size_t temp = 0;
        std::unordered_set<UpdateInterface*>::iterator current = m_objects.begin();
        std::unordered_set<UpdateInterface*>::iterator next;
        for (int i = 0; i < numThreads; i++)
        {
            temp = (extra > 0 ? 1 : 0);
            next = getIterator(current, partitionSize + temp);
            threads.emplace_back(pool.submit_task([deltaTime, current, next]{ _update2(deltaTime, current, next); }));
            current = next;
            extra -= temp;
        }

        for (auto& thread: threads)
        {
            thread.wait();
        }

        threads.clear();
    }
    else
    {
        _update2(deltaTime, m_objects.begin(), m_objects.end());
    }

    ObjectManager::ClearDestroyQueue();
}

void UpdateManager::_update(float deltaTime)
{
    m_iteratorLock.lock();
    if (m_iterator == m_objects.end())
    {
        m_iteratorLock.unlock();
        return;
    }
    std::unordered_set<UpdateInterface*>::iterator i = m_iterator++;
    m_iteratorLock.unlock();

    while (true)
    {
        if ((*i)->isEnabled())
            (*i)->Update(deltaTime);
        m_iteratorLock.lock();
        if (m_iterator == m_objects.end())
        {
            m_iteratorLock.unlock();
            return;
        }
        i = m_iterator++;
        m_iteratorLock.unlock();
    }
}

void UpdateManager::_update2(float deltaTime, std::unordered_set<UpdateInterface*>::iterator begin, std::unordered_set<UpdateInterface*>::iterator end)
{
    while (begin != end)
    {
        if ((*begin)->isEnabled())
            (*begin)->Update(deltaTime);
        begin++;
    }
}

void UpdateManager::LateUpdate(float deltaTime)
{
    std::unordered_set<UpdateInterface*>::iterator i = m_objects.begin();
    while (i != m_objects.end())
    {
        // in the case that the obj is destroyed during call
        UpdateInterface* temp = *i;
        i++;
        if (temp->isEnabled())
            temp->LateUpdate(deltaTime);
    }

    ObjectManager::ClearDestroyQueue();
}

void UpdateManager::FixedUpdate()
{
    std::unordered_set<UpdateInterface*>::iterator i = m_objects.begin();
    while (i != m_objects.end())
    {
        // in the case that the obj is destroyed during call
        UpdateInterface* temp = *i;
        i++;
        if (temp->isEnabled())
            temp->FixedUpdate();
    }

    ObjectManager::ClearDestroyQueue();
}

void UpdateManager::Start()
{
    for (UpdateInterface* obj: m_objects)
    {
        obj->Start();
    }
}

size_t UpdateManager::getNumberOfObjects()
{
    return m_objects.size();
}
