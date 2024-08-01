#include "Physics/Fixture.hpp"
#include "Physics/Collider.hpp"

Fixture::Fixture(Collider* collider, const b2ShapeDef& shapeDef, const b2Polygon& polygonDef)
{
    if (collider)
    {
        b2CreatePolygonShape(collider->m_body, &shapeDef, &polygonDef);
    }
}

Fixture::Fixture(Collider& collider, const b2ShapeDef& shapeDef, const b2Polygon& polygonDef)
{
    b2CreatePolygonShape(collider.m_body, &shapeDef, &polygonDef);
}

bool Fixture::isValid() const
{
    return b2Shape_IsValid(m_shape);
}

bool Fixture::isSensor() const
{
    return b2Shape_IsSensor(m_shape);
}

void Fixture::setFilter(const b2Filter& filter)
{
    b2Shape_SetFilter(m_shape, filter);
}

b2Filter Fixture::getFilter() const
{
    return b2Shape_GetFilter(m_shape);
}

Collider* Fixture::getCollider()
{
    return static_cast<Collider*>(b2Body_GetUserData(b2Shape_GetBody(m_shape)));
}

const Collider* Fixture::getCollider() const
{
    return static_cast<Collider*>(b2Body_GetUserData(b2Shape_GetBody(m_shape)));
}

bool Fixture::testPoint(const Vector2& p) const
{
    return b2Shape_TestPoint(m_shape, (b2Vec2)p);
}

void Fixture::setDensity(float density)
{
    b2Shape_SetDensity(m_shape, density);
}

float Fixture::getDensity() const
{
    return b2Shape_GetDensity(m_shape);
}

float Fixture::getFriction() const
{
    return b2Shape_GetFriction(m_shape);
}

void Fixture::setFriction(float friction)
{
    b2Shape_SetFriction(m_shape, friction);
}

float Fixture::getRestitution() const
{
    return b2Shape_GetRestitution(m_shape);
}

void Fixture::setRestitution(float restitution)
{
    b2Shape_SetRestitution(m_shape, restitution);
}

void Fixture::destroy()
{
    b2DestroyShape(m_shape);
}

Fixture::Fixture(b2ShapeId shape) : m_shape(shape) {}

FixtureList::FixtureList(b2BodyId id)
{
    m_size = b2Body_GetShapeCount(id);
    if (m_size == 0)
        return;
    m_array = (b2ShapeId*)malloc(sizeof(b2ShapeId)*m_size);
    if (nullptr)
        throw std::runtime_error("Not enough memory to get fixture list");
    b2Body_GetShapes(id, m_array, m_size);
}

Fixture FixtureList::getFixture(int index) const
{
    if (!m_array || index >= m_size)
        return Fixture(b2_nullShapeId);

    return Fixture(m_array[index]);
}

Fixture FixtureList::getNextFixture()
{
    m_current++;
    return Fixture(m_array[m_current]);
}

Fixture FixtureList::getCurrentFixture() const
{
    return Fixture(m_array[m_current]);
}

int FixtureList::getSize() const
{
    return m_size;
}

int FixtureList::getCurrentIndex() const
{
    return m_current;
}

void FixtureList::setCurrentFixture(int index)
{
    m_current = index;
}
