#include "Physics/ContactData.hpp"
#include "Physics/Collider.hpp"

//* Collision Data

#define GET_COLLIDER(b2Shape) ((Collider*)b2Body_GetUserData(b2Shape_GetBody(b2Shape)))

ContactData::ContactData(b2ShapeId thisFixture, b2ShapeId otherFixture, b2Manifold* manifold) : m_thisShape(thisFixture), m_otherShape(otherFixture), m_manifold(manifold) 
{
    assert(b2Shape_IsValid(thisFixture) && b2Shape_IsValid(otherFixture) && "Shapes must be valid");
    assert(manifold != nullptr && "Manifold must not be nullptr");
}

Collider* ContactData::getCollider()
{
    return GET_COLLIDER(m_thisShape);
}

const Collider* ContactData::getCollider() const
{
    return GET_COLLIDER(m_thisShape);
}

Fixture ContactData::getThisFixture()
{
    return Fixture(m_thisShape);
}

const Fixture ContactData::getThisFixture() const
{
    return Fixture(m_thisShape);
}

Fixture ContactData::getOtherFixture()
{
    return Fixture(m_otherShape);
}

const Fixture ContactData::getOtherFixture() const
{
    return Fixture(m_otherShape);
}

std::int32_t ContactData::getPointCount() const
{
    return m_manifold->pointCount;
}

const Vector2 ContactData::getContactPoint(std::int32_t index) const
{
    assert(index > 0 && index <= m_manifold->pointCount && "Index must be in the range [0,2]");
    return m_manifold->points[index].point; // TODO implement getter functions for the manifold point data
}

const float ContactData::getSeparations(std::int32_t index) const
{
    assert(index > 0 && index <= m_manifold->pointCount && "Index must be in the range [0,2]");
    return m_manifold->points[index].separation;
}

Vector2 ContactData::getNormal() const
{
    return m_manifold->normal;
}

//* Pre Solve Data

PreSolveData::PreSolveData(b2ShapeId thisFixture, b2ShapeId otherFixture, const b2Manifold* manifold, EventHelper::Event* updateBodyEvent)
    : m_thisShape(thisFixture), m_otherShape(otherFixture), m_manifold(manifold), m_updateBodyEvent(updateBodyEvent) 
{
    assert(b2Shape_IsValid(thisFixture) && b2Shape_IsValid(otherFixture) && "Shapes must be valid");
    assert(manifold != nullptr && "Manifold must not be nullptr");
    assert(updateBodyEvent != nullptr && "Update body event must not be nullptr");
}

const Collider* PreSolveData::getCollider() const
{
    return GET_COLLIDER(m_otherShape);
}

Collider* PreSolveData::getNoneConstCollider()
{
    return GET_COLLIDER(m_otherShape);
}

const Fixture PreSolveData::getThisFixture() const
{
    return Fixture(m_thisShape);
}

const Fixture PreSolveData::getOtherFixture() const
{
    return Fixture(m_otherShape);
}

void PreSolveData::destroyOtherCollider()
{
    m_updateBodyEvent->connect(&Collider::destroy, GET_COLLIDER(m_otherShape));
}

void PreSolveData::destroyThisCollider()
{
    m_updateBodyEvent->connect(&Collider::destroy, GET_COLLIDER(m_thisShape));
}

EventHelper::Event& PreSolveData::getUpdateEvent()
{
    return *m_updateBodyEvent;
}

std::int32_t PreSolveData::getPointCount() const
{
    return m_manifold->pointCount;
}

const Vector2 PreSolveData::getContactPoint(std::int32_t index) const
{
    assert(index > 0 && index <= m_manifold->pointCount && "Index must be in the range [0,2]");
    return m_manifold->points[index].point; // TODO implement getter functions for the manifold point data
}

const float PreSolveData::getSeparations(std::int32_t index) const
{
    assert(index > 0 && index <= m_manifold->pointCount && "Index must be in the range [0,2]");
    return m_manifold->points[index].separation;
}

Vector2 PreSolveData::getNormal() const
{
    return m_manifold->normal;
}
