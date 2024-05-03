#include "ObjectManager.hpp"

ObjectManager::_objectCompClass ObjectManager::_compClass(0);
std::unordered_set<Object*> ObjectManager::_objects;

//* Comparison classes

ObjectManager::_objectCompClass::_objectCompClass(size_t id) : Object(id) {}
void ObjectManager::_objectCompClass::setID(size_t id) { Object::setID(id); }

// -------------------

Object::Ptr<> ObjectManager::getObject(size_t id)
{
    _compClass.setID(id);
    std::unordered_set<Object*>::iterator obj = _objects.find((Object*)&_compClass);
    _compClass.setID(0);

    return Object::Ptr(obj == _objects.end() ? nullptr : *obj);
}

size_t ObjectManager::getNumberOfObjects()
{
    return _objects.size();
}

void ObjectManager::ClearDestroyQueue()
{
    for (std::list<Object::Ptr<>>::iterator obj = Object::_destroyQueue.begin(); obj != Object::_destroyQueue.end(); obj++)
    {
        if (*obj)
        {
            (*obj)->_destroy();
        }
    }
    Object::_destroyQueue.clear();
}

Object::Ptr<> ObjectManager::addObject(Object* object)
{
    _objects.insert({object});
    return object->getPtr();
}

void ObjectManager::removeObject(Object* object)
{
    _objects.erase(object);
}

void ObjectManager::destroyAllObjects()
{
    auto i = _objects.begin();
    while (i != _objects.end())
    {
        auto temp = i;
        i++;
        (*temp)->_destroy();
    }
}
