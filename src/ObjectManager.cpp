#include "ObjectManager.hpp"

ObjectManager::_objectCompClass ObjectManager::m_compClass(0);
std::unordered_set<Object*> ObjectManager::m_objects;

std::list<Object*> ObjectManager::m_destroyQueue0;
std::list<Object*> ObjectManager::m_destroyQueue1;
bool ObjectManager::m_nextQueue = false;


//* Comparison classes

ObjectManager::_objectCompClass::_objectCompClass(uint64_t id) : Object(id) {}
void ObjectManager::_objectCompClass::setID(uint64_t id) { Object::m_setID(id); }

// -------------------

Object::Ptr<> ObjectManager::getObject(uint64_t id)
{
    m_compClass.setID(id);
    std::unordered_set<Object*>::iterator obj = m_objects.find((Object*)&m_compClass);
    m_compClass.setID(0);

    return Object::Ptr(obj == m_objects.end() ? nullptr : *obj);
}

size_t ObjectManager::getNumberOfObjects()
{
    return m_objects.size();
}

void ObjectManager::ClearDestroyQueue()
{
    std::list<Object*>& list = m_nextQueue ? m_destroyQueue1 : m_destroyQueue0;
    for (std::list<Object*>::iterator obj = list.begin(); obj != list.end(); obj++)
    {
        delete(*obj);
    }
    list.clear();

    m_nextQueue = !m_nextQueue;
}

Object::Ptr<> ObjectManager::addObject(Object* object)
{
    m_objects.insert({object});
    return object->getPtr();
}

void ObjectManager::removeObject(Object* object)
{
    m_objects.erase(object);
}

void ObjectManager::addToDestroyQueue(Object* object)
{
    std::list<Object*>& list = m_nextQueue ? m_destroyQueue0 : m_destroyQueue1;
    list.emplace_back(object);
}

void ObjectManager::destroyAllObjects()
{
    for (auto object: m_objects)
    {
        if (object->getParent() == nullptr) // only delete if a parent otherwise results in error
            object->destroy();
    }
    ClearDestroyQueue(); 
    ClearDestroyQueue(); // clearing twice for both queues
    Object::m_lastID = 1;
}
