#include "Physics/Fixture.hpp"
#include "Physics/Collider.hpp"

//* Circle
Fixture::Shape::Circle::Circle() : m_shape({{0,0}, 1}) {}
Fixture::Shape::Circle::Circle(b2Circle circle) : m_shape(circle) {}
Fixture::Shape::Circle::Circle(Vector2 center, float radius) : m_shape({{(b2Vec2)center}, radius}) {}

Vector2 Fixture::Shape::Circle::getCenter() const { return (Vector2)m_shape.center; }
float Fixture::Shape::Circle::getRadius() const { return m_shape.radius; }

void Fixture::Shape::Circle::setCenter(Vector2 center) { m_shape.center = (b2Vec2)center; }
void Fixture::Shape::Circle::setRadius(float radius) { m_shape.radius = radius; }

//* Capsule
Fixture::Shape::Capsule::Capsule() : m_shape({{0,0}, {0,1}, 1}) {}
Fixture::Shape::Capsule::Capsule(b2Capsule capsule) : m_shape(capsule) {}
Fixture::Shape::Capsule::Capsule(Vector2 center1, Vector2 center2, float radius) : m_shape({{(b2Vec2)center1}, {(b2Vec2)center2}, radius}) {}

Vector2 Fixture::Shape::Capsule::getCenter1() const { return (Vector2)m_shape.center1; }
Vector2 Fixture::Shape::Capsule::getCenter2() const { return (Vector2)m_shape.center2; }
float Fixture::Shape::Capsule::getRadius() const { return m_shape.radius; }

void Fixture::Shape::Capsule::setCenter1(Vector2 center1) { m_shape.center1 = (b2Vec2)center1; }
void Fixture::Shape::Capsule::setCenter2(Vector2 center2) { m_shape.center2 = (b2Vec2)center2; }
void Fixture::Shape::Capsule::setRadius(float radius) { m_shape.radius = radius; }

//* Polygon::Hull
Fixture::Shape::Polygon::Hull::Hull(b2Vec2 points[], std::int32_t size) : m_hull(b2ComputeHull(points, size)) {}
Fixture::Shape::Polygon::Hull::Hull(b2Hull hull) : m_hull(hull) {}
bool Fixture::Shape::Polygon::Hull::isValid() const { return m_hull.count >= 3; }

//* Polygon
Fixture::Shape::Polygon::Polygon() { m_shape.count = 0; }
Fixture::Shape::Polygon::Polygon(float x, float y) : m_shape(b2MakeBox(x/2,y/2)) {}
Fixture::Shape::Polygon::Polygon(float x, float y, Vector2 center, Rotation rotation) : m_shape(b2MakeOffsetBox(x/2, y/2, (b2Vec2)center, (b2Rot)rotation)) {}
Fixture::Shape::Polygon::Polygon(const Hull& hull, float radius) 
{ 
    assert(hull.isValid() && "Hull must be valid to create Polygon!"); 
    m_shape = b2MakePolygon(&hull.m_hull, radius);
}
Fixture::Shape::Polygon::Polygon(const Hull& hull, Vector2 center, Rotation rotation)
{
    assert(hull.isValid() && "Hull must be valid to create Polygon!"); 
    m_shape = b2MakeOffsetPolygon(&hull.m_hull, (b2Vec2)center, (b2Rot)rotation);
}
Fixture::Shape::Polygon::Polygon(float x, float y, float radius) : m_shape(b2MakeRoundedBox(x/2, y/2, radius)) {}
Fixture::Shape::Polygon::Polygon(float sideLengths) : m_shape(b2MakeSquare(sideLengths/2)) {}
Fixture::Shape::Polygon::Polygon(b2Polygon polygon) : m_shape(polygon) {}

bool Fixture::Shape::Polygon::isValid() const { return m_shape.count >= 3; }

void Fixture::Shape::Polygon::makeBox(float x, float y) 
{ this->m_shape = b2MakeBox(x/2, y/2); }
void Fixture::Shape::Polygon::makeOffsetBox(float x, float y, Vector2 center, Rotation rotation) 
{ this->m_shape = b2MakeOffsetBox(x/2, y/2, (b2Vec2)center, (b2Rot)rotation); }
void Fixture::Shape::Polygon::makePolygon(const Hull& hull, float radius) 
{ 
    assert(hull.isValid() && "Hull must be valid to create Polygon!"); 
    m_shape = b2MakePolygon(&hull.m_hull, radius);
}
void Fixture::Shape::Polygon::makeOffsetPolygon(const Hull& hull, Vector2 center, Rotation rotation) 
{ 
    assert(hull.isValid() && "Hull must be valid to create Polygon!"); 
    m_shape = b2MakeOffsetPolygon(&hull.m_hull, (b2Vec2)center, (b2Rot)rotation);
}
void Fixture::Shape::Polygon::makeOffsetRoundedPolygon(const Hull& hull, float radius, Vector2 center, Rotation rotation) 
{ 
    assert(hull.isValid() && "Hull must be valid to create Polygon!"); 
    m_shape = b2MakeOffsetRoundedPolygon(&hull.m_hull, (b2Vec2)center, (b2Rot)rotation, radius);
}
void Fixture::Shape::Polygon::makeRoundedBox(float x, float y, float radius) 
{ this->m_shape = b2MakeRoundedBox(x/2, y/2, radius); }
void Fixture::Shape::Polygon::makeOffsetRoundedBox(float x, float y, float radius, Vector2 center, Rotation rotation) 
{ 
    this->m_shape = b2MakeOffsetBox(x/2, y/2, (b2Vec2)center, (b2Rot)rotation);
    this->m_shape.radius = radius;
}
void Fixture::Shape::Polygon::makeSquare(float sideLengths) 
{ this->m_shape = b2MakeSquare(sideLengths); }
void Fixture::Shape::Polygon::makeOffsetSquare(float sideLengths, Vector2 center, Rotation rotation) 
{ 
    this->m_shape = b2MakeOffsetBox(sideLengths/2, sideLengths/2, (b2Vec2)center, (b2Rot)rotation);
}

//* Segment
Fixture::Shape::Segment::Segment() : m_shape({{0,0}, {0,1}}) {}
Fixture::Shape::Segment::Segment(b2Segment segment) : m_shape(segment) {}
Fixture::Shape::Segment::Segment(Vector2 point1, Vector2 point2) : m_shape({{(b2Vec2)point1}, {(b2Vec2)point2}}) {}

void Fixture::Shape::Segment::setPoint1(Vector2 point1) { m_shape.point1 = (b2Vec2)point1; }
void Fixture::Shape::Segment::setPoint2(Vector2 point2) { m_shape.point2 = (b2Vec2)point2; }

Vector2 Fixture::Shape::Segment::getPoint1() const { return (Vector2)m_shape.point1; }
Vector2 Fixture::Shape::Segment::getPoint2() const { return (Vector2)m_shape.point2; }

//* Fixture
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

Fixture::Shape::Type Fixture::getType() const
{
    return (Fixture::Shape::Type)b2Shape_GetType(m_fixture);
}

bool Fixture::isSensor()
{
    return b2Shape_IsSensor(m_fixture);
}

void Fixture::setDensity(float density, bool updateBodyProperties)
{
    b2Shape_SetDensity(m_fixture, density, updateBodyProperties);
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

Filter Fixture::getFilter() const
{
    return Filter(b2Shape_GetFilter(m_fixture));
}

void Fixture::setFilter(Filter filter)
{
    b2Shape_SetFilter(m_fixture, filter.m_filter);
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

bool Fixture::testPoint(Vector2 point)
{
    return b2Shape_TestPoint(m_fixture, (b2Vec2)point);
}

b2CastOutput Fixture::rayCast(Vector2 origin, Vector2 translation, float maxFraction)
{
    b2RayCastInput input;
    input.origin = (b2Vec2)origin;
    input.translation = (b2Vec2)translation;
    input.maxFraction = 1.f;
    return b2Shape_RayCast(m_fixture, &input);
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

b2MassData Fixture::calculateMassData() const
{
    switch (b2Shape_GetType(m_fixture))
    {
    case b2_circleShape:
    {
        auto temp = b2Shape_GetCircle(m_fixture);
        return b2ComputeCircleMass(&temp, b2Shape_GetDensity(m_fixture));
    }
    
    case b2_capsuleShape:
    {
        auto temp = b2Shape_GetCapsule(m_fixture);
        return b2ComputeCapsuleMass(&temp, b2Shape_GetDensity(m_fixture));
    }

    case b2_polygonShape:
    {
        auto temp = b2Shape_GetPolygon(m_fixture);
        return b2ComputePolygonMass(&temp, b2Shape_GetDensity(m_fixture));
    }

    default:
        return b2MassData{0};
    }
}

float Fixture::calculateMass() const
{
    return this->calculateMassData().mass;
}

b2AABB Fixture::getAABB() const
{
    return b2Shape_GetAABB(m_fixture);
}

Vector2 Fixture::getClosestPoint(Vector2 target) const
{
    return b2Shape_GetClosestPoint(m_fixture, (b2Vec2)target);
}

void Fixture::destroy(bool updateMassData)
{
    b2DestroyShape(m_fixture, updateMassData);
    m_fixture = b2_nullShapeId;
}

Fixture::Fixture(b2ShapeId fixture) : m_fixture(fixture) {}