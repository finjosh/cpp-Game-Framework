#include "Physics/Collider.hpp"
#include "Physics/CollisionManager.hpp"
#include "Physics/WorldHandler.hpp"

#ifdef _DEBUG
#define CHECK_IF_IN_PHYSICS_UPDATE(note) assert(WorldHandler::get()->isInPhysicsUpdate() == 0 && note)
#define CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA() CHECK_IF_IN_PHYSICS_UPDATE("Cannot edit any physics data while in a physics update")
#else
#define CHECK_IF_IN_PHYSICS_UPDATE(note)
#define CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA()
#endif

Collider::Collider()
{
    Object::m_onDisabled(&Collider::m_updatePhysicsState, this);
    Object::m_onEnabled(&Collider::m_updatePhysicsState, this);
    Object::m_onDestroyQueued([this](){
        CHECK_IF_IN_PHYSICS_UPDATE("Cannot destroy collider while in physics update");
        this->setPhysicsEnabled(false);
    });
    Object::m_onTransformUpdated(&Collider::m_updateTransform, this);

    CollisionManager::get()->addCollider(this);

    // initializing the body in box2d
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2)Object::getPosition();
    bodyDef.angle = Object::getRotation().getAngle();
    m_body = b2CreateBody(WorldHandler::get()->getWorld(), &bodyDef);
    b2Body_SetUserData(m_body, (void*)this);
}

Collider::~Collider()
{
    b2DestroyBody(m_body); // No need to delete user data as it just points to this collider
    CollisionManager::get()->removeCollider(this);
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

void Collider::destroyFixture(const Fixture& fixture)
{
    b2DestroyShape(fixture.m_fixture);
}

void Collider::setPhysicsEnabled(bool enabled)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
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
    CHECK_IF_IN_PHYSICS_UPDATE("Cannot enabled/disable any objects that have a collider during a physics update\nMaybe you meant to return true/false to enable/disable the current collision in the PreSolve callback?");
    if (this->isPhysicsEnabled())
        b2Body_Enable(m_body);
    else
        b2Body_Disable(m_body);
}

void Collider::m_updateTransform()
{
    // This could lead to slow downs since we are using lots of trig functions here
    b2Body_SetTransform(m_body, (b2Vec2)Object::getGlobalPosition(), Object::getGlobalRotation().getAngle() /*using atan2 then cos and sin*/); 
}

void Collider::m_update(b2Transform* transform)
{
    // Object::m_onTransformUpdated.setEnabled(false);
    Object::setGlobalTransform(*transform); // Note this could lead to slow downs due to the callback which results in 3 trig function calls (also required for other class)
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
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_SetSleepThreshold(m_body, sleepVelocity);
}

float Collider::getSleepThreshold() const
{
    return b2Body_GetSleepThreshold(m_body);
}

void Collider::setSleepingEnabled(bool enabled)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
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
    b2Body_GetShapes(m_body, rtn.getShapeArray(), rtn.size());
    return rtn;
}

FixtureArray Collider::getFixtureArray(int maxSize) const
{
    FixtureArray rtn(maxSize > b2Body_GetShapeCount(m_body) ? b2Body_GetShapeCount(m_body) : maxSize);
    b2Body_GetShapes(m_body, rtn.getShapeArray(), rtn.size());
    return rtn;
}

int Collider::getContactsCount() const
{
    return b2Body_GetContactCapacity(m_body);
}

ContactDataArray Collider::getContacts(int maxSize)
{
    ContactDataArray rtn(maxSize > b2Body_GetContactCapacity(m_body) ? b2Body_GetContactCapacity(m_body) : maxSize, this);
    b2Body_GetContactData(m_body, rtn.getContactArray(), rtn.size());
    return rtn;
}

ContactDataArray Collider::getContacts(int maxSize) const
{
    ContactDataArray rtn(maxSize > b2Body_GetContactCapacity(m_body) ? b2Body_GetContactCapacity(m_body) : maxSize, this);
    b2Body_GetContactData(m_body, rtn.getContactArray(), rtn.size());
    return rtn;
}

b2AABB Collider::computeAABB() const
{
    return b2Body_ComputeAABB(m_body);
}

void Collider::setAwake(bool awake)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_SetAwake(m_body, awake);
}

void Collider::setLinearVelocity(const Vector2& v)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_SetLinearVelocity(m_body, (b2Vec2)v);
}

Vector2 Collider::getLinearVelocity() const
{
    return b2Body_GetLinearVelocity(m_body);
}

void Collider::setAngularVelocity(float omega)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_SetAngularVelocity(m_body, omega);
}

float Collider::getAngularVelocity() const
{
    return b2Body_GetAngularVelocity(m_body);
}

void Collider::applyForce(const Vector2& force, const Vector2& point, bool wake)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_ApplyForce(m_body, (b2Vec2)force, (b2Vec2)point, wake);
}

void Collider::applyForceToCenter(const Vector2& force, bool wake)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_ApplyForceToCenter(m_body, (b2Vec2)force, wake);
}

void Collider::applyTorque(float torque, bool wake)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_ApplyTorque(m_body, torque, wake);
}

void Collider::applyLinearImpulse(const Vector2& impulse, const Vector2& point, bool wake)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_ApplyLinearImpulse(m_body, (b2Vec2)impulse, (b2Vec2)point, wake);
}

void Collider::applyLinearImpulseToCenter(const Vector2& impulse, bool wake)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_ApplyLinearImpulseToCenter(m_body, (b2Vec2)impulse, wake);
}

void Collider::applyAngularImpulse(float impulse, bool wake)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
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
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_SetMassData(m_body, data);
}

float Collider::getLinearDamping() const
{
    return b2Body_GetLinearDamping(m_body);
}

void Collider::setLinearDamping(float linearDamping)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_SetLinearDamping(m_body, linearDamping);
}

float Collider::getAngularDamping() const
{
    return b2Body_GetAngularDamping(m_body);
}

void Collider::setAngularDamping(float angularDamping)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_SetAngularDamping(m_body, angularDamping);
}

float Collider::getGravityScale() const
{
    return b2Body_GetGravityScale(m_body);
}

void Collider::setGravityScale(float scale)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_SetAngularDamping(m_body, scale);
}

void Collider::setType(b2BodyType type)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_SetType(m_body, type);
}

b2BodyType Collider::getType() const
{
    return b2Body_GetType(m_body);
}

void Collider::setBullet(bool flag)
{
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
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
    CHECK_IF_IN_PHYSICS_UPDATE_EDITING_DATA();
    b2Body_SetFixedRotation(m_body, flag);
}

bool Collider::isFixedRotation() const
{
    return b2Body_IsFixedRotation(m_body);
}

Transform Collider::getInterpolatedTransform() const
{
    return Transform{Object::getPosition() + WorldHandler::get()->getInterpolationTime() * b2Body_GetLinearVelocity(m_body), Object::getRotation() + b2Body_GetAngularVelocity(m_body) * WorldHandler::get()->getInterpolationTime()};
}
