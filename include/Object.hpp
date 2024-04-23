#ifndef OBJECT_H
#define OBJECT_H

#pragma once

#include <atomic>
#include <complex>
#include <list>

#include "Utils/EventHelper.hpp"

#include "box2d/b2_math.h"

class ObjectManager;

/// @brief dont use this unless you know what you are doing
/// @note this anything after this will be public unless specifying scope
#define createDestroy() private: inline virtual void _destroy() override { delete(this); } public:

#define PI b2_pi

/// @note the pure virtual "destroy" function must only handle the destruction of the derived object
class Object
{
public:
    template <typename T = Object, typename std::enable_if_t<std::is_base_of<Object, T>::value>* = nullptr>
    class Ptr
    {
    public:
        inline Ptr(T* obj)
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
                _ptr->_onDestroy.disconnect(_eventID);
            }
            this->removePtr();
        }
        
        inline T* operator->()
        {
            return _ptr;
        }
        
        inline const T* operator->() const
        {
            return _ptr;
        }
        
        inline T* operator*()
        {
            return _ptr;
        }
        
        inline const T* operator*() const
        {
            return _ptr;
        }
        
        inline Object::Ptr<T>& operator=(const Object::Ptr<T>& Ptr)
        {
            this->set(Ptr._ptr);
            return *this;
        }

        inline Object::Ptr<T>& operator=(const T* Ptr)
        {
            this->set(Ptr._ptr);
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
        
        inline bool operator==(T* Ptr) const
        {
            return this->getID() == (Ptr == nullptr ? 0 : Ptr->getID());
        }
        
        inline bool operator!=(T* Ptr) const
        {
            return this->getID() != (Ptr == nullptr ? 0 : Ptr->getID());
        }
        
        inline bool operator<(T* Ptr) const
        {
            return this->getID() < (Ptr == nullptr ? 0 : Ptr->getID());
        }
        
        inline bool operator>(T* Ptr) const
        {
            return this->getID() > (Ptr == nullptr ? 0 : Ptr->getID());
        }
        
        inline T* get()
        {
            return _ptr;
        }
        
        /// @brief if there is no ptr returns nullptr
        /// @returns obj or nullptr if no obj
        inline const T* get() const
        {
            return _ptr;
        }
        
        inline bool isValid() const
        {
            return (_ptr != nullptr);
        }
        
        /// @brief assigns which obj is stored in this ptr
        /// @param rawPtr the new ptr
        inline void set(T* rawPtr)
        {
            if (this->isValid())
            {
                _ptr->_onDestroy.disconnect(_eventID);
            }
            this->removePtr();

            if (rawPtr != nullptr)
            {
                _ptr = rawPtr;    
                _eventID = _ptr->_onDestroy(Object::Ptr<T>::removePtr, this);
            }
        }
        
        /// @brief if this is invalid then returns 0
        /// @returns the id of the base Object class
        inline size_t getID() const
        {
            return (this->_ptr == nullptr ? 0 : this->_ptr->getID());
        }

    protected:
        inline void removePtr()
        {
            _ptr = nullptr;
            _eventID = 0;
        }

    private:
        inline Ptr(const Ptr& ptr) = default;

        T* _ptr = nullptr;
        size_t _eventID = 0;
    };

    Object();
    /// @brief Create an object as a child
    /// @note this will be faster than creating an object and setting it as a child
    Object(Object::Ptr<> parent);
    ~Object();

    void setEnabled(bool enabled = true);
    bool isEnabled() const;

    unsigned long int getID() const;
    Object::Ptr<> getPtr();

    /// @brief if nullptr then no parent
    void setParent(Object* parent);
    /// @brief if invalid then no parent
    Object::Ptr<> getParent();

    /// @note if derived class, use the virtual function
    EventHelper::Event onEnabled;
    /// @note if derived class, use the virtual function
    EventHelper::Event onDisabled;
    /// @note if using this in the object just use deconstructor instead
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
        type* temp = nullptr;
    
        try
        {
            temp = dynamic_cast<type*>(this);    
        }
        catch(const std::exception& e)
        {
            temp = nullptr;
        }

        return temp;
    }

    void destroy();

    /// @param vec global b2Vec2
    /// @returns the equivalent local b2Vec2
    b2Vec2 getLocalVector(const b2Vec2& vec) const;
    /// @param vec local b2Vec2
    /// @return the equivalent global b2Vec2
    b2Vec2 getGlobalVector(const b2Vec2& vec) const;
    /// @brief rotates the given b2Vec2 around this object
    /// @param vec global vector
    /// @param rot rotation in RAD
    /// @returns the rotated vector
    void rotateAround(const b2Vec2& center, const float& rot);
    /// @brief rotates the given b2Vec2 around the given center
    /// @param vec the point to rotate
    /// @param center the point to rotate around
    /// @param rot rotation in RAD
    /// @returns the rotated vector
    static b2Vec2 rotateAround(const b2Vec2& vec, const b2Vec2& center, const float& rot);
    virtual void setPosition(const b2Vec2& position);
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
    virtual void setRotation(const float& rotation);
    /// @brief if this is a child then the rotation will be set according to the parent
    /// @note if "canSetTransform" is false then this does nothing
    /// @note if this is not a child then rotation is set according to global
    /// @param rotation according to the parent rotation
    void setLocalRotation(const float& rotation);
    /// @returns rotation in radians
    float getRotation() const;
    /// @returns the rotation in terms of b2Rot
    b2Rot getRotation_b2() const;
    /// @note if no parent returns global rotation
    /// @returns rotation according to parent
    float getLocalRotation() const;
    virtual void setTransform(const b2Transform& transform);
    b2Transform getTransform() const;
    /// @brief if there is another class that overrides the transform you should 1. not override it 2. not set transforms
    /// @note the transform includes the position and rotation functions
    /// @returns true if you can set transform, position, or rotation
    virtual bool canSetTransform() const;
    void move(const b2Vec2& move);
    /// @param rot in radians
    void rotate(const float& rot);

protected:
    inline virtual void OnEnable() {};
    inline virtual void OnDisable() {};
    /// @warning do NOT disconnect all EVER
    EventHelper::Event _onEnabled;
    /// @warning do NOT disconnect all EVER
    EventHelper::Event _onDisabled;
    /// @warning do NOT disconnect all EVER
    EventHelper::Event _onDestroy;
    /// @warning do NOT disconnect all EVER
    EventHelper::Event _onParentSet;
    /// @warning do NOT disconnect all EVER
    EventHelper::Event _onParentRemoved;
    /// @brief adds this object to the destroy queue
    /// @note same as using destroy
    void _addToDestroyQueue();

private:
    /// @brief this should actually delete the object
    virtual void _destroy() = 0;
    /// @warning only use this if you know what you are doing
    Object(unsigned long long id);
    /// @warning only use this if you know what you are doing
    void setID(unsigned long long id);

    void _addChild(Object* object);
    void _removeChild(Object* object);

    std::atomic_bool _enabled = true;
    size_t _id = 0;

    // TODO make a renderer that uses this transform
    b2Transform _transform = b2Transform(b2Vec2(0,0), b2Rot(0));

    Object* _parent = nullptr;
    std::list<Object*> _children;

    static std::list<Object::Ptr<>> _destroyQueue;
    static std::atomic_ullong _lastID;

    friend ObjectManager;
};

class _objectComp
{
public:
    bool operator()(const Object* lhs, const Object* rhs) const;
};

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
