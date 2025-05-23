#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#pragma once

#include <set>

#include "box2d/types.h"

#include "Physics/Fixture.hpp"
#include "Physics/FixtureArray.hpp"
#include "Physics/ContactData.hpp"
#include "Physics/ContactDataArray.hpp"
#include "Physics/WorldHandler.hpp"
#include "Object.hpp"
#include "EngineSettings.hpp"

class CollisionManager;

// TODO make a gui editor for making bodies over an image (prints the code that will produce the given effect) (should also be able to load based on given code)
// TODO make parent and child colliders have defined behaviour
/// @warning If there is a parent and child that have a Collider there is undefined behaviour (try using fixtures instead)
/// @note when added to destroy queue physics is set disabled and removed when fully destroyed
class Collider : public virtual Object
{
public:
    using Ptr = Object::Ptr<Collider>;

    Collider(WorldHandler& world = WorldHandler::get());
    virtual ~Collider();

    /// @brief Set the physics enabled state of this object
    /// @note if the object is disabled then this will not enable physics until the object is enabled
    /// @warning this is an expensive operation if the enabled state changes
    void setPhysicsEnabled(bool enabled = true);
    /// @note if the object is enabled then returns the physics state otherwise returns false
    /// @returns true if the physics are enabled and the object is enabled
    bool isPhysicsEnabled() const;

    /// @brief creates a fixture with the given data and shape
    Fixture createFixture(const Fixture::Shape::Circle& shape, const FixtureDef& fixtureDef = FixtureDef{});
    /// @brief creates a fixture with the given data and shape
    /// @note ignores the senor option in fixture def
    Fixture createFixtureSensor(const Fixture::Shape::Circle& shape, FixtureDef fixtureDef = FixtureDef{});
    /// @brief creates a fixture with the given data and shape
    Fixture createFixture(const Fixture::Shape::Capsule& shape, const FixtureDef& fixtureDef = FixtureDef{});
    /// @brief creates a fixture with the given data and shape
    /// @note ignores the senor option in fixture def
    Fixture createFixtureSensor(const Fixture::Shape::Capsule& shape, FixtureDef fixtureDef = FixtureDef{});
    /// @brief creates a fixture with the given data and shape
    Fixture createFixture(const Fixture::Shape::Segment& shape, const FixtureDef& fixtureDef = FixtureDef{});
    /// @brief creates a fixture with the given data and shape
    /// @note ignores the senor option in fixture def
    Fixture createFixtureSensor(const Fixture::Shape::Segment& shape, FixtureDef fixtureDef = FixtureDef{});
    /// @brief creates a fixture with the given data and shape
    /// @warning asserts that the polygon is valid
    Fixture createFixture(const Fixture::Shape::Polygon& shape, const FixtureDef& fixtureDef = FixtureDef{});
    /// @brief creates a fixture with the given data and shape
    /// @note ignores the senor option in fixture def
    /// @warning asserts that the polygon is valid
    Fixture createFixtureSensor(const Fixture::Shape::Polygon& shape, FixtureDef fixtureDef = FixtureDef{});
	/// @param fixture the fixture to be removed.
    /// @param updateBodyMass You may defer the body mass update which can improve performance if several shapes on a body are destroyed at once.
    /// @note the given fixture will be invalid after this function is called since it is just a reference to the actual fixture
	void destroyFixture(const Fixture& fixture, bool updateBodyMass = true);

    /// @brief called when a contact begins
    /// @note these are called for each fixture
    /// @note only called if both fixtures are NOT sensors
    /// @param ContactData the collision data
    inline virtual void BeginContact(ContactData ContactData) {}; // TODO add sensor specific versions?
    /// @brief called when a contact ends
    /// @note these are called for each fixture
    /// @note only called if both fixtures are NOT sensors
    /// @param ContactData the collision data
    inline virtual void EndContact(ContactData ContactData) {};
    /// @brief called when two fixtures begin contact and one is a sensor
    /// @note these are called for each fixture
    /// @warning only gets called if sensor events are enabled for this fixture
    /// @param ContactData the collision data
    inline virtual void BeginContactSensor(ContactData ContactData) {};
    /// @brief called when two fixtures end contact and one is a sensor
    /// @note these are called for each fixture
    /// @warning only gets called if sensor events are enabled for this fixture
    /// @param ContactData the collision data
    inline virtual void EndContactSensor(ContactData ContactData) {};
    /// @brief This is called before any collision is handled
    /// @note this will be called if this collider or the one colliding with this one has PreSolveEvents enabled
    /// @warning MUST be thread safe assuming you ever use multiple threads with the physics
    /// @warning do NOT write to the colliders or anything physics related during this callback
    /// @note try to make this efficient as it can be called many times per frame
    /// @param PreContactData the data that should be used for editing any physics data (it will be edited after the physics update)
    /// @return true if you want the collision to be handled, false if you want to ignore the collision (stops if either collider returns false, only stops for this one collision)
    inline virtual bool PreSolve(PreSolveData data) { return true; };
    /// @brief called every frame until the two objects are no longer colliding
    /// @note this will also be called on start of contact
    /// @note this is called for each fixture
    /// @note only called if both fixtures are NOT sensors
    inline virtual void OnColliding(ContactData ContactData) {};

    /// @returns Get the rotational inertia of the body, usually in kg*m^2
    float getRotationalInertia() const;
    /// @returns center of mass position in local space
    Vector2 getLocalCenterOfMass() const;
    /// @returns center of mass position in global space
    Vector2 getGlobalCenterOfMass() const;
    /// @brief This updates the mass properties to the sum of the mass properties of the fixtures
    /// @note This normally does not need to be called unless you called SetMassData to override the mass and you later want to reset the mass.
    /// @note You may also use this when automatic mass computation has been disabled.
    void applyMassFromShapes();
    /// @brief Set the sleep threshold, typically in meters per second
    void setSleepThreshold(float sleepVelocity);
    /// @brief Get the sleep threshold, typically in meters per second.
    float getSleepThreshold() const;
    /// @brief set if this body can sleep
    /// @note If you disable sleeping, the body will be woken
	void setSleepingEnabled(bool enabled = true);
    /// @brief Is this body allowed to sleep
	bool isSleepingEnabled() const;

    /// @returns number of fixtures on this body
    int getFixtureCount() const;
    /// @returns a fixture array of fixtures on this body up to the max size given
    FixtureArray getFixtureArray(int maxSize = INT_MAX);
    /// @returns a fixture array of fixtures on this body up to the max size given
    FixtureArray getFixtureArray(int maxSize = INT_MAX) const;
    /// @returns how many contacts this body has
    int getContactsCount() const;
    ContactDataArray getContacts(int maxSize = INT_MAX);
    ContactDataArray getContacts(int maxSize = INT_MAX) const;
    // TODO implement joint getter

    /// @brief Get the current world AABB that contains all the attached shapes
    /// @note this may not encompass the body origin
    /// @note If there are no shapes attached then the returned AABB is empty and centered on the body origin.
    b2AABB computeAABB() const;
    
    /// @brief Set the sleep state of the body
    /// @note A sleeping body has very low CPU cost.
	/// @param flag set to true to wake the body, false to put it to sleep
    void setAwake(bool awake = true);
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
    /// @brief the mass data of the body.
	/// @returns a struct containing the mass, inertia and center of the body.
	b2MassData getMassData() const;
    /// @brief calculates the mass of this collider via all of its fixtures
    /// @note only use this if getMassData does not satisfy your needs (i.e. body is not of type b2_dynamicBody)
    /// @note just sums the mass from every fixture
    /// @returns the mass of this collider
    float calculateMass() const;
    /// @brief Set the mass properties to override the mass properties of the fixtures.
	/// @note that this changes the center of mass position.
	/// @note that creating or destroying fixtures can also alter the mass.
	/// @note This function has no effect if the body isn't dynamic.
	/// @param data the mass properties.
	void setMassData(const b2MassData& data);
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
    /// @brief Get the sleeping state of this body.
	/// @returns true if the body is awake.
	bool isAwake() const;
    /// @brief Set this body to have fixed rotation
    /// @note This causes the mass to be reset.
	void setFixedRotation(bool flag = true);
    /// @returns Does this body have fixed rotation?
	bool isFixedRotation() const;
    // TODO implement joints
    // /// Get the list of all joints attached to this body.
	// b2JointEdge* GetJointList();
	// const b2JointEdge* GetJointList() const;
    Transform getInterpolatedTransform() const override;

protected:

private:
    friend CollisionManager;
    friend Fixture;
    /// @brief updates the object transform to this colliders body transform
    void m_update(b2Transform* transform);
    /// @brief updates the body state (enabled or not)
    void m_updatePhysicsState();
    /// @brief updates the body transform to match the object transform
    void m_updateTransform();

    // TODO make the body dynamically (if no fixtures destroy it, if adding fixture and no body make one)
    // This is because of a note on the box2d website "‍Caution: A dynamic body should have at least one shape with a non-zero density. Otherwise you will get strange behavior."
    b2BodyId m_body = b2_nullBodyId; 
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
 