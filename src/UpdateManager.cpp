#include "UpdateManager.hpp"
#include "ObjectManager.hpp"
#include <thread>

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
    // - give each thread a range then allow them to continue if done early (would be difficult to implement properly)
// - give each object a atomic<bool> for if it was updated this frame (would have to reset the var after each update)
void UpdateManager::Update(float deltaTime, int numThreads)
{
    m_iterator = m_objects.begin();
    
    std::list<std::thread> threads;

    for (int i = 0; i < numThreads; i++)
    {
        threads.emplace_back(&_update, deltaTime);
    }

    for (auto& thread: threads)
    {
        thread.join();
    }

    threads.clear();

    // _update(deltaTime);

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
