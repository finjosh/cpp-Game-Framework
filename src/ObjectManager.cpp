#include "ObjectManager.hpp"

ObjectManager::_objectCompClass ObjectManager::m_compClass(0);
std::unordered_set<Object*> ObjectManager::m_objects;

//* Comparison classes

ObjectManager::_objectCompClass::_objectCompClass(size_t id) : Object(id) {}
void ObjectManager::_objectCompClass::setID(size_t id) { Object::setID(id); }

// -------------------

Object::Ptr<> ObjectManager::getObject(size_t id)
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
    for (std::list<Object::Ptr<>>::iterator obj = Object::m_destroyQueue.begin(); obj != Object::m_destroyQueue.end(); obj++)
    {
        if (*obj)
        {
            (*obj)->_destroy();
        }
    }
    Object::m_destroyQueue.clear();
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

void ObjectManager::destroyAllObjects()
{
    auto i = m_objects.begin();
    while (i != m_objects.end())
    {
        auto temp = i;
        i++;
        (*temp)->_destroy();
    }
    Object::m_lastID = 1;
}
