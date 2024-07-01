#ifndef OBJECT_HPP
#define OBJECT_HPP

#pragma once

#include <atomic>
#include <list>

#include "Utils/EventHelper.hpp"
#include "Settings.hpp"
#include "Rotation.hpp"
#include "Vector2.hpp"
#include "Transform.hpp"

class ObjectManager;

/// @note anything after this will be private unless specifying scope
#define createDestroy() private: inline virtual void m_destroy() override { delete(this); }

/// @note the pure virtual "destroy" function must only handle the destruction of the derived object
/// @warning no thread safe if not acceded through a pointer or Object::Ptr
class Object
{
public:
    /// @warning the type must have the base class Object
    template <typename T = Object>
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
        
        /// @note same thing as using Object::Ptr<T>::set()
        inline Object::Ptr<T>& operator=(const Object::Ptr<T>& Ptr)
        {
            this->set(Ptr.m_ptr);
            return *this;
        }

        /// @note same thing as using Object::Ptr<T>::set()
        inline Object::Ptr<T>& operator=(T* rawPtr)
        {
            this->set(rawPtr);
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
    /// @note only use this if you do not need to know when the parent is no longer existant
    /// @returns a raw ptr to the parent
    Object* getParentRaw();
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
    /// @brief called when ever the transform of this object is updated
    /// @note only called if the local position is updated
    EventHelper::Event onTransformUpdated;

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
    /// @note rotates around and transforms position
    /// @returns the equivalent local point
    Vector2 getLocalPoint(const Vector2& point) const;
    /// @param vec local point
    /// @note rotates around and transforms position
    /// @return the equivalent global point
    Vector2 getGlobalPoint(const Vector2& point) const;
    /// @param vec local vector
    /// @note only rotates
    /// @return the equivalent global vector
    Vector2 getGlobalVector(const Vector2& vec) const;
    /// @param vec global vector
    /// @note only rotates
    /// @returns the equivalent local vector
    Vector2 getLocalVector(const Vector2& vec) const;
    /// @brief rotates the this object around the given center
    /// @note in local space
    /// @param center the point to rotate around
    void rotateAround(const Vector2& center, Rotation rot);
    void setPosition(const Vector2& position);
    void setPosition(float x, float y);
    void setGlobalPosition(const Vector2& position);
    Vector2 getPosition() const;
    /// @note if no parent returns global position
    /// @returns position according to parent
    Vector2 getGlobalPosition() const;
    void setRotation(Rotation rotation);
    void setGlobalRotation(Rotation rotation);
    Rotation getRotation() const;
    /// @note if no parent returns global rotation
    /// @returns rotation according to parent
    Rotation getGlobalRotation() const;
    void setTransform(const Transform& transform);
    const Transform& getTransform() const;
    void setGlobalTransform(const Transform& transform);
    const Transform getGlobalTransform() const;
    void move(const Vector2& move);
    void move(float x, float y);
    void rotate(Rotation rot);

    /// @brief sets the stored user type for easy comparison later on
    void setUserType(size_t type);
    /// @returns the stored user type
    size_t getUserType() const;

    /// @note this is just the transform if no other derived classes override this
    /// @returns the locally interpolated transform
    virtual Transform getInterpolatedTransform() const;
    /// @returns true if this object is in the destroy queue
    bool isDestroyed() const;

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
    // virtual Vector2 getInterpolatedPosition();

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

    bool m_enabled = true;
    bool m_destroyed = false;
    size_t m_id = 0;
    size_t m_userType = 0;

    Transform m_transform;

    Object* m_parent = nullptr;
    std::list<Object*> m_children;

    static size_t m_lastID;

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
