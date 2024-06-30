#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#pragma once

#include <set>

#include "box2d/box2d.h"

#include "Physics/Fixture.hpp"
#include "Object.hpp"
#include "Settings.hpp"

class CollisionManager; // defined early to friend in Collider

class Collider;

class ContactData
{
public:
    /// @brief ignore this unless you know what you are doing
    ContactData(Collider* collider, b2Fixture* thisFixture, b2Fixture* otherFixture, b2Contact* contactData);

    /// @returns the other objects collider
    Collider* getCollider();
    const Collider* getCollider() const;
    /// @returns the fixture from this object that collided
    Fixture getThisFixture();
    const Fixture getThisFixture() const;
    /// @returns the fixture from the other object that collided
    Fixture getOtherFixture();
    const Fixture getOtherFixture() const;

    class Info
    {
    public:
        /// @brief ignore this unless you know what you are doing
        Info(b2Contact* contact);

        /// @returns how many contact points there are
        int32 getPointCount() const;

        /// @warning does not check if index is in range
        /// @returns returns the point at the given index (size of point count)
        const Vector2 getContactPoint(uint8 index) const;

        /// @warning does not check if index is in range
        /// @note negative distance is overlap in meters
        /// @returns returns the distance at the given index (size of point count)
        const float getSeparations(uint8 index) const;
        /// @returns the normal of the collision
        Vector2 getNormal() const;

    protected:
        b2WorldManifold m_data;
        int32 m_points;
    };

    /// @brief calculates more in-depth contact information
    /// @note calculates the contact normal, points of contact, and distance of overlap
    ContactData::Info getInfo() const;

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

class PreSolveData
{
public: 
    /// @brief ignore this unless you know what you are doing
    PreSolveData(Collider* collider, b2Fixture* thisFixture, b2Fixture* otherFixture, b2Contact* contactData);

    const Collider* getCollider() const;
    const Fixture getThisFixture() const;
    const Fixture getOtherFixture() const;
    ContactData::Info getInfo() const;

    /// @returns if the two fixtures are touching
    bool isTouching() const;
    /// @brief Enable/disable this contact
	/// @note The contact is only disabled for the curren time step (or sub-step in continuous collisions)
	void setEnabled(bool flag = true);
    /// @brief Has this contact been disabled?
	bool isEnabled() const;
    /// @brief Override the default friction mixture
	/// @note This value persists until set or reset.
	void setFriction(float friction);
    /// @returns friction mixture
	float getFriction() const;
    /// @brief Reset the friction mixture to the default value.
	void resetFriction();
    /// @brief Override the default restitution mixture
	/// @note The value persists until you set or reset.
	void setRestitution(float restitution);
    /// @returns restitution mixture
	float getRestitution() const;
    /// @brief Reset the restitution to the default value.
	void resetRestitution();
    /// @brief Override the default restitution velocity threshold mixture
	/// @note The value persists until you set or reset.
	void setRestitutionThreshold(float threshold);
    /// @returns restitution threshold mixture
	float getRestitutionThreshold() const;
    /// @brief Reset the restitution threshold to the default value.
	void resetRestitutionThreshold();
    /// @brief Set the desired tangent speed for a conveyor belt behavior. In meters per second.
	void setTangentSpeed(float speed);
    /// @returns tangent speed. In meters per second.
	float getTangentSpeed() const;

private:
    Collider *const m_collider;
    b2Fixture *const m_thisFixture;
    b2Fixture *const m_otherFixture;
    b2Contact *const m_contactData;
};

typedef ContactData CollisionData;

// TODO make a gui editor for making bodies over an image (prints the code that will produce the given effect) (should also be able to load based on given code)
// TODO make a wrapper for creating fixtures of different shapes
// TODO implement contact filtering
// TODO make parent and child colliders have defined behaviour
/// @warning If there is a parent and child that have a Collider there is undefined behaviour (try using fixtures instead)
class Collider : public virtual Object
{
public:
    Collider();
    ~Collider();

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
    Fixture createFixture(const b2Shape& shape, float friction = 0.1f, float restitution = 0.f, float restitutionThreshold = 0.f, 
                          float density = 1.f, const b2Filter& filter = {});
    /// @brief creates a sensor fixture with the given shape and density
    /// @param density the density of the shape
    /// @param filter contact filtering data
    /// @returns the new fixture
    Fixture createFixtureSensor(const b2Shape& shape, float density = 1.f, const b2Filter& filter = {});
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
    /// @warning do NOT DESTROY OR EDIT the ANY colliders during this callback
    /// @param PreContactData the pre solve contact data
    inline virtual void PreSolve(PreSolveData data) {};
    /// @brief called every frame until the two objects are no longer colliding
    /// @note this will also be called on start of contact
    /// @note these are called for each fixture
    inline virtual void OnColliding(ContactData ContactData) {};

    void setAwake(bool awake = true);
    /// @returns world position of the center of mass.
	Vector2 getWorldCenter() const;
    /// @returns local position of the center of mass.
	Vector2 getLocalCenter() const;
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
    /// @brief  the mass data of the body.
	/// @returns a struct containing the mass, inertia and center of the body.
	b2MassData getMassData() const;
    /// @brief Set the mass properties to override the mass properties of the fixtures.
	/// @note that this changes the center of mass position.
	/// @note that creating or destroying fixtures can also alter the mass.
	/// @note This function has no effect if the body isn't dynamic.
	/// @param data the mass properties.
	void setMassData(const b2MassData* data);
    /// @brief This resets the mass properties to the sum of the mass properties of the fixtures.
	/// @note This normally does not need to be called unless you called SetMassData to override the mass and you later want to reset the mass.
	void resetMassData();
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
	void setSleepingAllowed(bool flag = true);
    /// @brief Is this body allowed to sleep
	bool isSleepingAllowed() const;
    /// @brief Get the sleeping state of this body.
	/// @returns true if the body is awake.
	bool isAwake() const;
    /// @brief Set this body to have fixed rotation
    /// @note This causes the mass to be reset.
	void setFixedRotation(bool flag = true);
    /// @returns Does this body have fixed rotation?
	bool isFixedRotation() const;
    /// @note use Fixture.GetNext() to iterate through the list
    /// @returns the first fixture on this body
	Fixture getFixtureList();
    // TODO implement joints
    // /// Get the list of all joints attached to this body.
	// b2JointEdge* GetJointList();
	// const b2JointEdge* GetJointList() const;
    Transform getInterpolatedTransform() const override;

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
 