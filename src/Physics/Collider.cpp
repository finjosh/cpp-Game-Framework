#include "Physics/Collider.hpp"
#include "Physics/CollisionManager.hpp"
#include "Physics/WorldHandler.hpp"

Collider::Collider()
{
    Object::m_onDisabled(&Collider::m_updatePhysicsState, this);
    Object::m_onEnabled(&Collider::m_updatePhysicsState, this);
    Object::m_onDestroyQueued([this](){
        this->setPhysicsEnabled(false);
    });
    Object::m_onTransformUpdated(&Collider::m_updateTransform, this);

    CollisionManager::addCollider(this);

    // initializing the body in box2d
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2)Object::getPosition();
    bodyDef.angle = Object::getRotation().getAngle();
    m_body = b2CreateBody(WorldHandler::getWorld(), &bodyDef);
    b2Body_SetUserData(m_body, (void*)this);
}

Collider::~Collider()
{
    b2DestroyBody(m_body); // No need to delete user data as it just points to this collider
    CollisionManager::removeCollider(this);
}

Fixture Collider::createFixture(const Fixture::Shape::Circle& shape, const FixtureDef& fixtureDef)
{
    return Fixture{b2CreateCircleShape(m_body, &fixtureDef.m_shapeDef, &shape.m_shape)};
}

Fixture Collider::createFixtureSensor(const Fixture::Shape::Circle& shape, FixtureDef fixtureDef)
{
    fixtureDef.setAsSensor(true);
    return Fixture{b2CreateCircleShape(m_body, &fixtureDef.m_shapeDef, &shape.m_shape)};
}

Fixture Collider::createFixture(const Fixture::Shape::Capsule& shape, const FixtureDef& fixtureDef)
{
    return Fixture{b2CreateCapsuleShape(m_body, &fixtureDef.m_shapeDef, &shape.m_shape)};
}

Fixture Collider::createFixtureSensor(const Fixture::Shape::Capsule& shape, FixtureDef fixtureDef)
{
    fixtureDef.setAsSensor(true);
    return Fixture{b2CreateCapsuleShape(m_body, &fixtureDef.m_shapeDef, &shape.m_shape)};
}

Fixture Collider::createFixture(const Fixture::Shape::Segment& shape, const FixtureDef& fixtureDef)
{
    return Fixture{b2CreateSegmentShape(m_body, &fixtureDef.m_shapeDef, &shape.m_shape)};
}

Fixture Collider::createFixtureSensor(const Fixture::Shape::Segment& shape, FixtureDef fixtureDef)
{
    fixtureDef.setAsSensor(true);
    return Fixture{b2CreateSegmentShape(m_body, &fixtureDef.m_shapeDef, &shape.m_shape)};
}

Fixture Collider::createFixture(const Fixture::Shape::Polygon& shape, const FixtureDef& fixtureDef)
{
    return Fixture{b2CreatePolygonShape(m_body, &fixtureDef.m_shapeDef, &shape.m_shape)};
}

Fixture Collider::createFixtureSensor(const Fixture::Shape::Polygon& shape, FixtureDef fixtureDef)
{
    fixtureDef.setAsSensor(true);
    return Fixture{b2CreatePolygonShape(m_body, &fixtureDef.m_shapeDef, &shape.m_shape)};
}


// Fixture Collider::createFixture(const b2Shape& shape, float friction, float restitution, float restitutionThreshold, 
//                       float density, const b2Filter& filter)
// {
//     b2FixtureDef temp;
//     temp.density = density;
//     temp.friction = friction;
//     temp.restitution = restitution;
//     temp.restitutionThreshold = restitutionThreshold;
//     temp.filter = filter;
//     temp.shape = &shape;
//     return Fixture(this, temp);
// }

// Fixture Collider::createFixtureSensor(const b2Shape& shape, float density, const b2Filter& filter)
// {
//     b2FixtureDef temp;
//     temp.density = density;
//     temp.filter = filter;
//     temp.shape = &shape;
//     temp.isSensor = true;
//     return Fixture(this, temp);
// }

void Collider::destroyFixture(const Fixture& fixture)
{
    b2DestroyShape(fixture.m_fixture);
}

void Collider::setPhysicsEnabled(bool enabled)
{
    if (this->isPhysicsEnabled() == enabled)
        return;
    m_enabled = enabled;
    m_updatePhysicsState();
}

bool Collider::isPhysicsEnabled() const
{
    return m_enabled && Object::isEnabled();
}

void Collider::m_updatePhysicsState()
{
    if (CollisionManager::m_inPhysicsUpdate)
    {
        if (this->isPhysicsEnabled())
            CollisionManager::m_updateBodyEvent(b2Body_Enable, this->m_body);
        else
            CollisionManager::m_updateBodyEvent(b2Body_Disable, this->m_body);
    }
    else
    {
        if (this->isPhysicsEnabled())
            b2Body_Enable(m_body);
        else
            b2Body_Disable(m_body);
    }
}

void Collider::m_updateTransform()
{
    // This could lead to slow downs since we are using lots of trig functions here
    b2Body_SetTransform(m_body, (b2Vec2)Object::getGlobalPosition(), Object::getGlobalRotation().getAngle() /*using atan2 then cos and sin*/); 
}

void Collider::m_update()
{
    // Object::m_onTransformUpdated.setEnabled(false);
    Object::setGlobalTransform(b2Body_GetTransform(m_body)); // Note this could lead to slow downs due to the callback which results in 3 trig function calls (also required for other class)
    // Object::m_onTransformUpdated.setEnabled(true);
}

float Collider::getInertiaTensor() const
{
    return b2Body_GetInertiaTensor(m_body);
}

Vector2 Collider::getLocalCenterOfMass() const
{
    return Vector2{b2Body_GetLocalCenterOfMass(m_body)};
}

Vector2 Collider::getGlobalCenterOfMass() const
{
    return Vector2{b2Body_GetWorldCenterOfMass(m_body)};
}

void Collider::applyMassFromShapes()
{
    b2Body_ApplyMassFromShapes(m_body);
}

void Collider::setAutomaticMass(bool automaticMass)
{
    b2Body_SetAutomaticMass(m_body, automaticMass);
}

bool Collider::getAutomaticMass() const
{
    return b2Body_GetAutomaticMass(m_body);
}

void Collider::setSleepThreshold(float sleepVelocity)
{
    b2Body_SetSleepThreshold(m_body, sleepVelocity);
}

float Collider::getSleepThreshold() const
{
    return b2Body_GetSleepThreshold(m_body);
}

void Collider::enableHitEvents(bool enableHitEvents)
{
    b2Body_EnableHitEvents(m_body, enableHitEvents);
}

void Collider::setSleepingEnabled(bool enabled)
{
    b2Body_EnableSleep(m_body, enabled);
}

bool Collider::isSleepingEnabled() const
{
    return b2Body_IsSleepEnabled(m_body);
}

int Collider::getFixtureCount() const
{
    return b2Body_GetShapeCount(m_body);
}

FixtureArray Collider::getFixtureArray(int maxSize)
{
    FixtureArray rtn(maxSize > b2Body_GetShapeCount(m_body) ? b2Body_GetShapeCount(m_body) : maxSize);
    b2Body_GetShapes(m_body, rtn.getShapeArray(), rtn.getSize());
    return rtn;
}

FixtureArray Collider::getFixtureArray(int maxSize) const
{
    FixtureArray rtn(maxSize > b2Body_GetShapeCount(m_body) ? b2Body_GetShapeCount(m_body) : maxSize);
    b2Body_GetShapes(m_body, rtn.getShapeArray(), rtn.getSize());
    return rtn;
}

b2AABB Collider::computeAABB() const
{
    return b2Body_ComputeAABB(m_body);
}

void Collider::setAwake(bool awake)
{
    if (CollisionManager::m_inPhysicsUpdate)
        CollisionManager::m_updateBodyEvent(b2Body_SetAwake, this->m_body, awake);
    else
        b2Body_SetAwake(m_body, awake);
}

void Collider::setLinearVelocity(const Vector2& v)
{
    b2Body_SetLinearVelocity(m_body, (b2Vec2)v);
}

Vector2 Collider::getLinearVelocity() const
{
    return b2Body_GetLinearVelocity(m_body);
}

void Collider::setAngularVelocity(float omega)
{
    b2Body_SetAngularVelocity(m_body, omega);
}

float Collider::getAngularVelocity() const
{
    return b2Body_GetAngularVelocity(m_body);
}

void Collider::applyForce(const Vector2& force, const Vector2& point, bool wake)
{
    b2Body_ApplyForce(m_body, (b2Vec2)force, (b2Vec2)point, wake);
}

void Collider::applyForceToCenter(const Vector2& force, bool wake)
{
    b2Body_ApplyForceToCenter(m_body, (b2Vec2)force, wake);
}

void Collider::applyTorque(float torque, bool wake)
{
    b2Body_ApplyTorque(m_body, torque, wake);
}

void Collider::applyLinearImpulse(const Vector2& impulse, const Vector2& point, bool wake)
{
    b2Body_ApplyLinearImpulse(m_body, (b2Vec2)impulse, (b2Vec2)point, wake);
}

void Collider::applyLinearImpulseToCenter(const Vector2& impulse, bool wake)
{
    b2Body_ApplyLinearImpulseToCenter(m_body, (b2Vec2)impulse, wake);
}

void Collider::applyAngularImpulse(float impulse, bool wake)
{
    b2Body_ApplyAngularImpulse(m_body, impulse, wake);
}

float Collider::getMass() const
{
    return b2Body_GetMass(m_body);
}

b2MassData Collider::getMassData() const
{
    return b2Body_GetMassData(m_body);
}

void Collider::setMassData(const b2MassData& data)
{
    b2Body_SetMassData(m_body, data);
}

float Collider::getLinearDamping() const
{
    return b2Body_GetLinearDamping(m_body);
}

void Collider::setLinearDamping(float linearDamping)
{
    b2Body_SetLinearDamping(m_body, linearDamping);
}

float Collider::getAngularDamping() const
{
    return b2Body_GetAngularDamping(m_body);
}

void Collider::setAngularDamping(float angularDamping)
{
    b2Body_SetAngularDamping(m_body, angularDamping);
}

float Collider::getGravityScale() const
{
    return b2Body_GetGravityScale(m_body);
}

void Collider::setGravityScale(float scale)
{
    b2Body_SetAngularDamping(m_body, scale);
}

void Collider::setType(b2BodyType type)
{
    b2Body_SetType(m_body, type);
}

b2BodyType Collider::getType() const
{
    return b2Body_GetType(m_body);
}

void Collider::setBullet(bool flag)
{
    b2Body_SetBullet(m_body, flag);
}

bool Collider::isBullet() const
{
    return b2Body_IsBullet(m_body);
}

bool Collider::isAwake() const
{
    return b2Body_IsAwake(m_body);
}

void Collider::setFixedRotation(bool flag)
{
    b2Body_SetFixedRotation(m_body, flag);
}

bool Collider::isFixedRotation() const
{
    return b2Body_IsFixedRotation(m_body);
}

Transform Collider::getInterpolatedTransform() const
{
    return Transform{Object::getPosition() + WorldHandler::getInterpolationTime() * b2Body_GetLinearVelocity(m_body), Object::getRotation() + b2Body_GetAngularVelocity(m_body) * WorldHandler::getInterpolationTime()};
}

//* Collision Data

ContactData::ContactData(Collider* collider, b2ShapeId thisFixture, b2ShapeId otherFixture) : 
    m_collider(collider), m_thisShape(thisFixture), m_otherShape(otherFixture) {}

Collider* ContactData::getCollider()
{
    return m_collider;
}

const Collider* ContactData::getCollider() const
{
    return m_collider;
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

// ContactData::Info ContactData::getInfo() const
// {
//     return Info(m_manifold);
// }

// ContactData::Info::Info(b2Contact* contact)
// {
//     contact->GetWorldManifold(&m_data);
//     m_points = contact->GetManifold()->pointCount;
// }

// int32 ContactData::Info::getPointCount() const
// {
//     return m_points;
// }

// const Vector2 ContactData::Info::getContactPoint(uint8 index) const
// {
//     return Vector2{m_data.points[index]};
// }

// const float ContactData::Info::getSeparations(uint8 index) const
// {
//     return m_data.separations[index];
// }

// Vector2 ContactData::Info::getNormal() const
// {
//     return m_data.normal;
// }

//* Hit Data

HitData::HitData(Collider* collider, b2ShapeId thisFixture, b2ShapeId otherFixture, const b2ContactHitEvent* m_hitData) 
    : m_collider(collider), m_thisShape(thisFixture), m_otherShape(otherFixture), m_hitData(m_hitData) {}

Collider* HitData::getCollider()
{ return m_collider; }

const Collider* HitData::getCollider() const
{ return m_collider; }

Fixture HitData::getThisFixture()
{ return Fixture{m_thisShape}; }

const Fixture HitData::getThisFixture() const
{ return Fixture{m_thisShape}; }

Fixture HitData::getOtherFixture()
{ return Fixture{m_otherShape}; }

const Fixture HitData::getOtherFixture() const
{ return Fixture{m_otherShape}; }

Vector2 HitData::getContactPoint() const
{ return Vector2{m_hitData->point}; }

Vector2 HitData::getContactNormal() const
{ return Vector2{m_hitData->normal}; }

float HitData::getApproachSpeed() const
{ return m_hitData->approachSpeed; }

//* Pre Solve Data

PreSolveData::PreSolveData(Collider* collider, b2ShapeId thisFixture, b2ShapeId otherFixture, b2Manifold* contactData) : 
    m_collider(collider), m_thisShape(thisFixture), m_otherShape(otherFixture), m_manifold(contactData) {}

const Collider* PreSolveData::getCollider() const
{
    return m_collider;
}

Collider* PreSolveData::getNoneConstCollider()
{
    return m_collider;
}

const Fixture PreSolveData::getThisFixture() const
{
    return Fixture(m_thisShape);
}

const Fixture PreSolveData::getOtherFixture() const
{
    return Fixture(m_otherShape);
}

void PreSolveData::destroyCollider()
{
    m_collider->destroy();
}

// ContactData::Info PreSolveData::getInfo() const
// {
//     return ContactData::Info(m_manifold);
// }

// bool PreSolveData::isTouching() const
// {
//     return m_manifold->IsTouching();
// }

// void PreSolveData::setEnabled(bool flag)
// {
//     m_manifold->SetEnabled(flag);
// }

// bool PreSolveData::isEnabled() const
// {
//     return m_manifold->IsEnabled();
// }

// void PreSolveData::setFriction(float friction)
// {
//     m_manifold->SetFriction(friction);
// }

// float PreSolveData::getFriction() const
// {
//     return m_manifold->GetFriction();
// }

// void PreSolveData::resetFriction()
// {
//     m_manifold->ResetFriction();
// }

// void PreSolveData::setRestitution(float restitution)
// {
//     m_manifold->SetRestitution(restitution);
// }

// float PreSolveData::getRestitution() const
// {
//     return m_manifold->GetRestitution();
// }

// void PreSolveData::resetRestitution()
// {
//     m_manifold->ResetRestitution();
// }

// void PreSolveData::setRestitutionThreshold(float threshold)
// {
//     m_manifold->SetRestitutionThreshold(threshold);
// }

// float PreSolveData::getRestitutionThreshold() const
// {
//     return m_manifold->GetRestitutionThreshold();
// }

// void PreSolveData::resetRestitutionThreshold()
// {
//     m_manifold->ResetRestitutionThreshold();
// }

// void PreSolveData::setTangentSpeed(float speed)
// {
//     m_manifold->SetTangentSpeed(speed);
// }

// float PreSolveData::getTangentSpeed() const
// {
//     return m_manifold->GetTangentSpeed();
// }
