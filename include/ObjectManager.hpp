#ifndef OBJECT_MANAGER_HPP
#define OBJECT_MANAGER_HPP

#pragma once

#include <unordered_set>

#include "Object.hpp"

class ObjectManager
{
public:
    /// @returns nullptr if the object does not exist 
    static Object::Ptr<> getObject(size_t id);

    static size_t getNumberOfObjects();

    /// @brief destroys all the objects in the queue
    /// @note only use this if you know what you are doing
    static void ClearDestroyQueue();

    /// @note instantly destroys all objects
    /// @note resets all object IDs
    static void destroyAllObjects();

protected:
    static Object::Ptr<> addObject(Object* object);
    static void removeObject(Object* object);

    static void addToDestroyQueue(Object* object);

    friend Object;

private:
    inline ObjectManager() = default;

    /// @note used as Object can not be made on its own due to its pure virtual destroy function
    class _objectCompClass : public Object
    {
    public:
        _objectCompClass(size_t id);
        void setID(size_t id);
        createDestroy();
    };

    /// @brief if the comp class is deleted the object will be removed with the id
    static _objectCompClass m_compClass;
    /// @brief all objects
    static std::unordered_set<Object*> m_objects;

    static std::list<Object*> m_destroyQueue0;
    static std::list<Object*> m_destroyQueue1;
    static bool m_nextQueue;
};

#endif
