#ifndef CONTACT_DATA_HPP
#define CONTACT_DATA_HPP

#include "box2d/box2d.h"
#include "Physics/Fixture.hpp"
#include "Utils/EventHelper.hpp"

class Collider;
class CollisionManager;
class ContactDataArray;

/// @note dont ever store this object
class ContactData
{
public:    
    /// @returns the other objects collider
    Collider* getCollider();
    const Collider* getCollider() const;
    /// @returns the fixture from this object that collided
    Fixture getThisFixture();
    const Fixture getThisFixture() const;
    /// @returns the fixture from the other object that collided
    Fixture getOtherFixture();
    const Fixture getOtherFixture() const;

private:
    friend CollisionManager;
    friend ContactDataArray;

    /// @brief ignore this unless you know what you are doing
    ContactData(b2ShapeId thisFixture, b2ShapeId otherFixture);

    b2ShapeId const m_thisShape;
    b2ShapeId const m_otherShape;
};

class HitData
{
public:
    /// @brief ignore this unless you know what you are doing
    HitData(b2ShapeId thisFixture, b2ShapeId otherFixture, const b2ContactHitEvent* m_hitData);

    /// @returns the other objects collider
    Collider* getCollider();
    const Collider* getCollider() const;
    /// @returns the fixture from this object that collided
    Fixture getThisFixture();
    const Fixture getThisFixture() const;
    /// @returns the fixture from the other object that collided
    Fixture getOtherFixture();
    const Fixture getOtherFixture() const;
    /// @brief Point where the fixtures hit
    Vector2 getContactPoint() const;
    /// @brief Normal vector pointing from shape A to shape B
    Vector2 getContactNormal() const;
    /// @brief The speed the shapes are approaching. Always positive. Typically in meters per second.
    float getApproachSpeed() const;

private:
    b2ShapeId const m_thisShape;
    b2ShapeId const m_otherShape;
    const b2ContactHitEvent* const m_hitData;
};

/// @note dont ever store this object
class PreSolveData // TODO implement a way to use the updateBody event for either all functions in the collider or a way to easily add functions to the event
{
public: 
    /// @brief ignore this unless you know what you are doing
    PreSolveData(Collider* thisCollider, b2ShapeId thisFixture, Collider* otherCollider, b2ShapeId otherFixture, b2Manifold* contactData, EventHelper::Event* updateBodyEvent);

    const Collider* getCollider() const;
    /// @warning NEVER edit the collider in the pre solve callback this should only be used for storage and editing later
    /// @returns a non const ptr to the other collider
    Collider* getNoneConstCollider();
    const Fixture getThisFixture() const;
    const Fixture getOtherFixture() const;

    /// @brief destroys the other collider AFTER physics is done updating
    void destroyOtherCollider();
    /// @brief destroys this collider AFTER physics is done updating
    /// @warning DO NOT destroy this collider directly only use this function in the callback
    void destroyThisCollider();

    // /// @returns if the two fixtures are touching
    // bool isTouching() const;
    // /// @brief Enable/disable this contact
	// /// @note The contact is only disabled for the curren time step (or sub-step in continuous collisions)
	// void setEnabled(bool flag = true);
    // /// @brief Has this contact been disabled?
	// bool isEnabled() const;
    // /// @brief Override the default friction mixture
	// /// @note This value persists until set or reset.
	// void setFriction(float friction);
    // /// @returns friction mixture
	// float getFriction() const;
    // /// @brief Reset the friction mixture to the default value.
	// void resetFriction();
    // /// @brief Override the default restitution mixture
	// /// @note The value persists until you set or reset.
	// void setRestitution(float restitution);
    // /// @returns restitution mixture
	// float getRestitution() const;
    // /// @brief Reset the restitution to the default value.
	// void resetRestitution();
    // /// @brief Override the default restitution velocity threshold mixture
	// /// @note The value persists until you set or reset.
	// void setRestitutionThreshold(float threshold);
    // /// @returns restitution threshold mixture
	// float getRestitutionThreshold() const;
    // /// @brief Reset the restitution threshold to the default value.
	// void resetRestitutionThreshold();
    // /// @brief Set the desired tangent speed for a conveyor belt behavior. In meters per second.
	// void setTangentSpeed(float speed);
    // /// @returns tangent speed. In meters per second.
	// float getTangentSpeed() const;

private:
    Collider *const m_thisCollider;
    b2ShapeId const m_thisShape;
    Collider *const m_otherCollider;
    b2ShapeId const m_otherShape;
    b2Manifold *const m_manifold;
    EventHelper::Event *const m_updateBodyEvent;
};

typedef ContactData CollisionData;

#endif
