#ifndef COLLIDER_H
#define COLLIDER_H

#pragma once

#include <set>

#include "box2d/box2d.h"

#include "Physics/WorldHandler.hpp"
#include "Physics/Fixture.hpp"
#include "Object.hpp"

class CollisionManager; // defined early to friend in Collider

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

    bool operator < (const CollisionData& data) const;
    bool operator > (const CollisionData& data) const;
    bool operator == (const CollisionData& data) const;
    bool operator != (const CollisionData& data) const;

private:
    Collider* m_collider;
    b2Fixture* m_thisFixture;
    b2Fixture* m_otherFixture;
};

// TODO completely hide all body functions unless they are needed
// TODO refactor for easier to use code
// TODO hide more box2d code so that its harder to screw up
// TODO implement contact filtering
/// @note do not use body user data as that stores this collider !(REMOVE THIS WHEN DONE REFACTOR)!
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
    /// @returns the pointer to the new fixture or nullptr if no body is stored
    b2Fixture* createFixture(const b2Shape& shape, const float& density = 1.f, const float& friction = 0.1, 
                            const float& restitution = 0.f, const float& restitutionThreshold = 0.f, const b2Filter& filter = {});
    /// @brief creates a fixture with the given shape and density
    /// @param density the density of the shape
    /// @param isSensor a sensor shape collects contact information but never generates a collision response
    /// @param filter contact filtering data
    /// @returns the pointer to the new fixture or nullptr if no body is stored
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
    /// @warning do not destroy or edit the body during this callback
    /// @param contact the contact data
    /// @param oldManifold the old manifold data
    inline virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {};
    /// @brief This can be called multiple times in one frame (called after the collision is handled)
    /// @note to get the collider get the userdata from the body and cast to Collider
    /// @warning do not destroy or edit the body during this callback
    /// @param contact the contact data
    /// @param impulse the impulse data
    inline virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {};
    /// @brief called every frame until the two objects are no longer colliding
    /// @note this will also be called on start of contact
    inline virtual void OnColliding(CollisionData collisionData) {};

    void setAwake(const bool& awake = true);

protected:

private:
    friend CollisionManager;
    friend Fixture;
    /// @brief removes the current body from physics
    void m_destroyBody();
    /// @brief updates the object transform to this colliders body transform
    void m_update();
    /// @brief updates the body state (enabled or not)
    void m_updatePhysicsState();
    /// @brief updates the body transform to match the object transform
    void m_updateTransform();

    b2Body* m_body = nullptr;
    /// @brief if true follows object else physics are disabled no matter object state
    bool m_enabled = true;
    /// @note stores all the current contacts that are ongoing
    std::set<CollisionData> m_currentCollisions;
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
 