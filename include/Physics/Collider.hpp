#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#pragma once

#include <set>

#include "box2d/box2d.h"

#include "Physics/Fixture.hpp"
#include "Object.hpp"
#include "EngineSettings.hpp"

// TODO go through all physics classes and make sure that all box2d functions are able to be used one way or another
class CollisionManager; // defined early to friend in Collider

class Collider;

class HitData
{
public:
    /// @brief ignore this unless you know what you are doing
    HitData(Collider* otherCollider, b2ShapeId otherShape, b2ShapeId thisShape, Vector2 normal, Vector2 point, float approachSpeed);

    /// @returns the other objects collider
    Collider* getCollider();
    const Collider* getCollider() const;
    /// @returns the fixture from this object that collided
    Fixture getThisFixture();
    const Fixture getThisFixture() const;
    /// @returns the fixture from the other object that collided
    Fixture getOtherFixture();
    const Fixture getOtherFixture() const;

    /// @returns the point where the shapes hit
    Vector2 getContactPoint() const;
    /// @returns the normal vector pointing from shape A to B
    Vector2 getNormal() const;
    /// @returns the speed the shapes are approaching
    float getApproachSpeed() const;

protected:
    Collider *const m_collider;
    b2ShapeId m_otherShape;
    b2ShapeId m_thisShape;
    Vector2 m_normal;
    Vector2 m_point;
    float m_approachSpeed;
};

class ContactData
{
public:
    /// @brief ignore this unless you know what you are doing
    ContactData(Collider* otherCollider, b2ShapeId otherShape, b2ShapeId thisShape);

    /// @returns the other objects collider
    Collider* getCollider();
    const Collider* getCollider() const;
    /// @returns the fixture from this object that collided
    Fixture getThisFixture();
    const Fixture getThisFixture() const;
    /// @returns the fixture from the other object that collided
    Fixture getOtherFixture();
    const Fixture getOtherFixture() const;

protected:
    Collider *const m_collider;
    b2ShapeId m_otherShape;
    b2ShapeId m_thisShape;
};

/// @note to disable to current collision make sure to return false in the callback
/// @note event if you destroy one or both of the bodies the collision will still be active for the current frame (unless you return false)
class PreSolveData
{
public: 
    /// @brief ignore this unless you know what you are doing
    PreSolveData(Collider* collider, b2ShapeId thisFixture, b2ShapeId otherFixture, b2Manifold* manifold);

    const Collider* getCollider() const;
    /// @warning NEVER edit the collider in the pre solve callback this should only be used for storage for editing later
    /// @warning it may seem tempting but DONT destroy the collider through this ptr (use the dedicated function)
    /// @returns a non const ptr to the other collider
    Collider* getCollider_NoneConst();
    const Fixture getThisFixture() const;
    const Fixture getOtherFixture() const;
    /// @brief adds the collider to a destroy queue for after the physics update
    /// @note this only takes effect after the physics update has finished
    void destroyOtherCollider() const;
    /// @brief adds the collider to a destroy queue for after the physics update
    /// @note this only takes effect after the physics update has finished
    void destroyThisCollider() const;

    /// @returns a list of 0-2 contact points
    std::list<Vector2> getContactPoints() const;
    /// @returns the normal vector of the contact
    Vector2 getNormal() const;

private:
    Collider *const m_collider;
    b2ShapeId m_thisFixture;
    b2ShapeId m_otherFixture;
    const b2Manifold *const m_manifold;
};

typedef ContactData CollisionData;

// TODO make a gui editor for making bodies over an image (prints the code that will produce the given effect) (should also be able to load based on given code)
// TODO implement contact filtering
// TODO make parent and child colliders have defined behaviour

// TODO update functions for newer version of box2d

/// @warning If there is a parent and child that have a Collider there is undefined behaviour (try using fixtures instead)
/// @note positions assume that the origin is in the middle of the shape
/// @note when added to destroy queue physics is set disabled and removed when fully destroyed
/// @bug onTransformUpdates is NOT invoked from physics position updates
class Collider : public virtual Object
{
public:
    using Ptr = Object::Ptr<Collider>;

    Collider();
    virtual ~Collider();

    /// @brief sets only physics enabled or disabled, ignores the object state
    /// @note if disabled object state does not matter if enabled physics will follow object state
    void setPhysicsEnabled(bool enabled = true);
    /// @returns true if physics are enabled
    bool isPhysicsEnabled() const;

    /// @param density the density of the shape
    /// @param friction the friction of the shape usually between [0,1]
    /// @param restitution the restitution (elasticity) usually in the range [0,1]
    /// @param restitutionThreshold Restitution velocity threshold, usually in m/s. Collisions above this speed have restitution applied (will bounce)
    /// @param filter contact filtering data
    /// @returns the new fixture
    Fixture createFixture(const b2Polygon& shape, float friction = 0.1f, float restitution = 0.f, float restitutionThreshold = 0.f, 
                          float density = 1.f, const b2Filter& filter = {});
    /// @brief creates a sensor fixture with the given shape and density
    /// @param density the density of the shape
    /// @param filter contact filtering data
    /// @returns the new fixture
    Fixture createFixtureSensor(const b2Polygon& shape, float density = 1.f, const b2Filter& filter = {});
	/// @param fixture the fixture to be removed.
	void destroyFixture(const Fixture& fixture);

    /// @brief called when a contact begins
    /// @note these are called for each fixture
    /// @param ContactData the collision data
    inline virtual void BeginContact(ContactData ContactData) {};
    /// @brief called when a contact ends
    /// @note these are called for each fixture
    /// @param ContactData the collision data
    inline virtual void EndContact(ContactData ContactData) {};
    /// @brief This can be called multiple times in one frame (called before any collision is handled)
    /// @note try to make this efficient as it can be called many times per frame
    /// @warning do NOT edit the colliders during this callback
    /// @warning do NOT create objects during this callback
    /// @param PreContactData the pre solve contact data
    /// @returns if the contact should be handled or not
    inline virtual bool PreSolve(PreSolveData data) { return true; };
    /// @brief called every frame until the two objects are no longer colliding
    /// @note this will also be called on start of contact
    /// @note this is called for each fixture
    inline virtual void OnColliding(ContactData ContactData) {};
    /// @brief called when the collider has fallen asleep (if sleeping is enabled)
    inline virtual void OnFellAsleep() {};
    /// @brief A hit touch event is generated when two shapes collide with a speed faster than the hit speed threshold
    inline virtual void OnContactHit(HitData HitData) {};

    /// @brief Set the sleep state of the body
    /// @note A sleeping body has very low CPU cost.
	/// @param flag set to true to wake the body, false to put it to sleep
    void setAwake(bool awake = true);
    /// @returns world position of the center of mass.
	Vector2 getWorldCenterOfMass() const;
    /// @returns local position of the center of mass.
	Vector2 getLocalCenterOfMass() const;
    /// @brief Set the linear velocity of the center of mass.
	/// @param v the new linear velocity of the center of mass.
	void setLinearVelocity(const Vector2& v);
	/// @returns linear velocity of the center of mass.
	Vector2 getLinearVelocity() const;
    /// @brief Set the angular velocity.
	/// @param omega the new angular velocity in radians/second.
	void setAngularVelocity(float omega);
	/// @return the angular velocity in radians/second.
	float getAngularVelocity() const;
    /// @brief Apply a force at a world point. 
    /// @note If the force is not applied at the center of mass, it will generate a torque and affect the angular velocity. This wakes up the body.
	/// @param force the world force vector, usually in Newtons (N).
	/// @param point the world position of the point of application.
	/// @param wake also wake up the body
	void applyForce(const Vector2& force, const Vector2& point, bool wake = true);
    /// @brief Apply a force to the center of mass. This wakes up the body.
	/// @param force the world force vector, usually in Newtons (N).
	/// @param wake also wake up the body
	void applyForceToCenter(const Vector2& force, bool wake = true);
    /// @brief Apply a torque. 
    /// @note This affects the angular velocity without affecting the linear velocity of the center of mass.
	/// @param torque about the z-axis (out of the screen), usually in N-m.
	/// @param wake also wake up the body
	void applyTorque(float torque, bool wake = true);
    /// @brief Apply an impulse at a point. This immediately modifies the velocity.
	/// @note It also modifies the angular velocity if the point of application is not at the center of mass. This wakes up the body.
	/// @param impulse the world impulse vector, usually in N-seconds or kg-m/s.
	/// @param point the world position of the point of application.
	/// @param wake also wake up the body
	void applyLinearImpulse(const Vector2& impulse, const Vector2& point, bool wake = true);
    /// @brief Apply an impulse to the center of mass. This immediately modifies the velocity.
	/// @param impulse the world impulse vector, usually in N-seconds or kg-m/s.
	/// @param wake also wake up the body
	void applyLinearImpulseToCenter(const Vector2& impulse, bool wake = true);
    /// @brief Apply an angular impulse.
	/// @param impulse the angular impulse in units of kg*m*m/s
	/// @param wake also wake up the body
	void applyAngularImpulse(float impulse, bool wake = true);
    /// Get the total mass of the body.
	/// @returns mass in kilograms (kg).
	float getMass() const;
    /// @brief Get the rotational inertia of the body about the local origin.
	/// @returns rotational inertia in kg-m^2.
	float getInertia() const;
    /// @brief Get the mass data for a body
	b2MassData getMassData() const;
    /// @brief Override the body's mass properties. Normally this is computed automatically using the shape geometry and density. This information is lost if a shape is added or removed or if the	body type changes.
	void setMassData(b2MassData data);
    /// @returns linear damping of the body.
	float getLinearDamping() const;
    /// @brief Set the linear damping of the body.
	void setLinearDamping(float linearDamping);
    /// @returns angular damping of the body.
	float getAngularDamping() const;
    /// @brief Set the angular damping of the body.
	void setAngularDamping(float angularDamping);
    /// @returns gravity scale of the body.
	float getGravityScale() const;
    /// @brief Set the gravity scale of the body.
	void setGravityScale(float scale);
    /// @brief Set the type of this body. This may alter the mass and velocity.
	void setType(b2BodyType type);
    /// @returns type of this body.
	b2BodyType getType() const;
    /// @brief Should this body be treated like a bullet for continuous collision detection?
	void setBullet(bool flag = true);
    /// @brief Is this body treated like a bullet for continuous collision detection?
	bool isBullet() const;
    /// @brief You can disable sleeping on this body. If you disable sleeping, the body will be woken.
	void setSleepingEnabled(bool flag = true);
    /// @brief Is this body Enabled to sleep
	bool isSleepingEnabled() const;
    /// @brief Set the sleep threshold, typically in meters per second
    void setSleepingThreshold(float velocity);
    /// @brief Get the sleep threshold, typically in meters per second.
    float getSleepingThreshold() const;
    /// @brief Get the sleeping state of this body.
	/// @returns true if the body is awake.
	bool isAwake() const;
    /// @brief Set this body to have fixed rotation
    /// @note This causes the mass to be reset.
	void setFixedRotation(bool flag = true);
    /// @returns Does this body have fixed rotation?
	bool isFixedRotation() const;
	FixtureList getFixtureList();
    // TODO implement joints and chains
    // /// Get the list of all joints attached to this body.
	// b2JointEdge* GetJointList();
	// const b2JointEdge* GetJointList() const;
    Transform getInterpolatedTransform() const override;

protected:

private:
    friend CollisionManager;
    friend Fixture;
    /// @brief updates the object transform to this colliders body transform
    void m_update(b2Transform transform);
    /// @brief updates the body state (enabled or not)
    void m_updatePhysicsState();
    /// @brief updates the body transform to match the object transform
    void m_updateTransform();

    b2BodyId m_body = b2_nullBodyId;
    /// @brief if true follows object else physics are disabled no matter object state
    bool m_enabled = true;
};

namespace std {
    template <>
    struct hash<Collider> {
        inline uint64_t operator()(const Collider& obj) const noexcept
        {
            return hash<uint64_t>{}(obj.getID());
        }
    };
    template <>
    struct hash<Collider*> {
        inline uint64_t operator()(const Collider* obj) const noexcept
        {
            if (obj == nullptr)
                return 0;
            return hash<uint64_t>{}(obj->getID());
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
 