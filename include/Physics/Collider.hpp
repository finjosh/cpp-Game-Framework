#ifndef COLLIDER_H
#define COLLIDER_H

#pragma once

#include "box2d/box2d.h"

#include "Physics/WorldHandler.hpp"
#include "Object.hpp"

class CollisionManager;

class Collider;

class CollisionData
{
public:
    CollisionData(Collider* collider, b2Fixture* thisFixture, b2Fixture* otherFixture);

    /// @returns the other objects collider
    Collider* getCollider();
    /// @returns the fixture from this object that collided
    b2Fixture* getFixtureA();
    /// @returns the fixture from the other object that collided
    b2Fixture* getFixtureB();

private:
    Collider* _collider;
    b2Fixture* _thisFixture;
    b2Fixture* _otherFixture;
};

// TODO make an onColliding function for while a body is actively colliding with this 
// TODO completely hide all body functions unless they are needed
// TODO refactor for easier to use code
// TODO find a way to deal with box2d on end collision and this being destroyed before that is called (dont destroy objects that have colliders till the end contact is called?)
/// @note do not use body user data as that stores this collider
class Collider : public virtual Object
{
public:
    Collider();
    ~Collider();

    /// @brief sets only physics enabled or disabled, ignores the object state
    /// @note if disabled object state does not matter if enabled physics will follow object state
    void setPhysicsEnabled(const bool& enabled = true);
    /// @returns true if physics are enabled
    bool isPhysicsEnabled() const;

    /// @brief creates a fixture with the given fixture def
    /// @returns the pointer to the new fixture
    b2Fixture* createFixture(const b2FixtureDef& fixture);
    /// @brief creates a fixture with the given shape and density
    /// @param density the density of the shape
    /// @param friction the friction of the shape usually between [0,1]
    /// @param restitution the restitution (elasticity) usually in the range [0,1]
    /// @param restitutionThreshold Restitution velocity threshold, usually in m/s. Collisions above this speed have restitution applied (will bounce)
    /// @param filter contact filtering data
    /// @returns the pointer to the new fixture
    b2Fixture* createFixture(const b2Shape& shape, const float& density = 1.f, const float& friction = 0.1, 
                            const float& restitution = 0.f, const float& restitutionThreshold = 0.f, const b2Filter& filter = {});
    /// @brief creates a fixture with the given shape and density
    /// @param density the density of the shape
    /// @param isSensor a sensor shape collects contact information but never generates a collision response
    /// @param filter contact filtering data
    /// @returns the pointer to the new fixture
    b2Fixture* createFixtureSensor(const b2Shape& shape, const float& density = 1.f, const b2Filter& filter = {});

    /// @warning DO NOT DESTROY THE BODY
    /// @warning DO NOT USE USER DATA
    /// @note if you want to destroy the physics body call destroy on this object
    b2Body* operator->();
    const b2Body* operator->() const;
    /// @warning DO NOT DESTROY THE BODY
    /// @warning DO NOT USE USER DATA
    /// @note if you want to destroy the physics body call destroy on this object
    b2Body* operator*();
    const b2Body* operator*() const;
    /// @warning DO NOT DESTROY THE BODY
    /// @warning DO NOT USE USER DATA
    /// @note if you want to destroy the physics body call destroy on this object
    b2Body* getBody();
    const b2Body* getBody() const;

    /// @brief called when a contact begins
    /// @param collisionData the collision data
    inline virtual void BeginContact(CollisionData collisionData) {};
    /// @brief called when a contact ends
    /// @param collisionData the collision data
    inline virtual void EndContact(CollisionData collisionData) {};
    /// @brief This can be called multiple times in one frame (called before the collision is handled)
    /// @note to get the collider get the userdata from the body and cast to Collider
    /// @warning do not destroy the body during this callback
    /// @param contact the contact data
    /// @param oldManifold the old manifold data
    inline virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {};
    /// @brief This can be called multiple times in one frame (called after the collision is handled)
    /// @note to get the collider get the userdata from the body and cast to Collider
    /// @warning do not destroy the body during this callback
    /// @param contact the contact data
    /// @param impulse the impulse data
    inline virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {};

    bool canSetTransform() const override;

    /// @brief dont use this during a box2d callback (PreSolve and PostSolve)
    void setPosition(const b2Vec2& position) override;
    /// @brief dont use this during a box2d callback (PreSolve and PostSolve)
    void setRotation(const float& rotation) override;
    /// @brief dont use this during a box2d callback (PreSolve and PostSolve)
    void setTransform(const b2Transform& transform) override;

protected:
    /// @brief creates a body in the world with the default body def parameters
    void initCollider(const float& x = 0.f, const float& y = 0.f, const float& rot = 0.f);
    /// @brief create a body in the world with the default body def parameters
    /// @param pos the position to init the body at
    void initCollider(const b2Vec2& pos, const b2Rot& rot = b2Rot(0));
    /// @brief create a body in the world with the given body def
    void initCollider(const b2BodyDef& bodyDef);

    /// @brief only use this if you know what you are doing
    void _update();

    friend CollisionManager;

private:
    /// @brief updates the body state (enabled or not)
    void updatePhysicsState();

    b2Body* _body;
    /// @brief if true follows object else physics are disabled no matter object state
    bool _enabled = true;
};

namespace std {
    template <>
    struct hash<Collider> {
        inline size_t operator()(const Collider& obj) const noexcept
        {
            return hash<size_t>{}(obj.getID());
        }
    };
    template <>
    struct hash<Collider*> {
        inline size_t operator()(const Collider* obj) const noexcept
        {
            if (obj == nullptr)
                return 0;
            return hash<size_t>{}(obj->getID());
        }
    };
    template <>
    struct equal_to<Collider> {
        inline bool operator()(const Collider& obj, const Collider& obj2) const noexcept
        {
            return obj.getID() == obj2.getID();
        }
    };
    template <>
    struct equal_to<Collider*> {
        inline bool operator()(const Collider* obj, const Collider* obj2) const noexcept
        {
            return obj->getID() == obj2->getID();
        }
    };
}

#endif
 