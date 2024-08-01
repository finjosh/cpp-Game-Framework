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

    CollisionManager::get().addCollider(this);

    // initializing the body in box2d
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2)Object::getPosition();
    bodyDef.angle = Object::getRotation().getAngle();
    m_body = b2CreateBody(WorldHandler::get().getWorld(), &bodyDef);
    b2Body_SetUserData(m_body, (void*)this);
}

Collider::~Collider()
{
    b2DestroyBody(m_body);
    CollisionManager::get().removeCollider(this);
}

Fixture Collider::createFixture(const b2Polygon& shape, float friction, float restitution, float restitutionThreshold, 
                      float density, const b2Filter& filter)
{
    b2ShapeDef temp = b2DefaultShapeDef();
    temp.density = density;
    temp.friction = friction;
    temp.restitution = restitution;
    temp.filter = filter;
    return Fixture(this, temp, shape);
}

Fixture Collider::createFixtureSensor(const b2Polygon& shape, float density, const b2Filter& filter)
{
    b2ShapeDef temp = b2DefaultShapeDef();
    temp.density = density;
    temp.filter = filter;
    temp.isSensor = true;
    return Fixture(this, temp, shape);
}

void Collider::destroyFixture(const Fixture& fixture)
{
    b2DestroyShape(fixture.m_shape);
}

void Collider::setPhysicsEnabled(bool enabled)
{
    m_enabled = enabled;
    m_updatePhysicsState();
}

bool Collider::isPhysicsEnabled() const
{
    return m_enabled && Object::isEnabled();
}

void Collider::m_updatePhysicsState()
{
    auto funcPointer = m_enabled && Object::isEnabled() ? b2Body_Enable : b2Body_Disable; // getting if we want to enable or disable this
    if (CollisionManager::get().m_inPhysicsUpdate)
        CollisionManager::get().m_updateBodyEvent(funcPointer, this->m_body);
    else
        funcPointer(m_body);
}

void Collider::m_updateTransform()
{
    b2Body_SetTransform(m_body, (b2Vec2)Object::getGlobalPosition(), Object::getGlobalRotation().getAngle() /*using atan2 then cos and sin*/); // This could lead to slow downs since we are using lots of trig functions here
}

void Collider::m_update(b2Transform transform)
{
    Object::m_onTransformUpdated.setEnabled(false); // TODO find a efficient way to do this without disabling the event
    Object::setGlobalTransform(transform); // Note this could lead to slow downs due to the callback which results in 3 trig function calls and box2d thinking the body moved (also required for other class so we cant disable the callback)
    Object::m_onTransformUpdated.setEnabled(true);
}

void Collider::setAwake(bool awake)
{
    if (CollisionManager::get().m_inPhysicsUpdate)
        CollisionManager::get().m_updateBodyEvent(b2Body_SetAwake, this->m_body, awake);
    else
        b2Body_SetAwake(m_body, awake);
}

Vector2 Collider::getWorldCenterOfMass() const
{
    return b2Body_GetWorldCenterOfMass(m_body);
}

Vector2 Collider::getLocalCenterOfMass() const
{
    return b2Body_GetLocalCenterOfMass(m_body);
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

float Collider::getInertia() const
{
    return b2Body_GetInertiaTensor(m_body);
}

b2MassData Collider::getMassData() const
{
    return b2Body_GetMassData(m_body);
}

void Collider::setMassData(b2MassData data)
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

void Collider::setSleepingEnabled(bool flag)
{
    b2Body_EnableSleep(m_body, flag);
}

bool Collider::isSleepingEnabled() const
{
    return b2Body_IsSleepEnabled(m_body);
}

void Collider::setSleepingThreshold(float velocity)
{
    b2Body_SetSleepThreshold(m_body, velocity);
}

float Collider::getSleepingThreshold() const
{
    return b2Body_GetSleepThreshold(m_body);
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

FixtureList Collider::getFixtureList()
{
    return FixtureList(m_body);
}

Transform Collider::getInterpolatedTransform() const
{
    return Transform{Object::getPosition() + getLinearVelocity() * WorldHandler::get().getInterpolationTime(), Object::getRotation() + getAngularVelocity() * WorldHandler::get().getInterpolationTime()};
}

//* Hit Data

HitData::HitData(Collider* otherCollider, b2ShapeId otherShape, b2ShapeId thisShape, Vector2 normal, Vector2 point, float approachSpeed) :
    m_collider(otherCollider), m_otherShape(otherShape), m_thisShape(thisShape), m_normal(normal), m_point(point), m_approachSpeed(approachSpeed) {}

Collider* HitData::getCollider()
{
    return m_collider;
}

const Collider* HitData::getCollider() const
{
    return m_collider;
}

Fixture HitData::getThisFixture()
{
    return Fixture{m_thisShape};
}

const Fixture HitData::getThisFixture() const
{
    return Fixture{m_thisShape};
}

Fixture HitData::getOtherFixture()
{
    return Fixture{m_otherShape};
}

const Fixture HitData::getOtherFixture() const
{
    return Fixture{m_otherShape};
}

Vector2 HitData::getContactPoint() const
{
    return m_point;
}

Vector2 HitData::getNormal() const
{
    return m_normal;
}

float HitData::getApproachSpeed() const
{
    return m_approachSpeed;
}

//* Collision Data

ContactData::ContactData(Collider* collider, b2ShapeId otherShape, b2ShapeId thisShape) : 
    m_collider(collider), m_otherShape(otherShape), m_thisShape(thisShape) {}

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

//* Pre Solve Data

PreSolveData::PreSolveData(Collider* collider, b2ShapeId thisFixture, b2ShapeId otherFixture, b2Manifold* manifold) : 
    m_collider(collider), m_thisFixture(thisFixture), m_otherFixture(otherFixture), m_manifold(manifold) {}

const Collider* PreSolveData::getCollider() const
{
    return m_collider;
}

Collider* PreSolveData::getCollider_NoneConst()
{
    return m_collider;
}

const Fixture PreSolveData::getThisFixture() const
{
    return Fixture(m_thisFixture);
}

const Fixture PreSolveData::getOtherFixture() const
{
    return Fixture(m_otherFixture);
}

void PreSolveData::destroyOtherCollider() const
{
    CollisionManager::get().m_updateBodyEvent(Collider::destroy, static_cast<Collider*>(b2Body_GetUserData(b2Shape_GetBody(m_otherFixture))));
}

void PreSolveData::destroyThisCollider() const
{
    CollisionManager::get().m_updateBodyEvent(Collider::destroy, m_collider);
}

std::list<Vector2> PreSolveData::getContactPoints() const
{
    std::list<Vector2> rtn;
    for (int i = 0; i < m_manifold->pointCount; i++)
        rtn.emplace_back(m_manifold->points[i].point);
    return rtn;
}

Vector2 PreSolveData::getNormal() const
{
    return m_manifold->normal;
}
