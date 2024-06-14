#ifndef OBJECT_H
#define OBJECT_H

#pragma once

#include <atomic>
#include <complex>
#include <list>

#include "Utils/EventHelper.hpp"
#include "Settings.hpp"
#include "Vector2.hpp"

#include "box2d/b2_math.h"

class ObjectManager;

/// @note anything after this will be private unless specifying scope
#define createDestroy() private: inline virtual void m_destroy() override { delete(this); }

#define PI b2_pi

/// @note the pure virtual "destroy" function must only handle the destruction of the derived object
/// @warning no thread safe if not acceded through a pointer or Object::Ptr
class Object
{
public:
    template <typename T = Object, typename std::enable_if_t<std::is_base_of<Object, T>::value>* = nullptr>
    class Ptr
    {
    public:
        inline Ptr(T* obj = nullptr)
        {
            this->set(obj);
        }

        inline Ptr(Ptr& ptr) 
        {
            this->set(ptr.get());
        }

        inline ~Ptr()
        {
            if (this->isValid())
            {
                m_ptr->m_onDestroy.disconnect(m_eventID);
            }
            this->removePtr();
        }
        
        inline T* operator->()
        {
            return m_ptr;
        }
        
        inline const T* operator->() const
        {
            return m_ptr;
        }
        
        inline T* operator*()
        {
            return m_ptr;
        }
        
        inline const T* operator*() const
        {
            return m_ptr;
        }
        
        inline Object::Ptr<T>& operator=(const Object::Ptr<T>& Ptr)
        {
            this->set(Ptr.m_ptr);
            return *this;
        }

        inline Object::Ptr<T>& operator=(const T* Ptr)
        {
            this->set(Ptr.m_ptr);
            return *this;
        }
        
        /// @brief returns the value of isValid
        inline explicit operator bool() const
        {
            return this->isValid();
        }
        
        inline bool operator==(const Object::Ptr<T>& Ptr) const
        {
            return this->getID() == Ptr.getID();
        }
        
        inline bool operator!=(const Object::Ptr<T>& Ptr) const
        {
            return this->getID() != Ptr.getID();
        }
        
        inline bool operator<(const Object::Ptr<T>& Ptr) const
        {
            return this->getID() < Ptr.getID();
        }
        
        inline bool operator>(const Object::Ptr<T>& Ptr) const
        {
            return this->getID() > Ptr.getID();
        }
        
        inline bool operator==(const T* Ptr) const
        {
            return this->getID() == (Ptr == nullptr ? 0 : Ptr->getID());
        }
        
        inline bool operator!=(const T* Ptr) const
        {
            return this->getID() != (Ptr == nullptr ? 0 : Ptr->getID());
        }
        
        inline bool operator<(const T* Ptr) const
        {
            return this->getID() < (Ptr == nullptr ? 0 : Ptr->getID());
        }
        
        inline bool operator>(const T* Ptr) const
        {
            return this->getID() > (Ptr == nullptr ? 0 : Ptr->getID());
        }
        
        /// @brief if there is no ptr returns nullptr
        inline T* get()
        {
            return m_ptr;
        }
        
        /// @brief if there is no ptr returns nullptr
        inline const T* get() const
        {
            return m_ptr;
        }

        /// @note if there is no ptr returns nullptr
        /// @returns the ptr to the base object
        inline Object* getObj() const
        {
            return static_cast<Object*>(m_ptr);
        }

        inline bool isValid() const
        {
            return (m_ptr != nullptr);
        }
        
        /// @brief assigns which obj is stored in this ptr
        /// @param rawPtr the new ptr
        inline void set(T* rawPtr)
        {
            if (this->isValid())
            {
                m_ptr->m_onDestroy.disconnect(m_eventID);
            }
            this->removePtr();

            if (rawPtr != nullptr)
            {
                m_ptr = rawPtr;    
                m_eventID = m_ptr->m_onDestroy(Object::Ptr<T>::removePtr, this);
            }
        }

        /// @brief if this is invalid then returns 0
        /// @returns the id of the base Object class
        inline size_t getID() const
        {
            return (this->m_ptr == nullptr ? 0 : this->m_ptr->getID());
        }

    protected:
        inline void removePtr()
        {
            m_ptr = nullptr;
            m_eventID = 0;
        }

    private:
        inline Ptr(const Ptr& ptr) = default;

        T* m_ptr = nullptr;
        size_t m_eventID = 0;
    };

    Object();
    ~Object();

    void setEnabled(bool enabled = true); 
    bool isEnabled() const;

    unsigned long int getID() const;
    /// @note if you want a pointer to a derived type just use the Object::Ptr<T> constructor
    /// @returns the object ptr
    Object::Ptr<> getPtr();

    /// @note set parent to nullptr if you dont want a parent
    /// @param parent the wanted parent
    void setParent(Object::Ptr<>& parent);
    /// @note set parent to nullptr if you dont want a parent
    /// @param parent the wanted parent
    void setParent(Object* parent);
    /// @returns an invalid ptr if no parent
    Object::Ptr<> getParent();
    /// @brief adds the given object as a child to this one
    void addChild(Object::Ptr<>& child);
    /// @brief adds the given object as a child to this one
    void addChild(Object* child);
    /// @brief tries to convert every child to the given type until found
    /// @tparam T the type that you want the child to be
    /// @returns the first child of the wanted type
    template <typename T>
    inline Object::Ptr<T> tryGetChildOf()
    {
        for (auto child: m_children)
        {
            if (auto rtn = child->cast<T>())
                return Object::Ptr<T>(rtn);
        }
    }

    /// @note if derived class, use the virtual function
    EventHelper::Event onEnabled;
    /// @note if derived class, use the virtual function
    EventHelper::Event onDisabled;
    /// @note this is called one frame before the object is actually destroyed
    /// @note you should NOT access any thing about the object through this event
    EventHelper::Event onDestroy;
    /// @note this is called when the parent is set to anything but nullptr
    EventHelper::Event onParentSet;
    /// @note this is called when the parent is set to nullptr
    /// @note not called when this object is destroyed
    EventHelper::Event onParentRemoved;

    /// @brief tries to cast this object to a given type
    /// @returns nullptr if cast was unsuccessful  
    template<typename type>
    type* cast()
    {
        return dynamic_cast<type*>(this);
    }

    /// @brief tries to cast this object to a given type
    /// @returns nullptr if cast was unsuccessful  
    template<typename type>
    const type* cast() const
    {
        return dynamic_cast<const type*>(this);
    }

    void destroy();

    /// @param vec global point
    /// @returns the equivalent local point
    b2Vec2 getLocalPoint(const b2Vec2& point) const;
    /// @param vec local point
    /// @return the equivalent global point
    b2Vec2 getGlobalPoint(const b2Vec2& point) const;
    /// @param vec local vector
    /// @note only rotates
    /// @return the equivalent global vector
    b2Vec2 getGlobalVector(const b2Vec2& vec) const;
    /// @param vec global vector
    /// @note only rotates
    /// @returns the equivalent local vector
    b2Vec2 getLocalVector(const b2Vec2& vec) const;
    /// @brief rotates the this object around the given center
    /// @param center the point to rotate around
    /// @param rot rotation in radians
    void rotateAround(const b2Vec2& center, float rot);
    /// @brief rotates the given b2Vec2 around the given center
    /// @param pos the point that will be rotated
    /// @param center the point to rotate around
    /// @param rot rotation in radians
    /// @returns the rotated vector
    b2Vec2 rotateAround(const b2Vec2& pos, const b2Vec2& center, float rot);
    void setPosition(const b2Vec2& position);
    void setPosition(float x, float y);
    /// @brief if this is a child then the position will be set according to the parent
    /// @note if "canSetTransform" is false then this does nothing
    /// @note if this is not a child then position is set according to global
    /// @param position according to the parent position
    void setLocalPosition(const b2Vec2& position);
    b2Vec2 getPosition() const;
    /// @note if no parent returns global position
    /// @returns position according to parent
    b2Vec2 getLocalPosition() const;
    /// @param rotation in radians
    void setRotation(float rotation);
    /// @brief if this is a child then the rotation will be set according to the parent
    /// @note if "canSetTransform" is false then this does nothing
    /// @note if this is not a child then rotation is set according to global
    /// @param rotation according to the parent rotation
    void setLocalRotation(float rotation);
    /// @returns rotation in radians
    float getRotation() const;
    /// @returns the rotation in terms of b2Rot
    b2Rot getRotation_b2() const;
    /// @note if no parent returns global rotation
    /// @returns rotation according to parent
    float getLocalRotation() const;
    void setTransform(const b2Transform& transform);
    b2Transform getTransform() const;
    void move(const b2Vec2& move);
    void move(float x, float y);
    /// @param rot in radians
    void rotate(float rot);

protected:
    inline virtual void OnEnable() {};
    inline virtual void OnDisable() {};
    /// @warning do NOT disconnect all EVER
    EventHelper::Event m_onEnabled;
    /// @warning do NOT disconnect all EVER
    EventHelper::Event m_onDisabled;
    /// @brief called when the object is added to the destroy queue (should act as if it is already destroyed)
    /// @note this is where the object should show itself as being destroyed (deconstructor should be used to finalize destruction)
    /// @warning do NOT disconnect all EVER
    EventHelper::Event m_onDestroy;
    /// @warning do NOT disconnect all EVER
    EventHelper::Event m_onParentSet;
    /// @warning do NOT disconnect all EVER
    EventHelper::Event m_onParentRemoved;
    /// @brief called when ever the transform of this object is updated
    /// @warning do NOT disconnect all EVER
    EventHelper::Event m_onTransformUpdated;
    /// @brief adds this object to the destroy queue
    /// @note same as using destroy
    void m_addToDestroyQueue();

private:
    /// @brief this should actually delete the object
    virtual void m_destroy() = 0;
    /// @brief invokes the destroy events, sets enabled false, and calls on all children
    void m_invokeDestroyEvents();
    /// @warning only use this if you know what you are doing
    Object(size_t id);
    /// @warning only use this if you know what you are doing
    void m_setID(size_t id);

    void m_addChild(Object* object);
    void m_removeChild(Object* object);

    std::atomic_bool m_enabled = true;
    size_t m_id = 0;

    b2Transform m_transform = b2Transform(b2Vec2(0,0), b2Rot(0));

    Object* m_parent = nullptr;
    std::list<Object*> m_children;

    static std::atomic_ullong m_lastID;

    friend ObjectManager;
};

// class _objectComp
// {
// public:
//     bool operator()(const Object* lhs, const Object* rhs) const;
// };

namespace std {
    template <>
    struct hash<Object> {
        inline size_t operator()(const Object& obj) const noexcept
        {
            return hash<size_t>{}(obj.getID());
        }
    };
    template <>
    struct hash<Object*> {
        inline size_t operator()(const Object* obj) const noexcept
        {
            if (obj == nullptr)
                return 0;
            return hash<size_t>{}(obj->getID());
        }
    };
    template <>
    struct equal_to<Object> {
        inline bool operator()(const Object& obj, const Object& obj2) const noexcept
        {
            return obj.getID() == obj2.getID();
        }
    };
    template <>
    struct equal_to<Object*> {
        inline bool operator()(const Object* obj, const Object* obj2) const noexcept
        {
            return obj->getID() == obj2->getID();
        }
    };
}

#endif
