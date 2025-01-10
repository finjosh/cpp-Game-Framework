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

    /// @returns how many contact points there are
    std::int32_t getPointCount() const;
    /// @warning asserts that the index is in range [0,(size of point count)]
    /// @returns returns the point at the given index
    const Vector2 getContactPoint(std::int32_t index) const;
    /// @warning asserts that the index is in range [0,(size of point count)]
    /// @note negative distance is overlap in meters
    /// @returns returns the distance at the given index
    const float getSeparations(std::int32_t index) const;
    /// @returns the normal of the collision
    Vector2 getNormal() const;

private:
    friend CollisionManager;
    friend ContactDataArray;

    /// @brief ignore this unless you know what you are doing
    ContactData(b2ShapeId thisFixture, b2ShapeId otherFixture, b2Manifold* manifold);

    b2ShapeId const m_thisShape;
    b2ShapeId const m_otherShape;
    b2Manifold* const m_manifold;
};

// TODO implement a way to use the updateBody event for either all functions in the collider or a way to easily add functions to the event
/// @note dont ever store this object
class PreSolveData 
{
public: 
    const Collider* getCollider() const;
    /// @warning NEVER edit the collider in the pre solve callback this should only be used for storage and editing later
    /// @returns a non const ptr to the other collider
    Collider* getNoneConstCollider();
    const Fixture getThisFixture() const;
    const Fixture getOtherFixture() const;

    /// @brief destroys the other collider AFTER physics is done updating
    /// @warning do NOT try to edit any other data after doing this
    void destroyOtherCollider();
    /// @brief destroys this collider AFTER physics is done updating
    /// @warning DO NOT destroy this collider directly only use this function in the callback
    /// @warning do NOT try to edit any other data after doing this
    void destroyThisCollider();

    /// @brief if you want to edit any physics data you should add it to this event
    /// @note this can also be used to add functions that you want to be called in a thread safe manner after the physics update
    /// @note you should only set collider data is you are 100% sure the collider will not be destroyed by any other preSolveCallbacks
    /// @warning there is no assert to make sure that you are not using data after a collider is destroyed when adding directly to the event
    /// @warning do NOT store this
    /// @returns an event that will be invoked and cleared after the physics update
    EventHelper::Event& getUpdateEvent();

    /// @returns how many contact points there are
    std::int32_t getPointCount() const;
    /// @warning asserts that the index is in range [0,(size of point count)]
    /// @returns returns the point at the given index
    const Vector2 getContactPoint(std::int32_t index) const;
    /// @warning asserts that the index is in range [0,(size of point count)]
    /// @note negative distance is overlap in meters
    /// @returns returns the distance at the given index
    const float getSeparations(std::int32_t index) const;
    /// @returns the normal of the collision
    Vector2 getNormal() const;

private:
    friend CollisionManager;

    PreSolveData(b2ShapeId thisFixture, b2ShapeId otherFixture, const b2Manifold* contactData, EventHelper::Event* updateBodyEvent);

    b2ShapeId const m_thisShape;
    b2ShapeId const m_otherShape;
    const b2Manifold *const m_manifold;
    EventHelper::Event *const m_updateBodyEvent;
};

typedef ContactData CollisionData;

#endif
