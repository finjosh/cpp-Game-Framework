#include "Physics/Fixture.hpp"
#include "Physics/Collider.hpp"

Fixture::Fixture(Collider& collider, const FixtureDef& fixtureDef, const Shape::Circle& shape)
{
    m_fixture = b2CreateCircleShape(collider.m_body, &fixtureDef.m_shapeDef, &shape.m_shape);
}

Fixture::Fixture(Collider& collider, const FixtureDef& fixtureDef, const Shape::Capsule& shape)
{
    m_fixture = b2CreateCapsuleShape(collider.m_body, &fixtureDef.m_shapeDef, &shape.m_shape);
}

Fixture::Fixture(Collider& collider, const FixtureDef& fixtureDef, const Shape::Segment& shape)
{
    m_fixture = b2CreateSegmentShape(collider.m_body, &fixtureDef.m_shapeDef, &shape.m_shape);
    
}

Fixture::Fixture(Collider& collider, const FixtureDef& fixtureDef, const Shape::Polygon& shape)
{
    assert(shape.isValid() && "Polygon must be valid to create a fixture using it!");
    m_fixture = b2CreatePolygonShape(collider.m_body, &fixtureDef.m_shapeDef, &shape.m_shape);
}

Collider* Fixture::getCollider()
{
    return static_cast<Collider*>(b2Body_GetUserData(b2Shape_GetBody(m_fixture)));
}

const Collider* Fixture::getCollider() const
{
    return static_cast<Collider*>(b2Body_GetUserData(b2Shape_GetBody(m_fixture)));
}

bool Fixture::isValid() const
{
    return b2Shape_IsValid(m_fixture);
}

b2ShapeType Fixture::getType() const
{
    return b2Shape_GetType(m_fixture);
}

bool Fixture::isSensor()
{
    return b2Shape_IsSensor(m_fixture);
}

void Fixture::setDensity(float density, bool updateBodyProperties)
{
    b2Shape_SetDensity(m_fixture, density);
    if (updateBodyProperties)
        b2Body_ApplyMassFromShapes(b2Shape_GetBody(m_fixture));
}

float Fixture::getDensity() const
{
    return b2Shape_GetDensity(m_fixture);
}

void Fixture::setFriction(float friction)
{
    b2Shape_SetFriction(m_fixture, friction);
}

float Fixture::getFriction() const
{
    return b2Shape_GetFriction(m_fixture);
}

void Fixture::setRestitution(float restitution)
{
    b2Shape_SetRestitution(m_fixture, restitution);
}

float Fixture::getRestitution() const
{
    return b2Shape_GetRestitution(m_fixture);
}

b2Filter Fixture::getFilter() const
{
    return b2Shape_GetFilter(m_fixture);
}

void Fixture::setFilter(b2Filter filter)
{
    b2Shape_SetFilter(m_fixture, filter);
}

void Fixture::enableSensorEvents(bool enabled)
{
    b2Shape_EnableSensorEvents(m_fixture, enabled);
}

bool Fixture::areSensorEventsEnabled()
{
    return b2Shape_AreSensorEventsEnabled(m_fixture);
}

void Fixture::enableContactEvents(bool enabled)
{
    b2Shape_EnableContactEvents(m_fixture, enabled);
}

bool Fixture::areContactEventsEnabled()
{
    return b2Shape_AreContactEventsEnabled(m_fixture);
}

void Fixture::enablePreSolveEvents(bool enabled)
{
    b2Shape_EnablePreSolveEvents(m_fixture, enabled);
}

bool Fixture::arePreSolveEventsEnabled()
{
    return b2Shape_ArePreSolveEventsEnabled(m_fixture);
}

void Fixture::enableHitEvents(bool enabled)
{
    b2Shape_EnableHitEvents(m_fixture, enabled);
}

bool Fixture::areHitEventsEnabled()
{
    return b2Shape_AreHitEventsEnabled(m_fixture);
}

bool Fixture::testPoint(Vector2 point)
{
    return b2Shape_TestPoint(m_fixture, (b2Vec2)point);
}

b2CastOutput Fixture::rayCast(Vector2 origin, Vector2 translation)
{
    return b2Shape_RayCast(m_fixture, (b2Vec2)origin, (b2Vec2)translation);
}

Fixture::Shape::Circle Fixture::getCircle() const
{
    return b2Shape_GetCircle(m_fixture);
}

Fixture::Shape::Segment Fixture::getSegment() const
{
    return b2Shape_GetSegment(m_fixture);
}

// b2SmoothSegment Fixture::getSmoothSegment() const
// {
//     return b2Shape_GetSmoothSegment(m_fixture);
// }

Fixture::Shape::Capsule Fixture::getCapsule() const
{
    return b2Shape_GetCapsule(m_fixture);
}

Fixture::Shape::Polygon Fixture::getPolygon() const
{
    return b2Shape_GetPolygon(m_fixture);
}

// Fixture::Shape::Chain Fixture::getChain() const
// {
//     assert(this->getType() == b2_smoothSegmentShape && "Fixture is must be a b2_smoothSegmentShape to get a chain");
//     return b2Shape_GetParentChain(m_fixture);
// }

void Fixture::setCircle(const Fixture::Shape::Circle& circle, bool updateBodyProperties)
{
    b2Shape_SetCircle(m_fixture, &circle.m_shape);
    if (updateBodyProperties)
        b2Body_ApplyMassFromShapes(b2Shape_GetBody(m_fixture));
}

void Fixture::setCapsule(const Fixture::Shape::Capsule& capsule, bool updateBodyProperties)
{
    b2Shape_SetCapsule(m_fixture, &capsule.m_shape);
    if (updateBodyProperties)
        b2Body_ApplyMassFromShapes(b2Shape_GetBody(m_fixture));
}

void Fixture::setSegment(const Fixture::Shape::Segment& segment, bool updateBodyProperties)
{
    b2Shape_SetSegment(m_fixture, &segment.m_shape);
    if (updateBodyProperties)
        b2Body_ApplyMassFromShapes(b2Shape_GetBody(m_fixture));
}

void Fixture::setPolygon(const Fixture::Shape::Polygon& polygon, bool updateBodyProperties)
{
    b2Shape_SetPolygon(m_fixture, &polygon.m_shape);
    if (updateBodyProperties)
        b2Body_ApplyMassFromShapes(b2Shape_GetBody(m_fixture));
}

// b2ChainId Fixture::getParentChain()
// {
//     return b2Shape_GetParentChain(m_fixture);
// }

// int Fixture::getContactCapacity()
// {
//     return b2Shape_GetContactCapacity(m_fixture);
// }

// int Fixture::getContactData(b2ContactData* contactData, int capacity)
// {
//     return b2Shape_GetContactData(m_fixture, contactData, capacity);
// }

b2AABB Fixture::getAABB() const
{
    return b2Shape_GetAABB(m_fixture);
}

Vector2 Fixture::getClosestPoint(Vector2 target) const
{
    return b2Shape_GetClosestPoint(m_fixture, (b2Vec2)target);
}

void Fixture::destroy()
{
    b2DestroyShape(m_fixture);
    m_fixture = b2_nullShapeId;
}

Fixture::Fixture(b2ShapeId fixture) : m_fixture(fixture) {}