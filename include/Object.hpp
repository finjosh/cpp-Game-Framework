#ifndef OBJECT_HPP
#define OBJECT_HPP

#pragma once

#include <atomic>
#include <list>

#include "Utils/EventHelper.hpp"
#include "EngineSettings.hpp"
#include "Rotation.hpp"
#include "Vector2.hpp"
#include "Transform.hpp"

class ObjectManager;
class _objectCompare;

class Object
{
public:
    /// @note this will compare to other objects/Ptrs until the stored object is completely destroyed
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
            set(nullptr);
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
        /// @note the ptr returned could not be nullptr but still be invalid as it was added to the destroy queue
        inline T* get()
        {
            return m_ptr;
        }
        
        /// @brief if there is no ptr returns nullptr
        /// @note the ptr returned could not be nullptr but still be invalid as it was added to the destroy queue
        inline const T* get() const
        {
            return m_ptr;
        }

        /// @note if the ptr is invalid returns nullptr even if the object is not destroyed yet
        /// @returns the ptr to the base object
        inline Object* getObj() const
        {
            if (isValid())
                return nullptr;
            return static_cast<Object*>(m_ptr);
        }

        /// @returns false if the object originally stored object was added to destroy queue
        inline bool isValid() const
        {
            return (m_ptr != nullptr) && !m_ptr->m_destroyQueued;
        }
        
        /// @brief assigns which obj is stored in this ptr
        /// @param rawPtr the new ptr
        inline void set(T* rawPtr)
        {
            if (this->m_ptr != nullptr)
            {
                m_ptr->m_onDestroy.disconnect(m_eventID);
            }
            this->removePtr();

            if (rawPtr != nullptr)
            {
                m_ptr = rawPtr;    
                m_eventID = m_ptr->m_onDestroy.connect(Object::Ptr<T>::removePtr, this);
            }
        }

        /// @note this will return 0 when the stored object is destroyed
        /// @returns the id of the base Object class
        inline uint64_t getID() const
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
        uint64_t m_eventID = 0;
    };

    Object();
    virtual ~Object();

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
    /// @note if no child of type is found or in destroy queue returns nullptr
    /// @returns the first child of the wanted type
    template <typename T>
    inline T* tryGetChildOf()
    {
        for (auto child: m_children)
        {
            auto rtn = child->cast<T>();
            if (rtn != nullptr && !rtn->isDestroyQueued())
                return rtn;
        }
        return nullptr;
    }

    /// @note if derived class, use the virtual function
    EventHelper::Event onEnabled;
    /// @note if derived class, use the virtual function
    EventHelper::Event onDisabled;
    /// @brief this is called when the destruction of this object is queued
    EventHelper::Event onDestroyQueued;
    /// @brief this is called right before the destruction of this object
    /// @note the object will be destroyed after this is called
    EventHelper::Event onDestroy;
    /// @note this is called when the parent is set to something other than the current parent
    EventHelper::Event onParentSet;
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

    /// @brief add this object to the destroy queue
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
    void setUserType(uint64_t type);
    /// @returns the stored user type
    uint64_t getUserType() const;

    /// @note this is just the transform if no other derived classes override this
    /// @returns the locally interpolated transform
    virtual Transform getInterpolatedTransform() const;
    /// @returns true if this object is in the destroy queue
    bool isDestroyQueued() const;

protected:
    inline virtual void OnEnable() {};
    inline virtual void OnDisable() {};
    /// @warning do NOT disconnect all EVER
    EventHelper::Event m_onEnabled;
    /// @warning do NOT disconnect all EVER
    EventHelper::Event m_onDisabled;
    /// @brief this is called when the destruction of this object is queued
    /// @note the object should appear as if it is destroyed
    EventHelper::Event m_onDestroyQueued;
    /// @brief this is called right before the destruction of this object
    /// @note the object will be destroyed after this is called
    EventHelper::Event m_onDestroy;
    /// @warning do NOT disconnect all EVER
    EventHelper::Event m_onParentSet;
    /// @brief called when ever the transform of this object is updated
    /// @warning do NOT disconnect all EVER
    EventHelper::Event m_onTransformUpdated;

    /// @warning only use this if you know what you are doing
    Object(uint64_t id);
    /// @warning only use this if you know what you are doing
    void m_setID(uint64_t id);
private:
    /// @brief invokes the destroy events, sets enabled false, and calls on all children
    void m_invokeDestroyEvents();

    void m_addChild(Object* object);
    void m_removeChild(Object* object);

    bool m_enabled = true;
    bool m_destroyQueued = false;
    uint64_t m_id = 0;
    uint64_t m_userType = 0;

    Transform m_transform;

    Object* m_parent = nullptr;
    std::list<Object*> m_children;

    static uint64_t m_lastID;

    friend ObjectManager;
    friend _objectCompare;
};

/// @brief this should only be used to compare objects based on ID when stored with raw ptrs
/// @note this object is not handled by object manager
/// @note you must set the ID to the wanted ID before trying to compare
class _objectCompare : public Object
{
public:
    inline _objectCompare() : Object(0) {}
    /// @brief sets the object ID to the wanted ID
    /// @note remember to set this back to 0 once done
    inline void setID(uint64_t id)
    {
        Object::m_id = id;
    }
};

namespace std {
    template <>
    struct hash<Object> {
        inline uint64_t operator()(const Object& obj) const noexcept
        {
            return hash<uint64_t>{}(obj.getID());
        }
    };
    template <>
    struct hash<Object*> {
        inline uint64_t operator()(const Object* obj) const noexcept
        {
            if (obj == nullptr)
                return 0;
            return hash<uint64_t>{}(obj->getID());
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
