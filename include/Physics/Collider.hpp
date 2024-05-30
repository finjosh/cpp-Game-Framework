#ifndef COLLIDER_H
#define COLLIDER_H

#pragma once

#include <set>

#include "box2d/box2d.h"

#include "Physics/WorldHandler.hpp"
#include "Physics/Fixture.hpp"
#include "Object.hpp"
#include "Settings.hpp"

class CollisionManager; // defined early to friend in Collider

class Collider;

class ContactData
{
public:
    ContactData(Collider* collider, b2Fixture* thisFixture, b2Fixture* otherFixture, b2Contact* contactData);

    /// @returns the other objects collider
    Collider* getCollider();
    /// @returns the fixture from this object that collided
    Fixture getThisFixture();
    Fixture getThisFixture() const;
    /// @returns the fixture from the other object that collided
    Fixture getOtherFixture();
    Fixture getOtherFixture() const;
    
    //!TEMP
    inline b2Contact* getContactData() { return m_contactData; }

    bool operator < (const ContactData& data) const;
    bool operator > (const ContactData& data) const;
    bool operator == (const ContactData& data) const;
    bool operator != (const ContactData& data) const;

private:
    Collider *const m_collider;
    b2Fixture *const m_thisFixture;
    b2Fixture *const m_otherFixture;
    b2Contact *const m_contactData;
};

typedef ContactData CollisionData;

// TODO implement contact filtering
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

    /// @param density the density of the shape
    /// @param friction the friction of the shape usually between [0,1]
    /// @param restitution the restitution (elasticity) usually in the range [0,1]
    /// @param restitutionThreshold Restitution velocity threshold, usually in m/s. Collisions above this speed have restitution applied (will bounce)
    /// @param filter contact filtering data
    /// @returns the new fixture
    Fixture createFixture(const b2Shape& shape, const float& friction = 0.1f, const float& restitution = 0.f, const float& restitutionThreshold = 0.f, 
                          const float& density = 1.f, const b2Filter& filter = {});
    /// @brief creates a sensor fixture with the given shape and density
    /// @param density the density of the shape
    /// @param filter contact filtering data
    /// @returns the new fixture
    Fixture createFixtureSensor(const b2Shape& shape, const float& density = 1.f, const b2Filter& filter = {});
	/// @param fixture the fixture to be removed.
	void DestroyFixture(const Fixture& fixture);

    /// @brief called when a contact begins
    /// @note these are called for each fixture
    /// @param ContactData the collision data
    inline virtual void BeginContact(ContactData ContactData) {};
    /// @brief called when a contact ends
    /// @note these are called for each fixture
    /// @param ContactData the collision data
    inline virtual void EndContact(ContactData ContactData) {};
    /// @brief This can be called multiple times in one frame (called before the collision is handled)
    /// @note to get the collider get the userdata from the body and cast to Collider
    /// @warning do not destroy or edit the collider during this callback
    /// @param contact the contact data
    /// @param oldManifold the old manifold data
    inline virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {};
    /// @brief This can be called multiple times in one frame (called after the collision is handled)
    /// @note to get the collider get the userdata from the body and cast to Collider
    /// @warning do not destroy or edit the collider during this callback
    /// @param contact the contact data
    /// @param impulse the impulse data
    inline virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {};
    /// @brief called every frame until the two objects are no longer colliding
    /// @note this will also be called on start of contact
    /// @note these are called for each fixture
    inline virtual void OnColliding(ContactData ContactData) {};

    void setAwake(const bool& awake = true);
    /// @returns world position of the center of mass.
	const b2Vec2& GetWorldCenter() const;
    /// @returns local position of the center of mass.
	const b2Vec2& GetLocalCenter() const;
    /// @brief Set the linear velocity of the center of mass.
	/// @param v the new linear velocity of the center of mass.
	void SetLinearVelocity(const b2Vec2& v);
	/// @returns linear velocity of the center of mass.
	const b2Vec2& GetLinearVelocity() const;
    /// @brief Set the angular velocity.
	/// @param omega the new angular velocity in radians/second.
	void SetAngularVelocity(const float& omega);
	/// @return the angular velocity in radians/second.
	float GetAngularVelocity() const;
    /// @brief Apply a force at a world point. 
    /// @note If the force is not applied at the center of mass, it will generate a torque and affect the angular velocity. This wakes up the body.
	/// @param force the world force vector, usually in Newtons (N).
	/// @param point the world position of the point of application.
	/// @param wake also wake up the body
	void ApplyForce(const b2Vec2& force, const b2Vec2& point, const bool& wake = true);
    /// @brief Apply a force to the center of mass. This wakes up the body.
	/// @param force the world force vector, usually in Newtons (N).
	/// @param wake also wake up the body
	void ApplyForceToCenter(const b2Vec2& force, const bool& wake = true);
    /// @brief Apply a torque. 
    /// @note This affects the angular velocity without affecting the linear velocity of the center of mass.
	/// @param torque about the z-axis (out of the screen), usually in N-m.
	/// @param wake also wake up the body
	void ApplyTorque(const float& torque, const bool& wake = true);
    /// @brief Apply an impulse at a point. This immediately modifies the velocity.
	/// @note It also modifies the angular velocity if the point of application is not at the center of mass. This wakes up the body.
	/// @param impulse the world impulse vector, usually in N-seconds or kg-m/s.
	/// @param point the world position of the point of application.
	/// @param wake also wake up the body
	void ApplyLinearImpulse(const b2Vec2& impulse, const b2Vec2& point, const bool& wake = true);
    /// @brief Apply an impulse to the center of mass. This immediately modifies the velocity.
	/// @param impulse the world impulse vector, usually in N-seconds or kg-m/s.
	/// @param wake also wake up the body
	void ApplyLinearImpulseToCenter(const b2Vec2& impulse, const bool& wake = true);
    /// @brief Apply an angular impulse.
	/// @param impulse the angular impulse in units of kg*m*m/s
	/// @param wake also wake up the body
	void ApplyAngularImpulse(const float& impulse, const bool& wake = true);
    /// Get the total mass of the body.
	/// @returns mass in kilograms (kg).
	float GetMass() const;
    /// @brief Get the rotational inertia of the body about the local origin.
	/// @returns rotational inertia in kg-m^2.
	float GetInertia() const;
    /// @brief  the mass data of the body.
	/// @returns a struct containing the mass, inertia and center of the body.
	b2MassData GetMassData() const;
    /// @brief Set the mass properties to override the mass properties of the fixtures.
	/// @note that this changes the center of mass position.
	/// @note that creating or destroying fixtures can also alter the mass.
	/// @note This function has no effect if the body isn't dynamic.
	/// @param data the mass properties.
	void SetMassData(const b2MassData* data);
    /// @brief This resets the mass properties to the sum of the mass properties of the fixtures.
	/// @note This normally does not need to be called unless you called SetMassData to override the mass and you later want to reset the mass.
	void ResetMassData();
    /// @returns linear damping of the body.
	float GetLinearDamping() const;
    /// @brief Set the linear damping of the body.
	void SetLinearDamping(const float& linearDamping);
    /// @returns angular damping of the body.
	float GetAngularDamping() const;
    /// @brief Set the angular damping of the body.
	void SetAngularDamping(const float& angularDamping);
    /// @returns gravity scale of the body.
	float GetGravityScale() const;
    /// @brief Set the gravity scale of the body.
	void SetGravityScale(const float& scale);
    /// @brief Set the type of this body. This may alter the mass and velocity.
	void SetType(const b2BodyType& type);
    /// @returns type of this body.
	b2BodyType GetType() const;
    /// @brief Should this body be treated like a bullet for continuous collision detection?
	void SetBullet(const bool& flag = true);
    /// @brief Is this body treated like a bullet for continuous collision detection?
	bool IsBullet() const;
    /// @brief You can disable sleeping on this body. If you disable sleeping, the body will be woken.
	void SetSleepingAllowed(const bool& flag = true);
    /// @brief Is this body allowed to sleep
	bool IsSleepingAllowed() const;
    /// @brief Get the sleeping state of this body.
	/// @returns true if the body is awake.
	bool IsAwake() const;
    /// @brief Set this body to have fixed rotation
    /// @note This causes the mass to be reset.
	void SetFixedRotation(const bool& flag = true);
    /// @returns Does this body have fixed rotation?
	bool IsFixedRotation() const;
    /// @note use Fixture.GetNext() to iterate through the list
    /// @returns the first fixture on this body
	Fixture GetFixtureList();
    // TODO implement joints
    // /// Get the list of all joints attached to this body.
	// b2JointEdge* GetJointList();
	// const b2JointEdge* GetJointList() const;

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
    std::set<ContactData> m_currentCollisions;
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
 