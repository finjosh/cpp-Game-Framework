#include "Physics/Collider.hpp"
#include "Physics/CollisionManager.hpp"
#include "Physics/WorldHandler.hpp"

Collider::Collider()
{
    Object::m_onDisabled(&Collider::m_updatePhysicsState, this);
    Object::m_onEnabled(&Collider::m_updatePhysicsState, this);
    Object::onDestroy(&Collider::m_destroyBody, this);
    Object::m_onTransformUpdated(&Collider::m_updateTransform, this);
    // Object::_onParentRemoved(&CollisionManager::addCollider, this);
    // Object::_onParentSet(&CollisionManager::removeCollider, this);

    CollisionManager::addCollider(this);

    // initializing the body in box2d
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2)Object::getPosition();
    bodyDef.angle = Object::getRotation().getAngle();
    m_body = WorldHandler::getWorld().CreateBody(&bodyDef);
    m_body->GetUserData().pointer = (uintptr_t)this;
}

Collider::~Collider()
{
    CollisionManager::removeCollider(this);
}

void Collider::m_destroyBody()
{
    WorldHandler::getWorld().DestroyBody(m_body); // No need to delete user data as it just points to this collider
    // m_body = nullptr;
}

Fixture Collider::createFixture(const b2Shape& shape, float friction, float restitution, float restitutionThreshold, 
                      float density, const b2Filter& filter)
{
    b2FixtureDef temp;
    temp.density = density;
    temp.friction = friction;
    temp.restitution = restitution;
    temp.restitutionThreshold = restitutionThreshold;
    temp.filter = filter;
    temp.shape = &shape;
    return Fixture(this, temp);
}

Fixture Collider::createFixtureSensor(const b2Shape& shape, float density, const b2Filter& filter)
{
    b2FixtureDef temp;
    temp.density = density;
    temp.filter = filter;
    temp.shape = &shape;
    temp.isSensor = true;
    return Fixture(this, temp);
}

void Collider::destroyFixture(const Fixture& fixture)
{
    m_body->DestroyFixture(fixture.m_fixture);
}

void Collider::setPhysicsEnabled(bool enabled)
{
    m_enabled = enabled;
    m_body->SetEnabled(m_enabled && Object::isEnabled());
}

bool Collider::isPhysicsEnabled() const
{
    return m_enabled && Object::isEnabled();
}

void Collider::m_updatePhysicsState()
{
    m_body->SetEnabled(m_enabled && Object::isEnabled());
}

void Collider::m_updateTransform()
{
    m_body->SetTransform((b2Vec2)Object::getGlobalPosition(), Object::getGlobalRotation().getAngle() /*using atan2 then cos and sin*/); // This could lead to slow downs since we are using lots of trig functions here
}

void Collider::m_update()
{
    // Object::m_onTransformUpdated.setEnabled(false);
    Object::setGlobalTransform(m_body->GetTransform()); // Note this could lead to slow downs due to the callback which results in 3 trig function calls (also required for other class)
    // Object::m_onTransformUpdated.setEnabled(true);
}

void Collider::setAwake(bool awake)
{
    m_body->SetAwake(awake);
}

Vector2 Collider::getWorldCenter() const
{
    return m_body->GetWorldCenter();
}

Vector2 Collider::getLocalCenter() const
{
    return m_body->GetLocalCenter();
}

void Collider::setLinearVelocity(const Vector2& v)
{
    m_body->SetLinearVelocity((b2Vec2)v);
}

Vector2 Collider::getLinearVelocity() const
{
    return m_body->GetLinearVelocity();
}

void Collider::setAngularVelocity(float omega)
{
    m_body->SetAngularVelocity(omega);
}

float Collider::getAngularVelocity() const
{
    return m_body->GetAngularVelocity();
}

void Collider::applyForce(const Vector2& force, const Vector2& point, bool wake)
{
    m_body->ApplyForce((b2Vec2)force, (b2Vec2)point, wake);
}

void Collider::applyForceToCenter(const Vector2& force, bool wake)
{
    m_body->ApplyForceToCenter((b2Vec2)force, wake);
}

void Collider::applyTorque(float torque, bool wake)
{
    m_body->ApplyTorque(torque, wake);
}

void Collider::applyLinearImpulse(const Vector2& impulse, const Vector2& point, bool wake)
{
    m_body->ApplyLinearImpulse((b2Vec2)impulse, (b2Vec2)point, wake);
}

void Collider::applyLinearImpulseToCenter(const Vector2& impulse, bool wake)
{
    m_body->ApplyLinearImpulseToCenter((b2Vec2)impulse, wake);
}

void Collider::applyAngularImpulse(float impulse, bool wake)
{
    m_body->ApplyAngularImpulse(impulse, wake);
}

float Collider::getMass() const
{
    return m_body->GetMass();
}

float Collider::getInertia() const
{
    return m_body->GetInertia();
}

b2MassData Collider::getMassData() const
{
    return m_body->GetMassData();
}

void Collider::setMassData(const b2MassData* data)
{
    m_body->SetMassData(data);
}

void Collider::resetMassData()
{
    m_body->ResetMassData();
}

float Collider::getLinearDamping() const
{
    return m_body->GetLinearDamping();
}

void Collider::setLinearDamping(float linearDamping)
{
    m_body->SetLinearDamping(linearDamping);
}

float Collider::getAngularDamping() const
{
    return m_body->GetAngularDamping();
}

void Collider::setAngularDamping(float angularDamping)
{
    m_body->SetAngularDamping(angularDamping);
}

float Collider::getGravityScale() const
{
    return m_body->GetGravityScale();
}

void Collider::setGravityScale(float scale)
{
    m_body->SetAngularDamping(scale);
}

void Collider::setType(b2BodyType type)
{
    m_body->SetType(type);
}

b2BodyType Collider::getType() const
{
    return m_body->GetType();
}

void Collider::setBullet(bool flag)
{
    m_body->SetBullet(flag);
}

bool Collider::isBullet() const
{
    return m_body->IsBullet();
}

void Collider::setSleepingAllowed(bool flag)
{
    m_body->SetSleepingAllowed(flag);
}

bool Collider::isSleepingAllowed() const
{
    return m_body->IsSleepingAllowed();
}

bool Collider::isAwake() const
{
    return m_body->IsAwake();
}

void Collider::setFixedRotation(bool flag)
{
    m_body->SetFixedRotation(flag);
}

bool Collider::isFixedRotation() const
{
    return m_body->IsFixedRotation();
}

Fixture Collider::getFixtureList()
{
    return Fixture(m_body->GetFixtureList());
}

Transform Collider::getInterpolatedTransform() const
{
    return Transform{Object::getPosition() + WorldHandler::getInterpolationTime() * m_body->GetLinearVelocity(), Object::getRotation() + m_body->GetAngularVelocity() * WorldHandler::getInterpolationTime()};
}

//* Collision Data

ContactData::ContactData(Collider* collider, b2Fixture* thisFixture, b2Fixture* otherFixture, b2Contact* contactData) : 
    m_collider(collider), m_thisFixture(thisFixture), m_otherFixture(otherFixture), m_contactData(contactData) {}

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
    return Fixture(m_thisFixture);
}

const Fixture ContactData::getThisFixture() const
{
    return Fixture(m_thisFixture);
}

Fixture ContactData::getOtherFixture()
{
    return Fixture(m_contactData->GetFixtureA());
}

const Fixture ContactData::getOtherFixture() const
{
    return Fixture(m_contactData->GetFixtureA());
}

bool ContactData::operator < (const ContactData& data) const
{
    return data.m_collider > this->m_collider || data.m_otherFixture > this->m_otherFixture || data.m_thisFixture > this->m_thisFixture;
}

bool ContactData::operator > (const ContactData& data) const
{
    return data.m_collider < this->m_collider || data.m_otherFixture < this->m_otherFixture || data.m_thisFixture < this->m_thisFixture;
}

bool ContactData::operator == (const ContactData& data) const
{
    return data.m_collider == this->m_collider && data.m_otherFixture == this->m_otherFixture && data.m_thisFixture == this->m_thisFixture;
}

bool ContactData::operator != (const ContactData& data) const
{
    return !(*this == data);
}

ContactData::Info ContactData::getInfo() const
{
    return Info(m_contactData);
}

ContactData::Info::Info(b2Contact* contact)
{
    contact->GetWorldManifold(&m_data);
    m_points = contact->GetManifold()->pointCount;
}

int32 ContactData::Info::getPointCount() const
{
    return m_points;
}

const Vector2 ContactData::Info::getContactPoint(uint8 index) const
{
    return Vector2{m_data.points[index]};
}

const float ContactData::Info::getSeparations(uint8 index) const
{
    return m_data.separations[index];
}

Vector2 ContactData::Info::getNormal() const
{
    return m_data.normal;
}

//* Pre Solve Data

PreSolveData::PreSolveData(Collider* collider, b2Fixture* thisFixture, b2Fixture* otherFixture, b2Contact* contactData) : 
    m_collider(collider), m_thisFixture(thisFixture), m_otherFixture(otherFixture), m_contactData(contactData) {}

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
    return Fixture(m_thisFixture);
}

const Fixture PreSolveData::getOtherFixture() const
{
    return Fixture(m_contactData->GetFixtureA());
}

ContactData::Info PreSolveData::getInfo() const
{
    return ContactData::Info(m_contactData);
}

bool PreSolveData::isTouching() const
{
    return m_contactData->IsTouching();
}

void PreSolveData::setEnabled(bool flag)
{
    m_contactData->SetEnabled(flag);
}

bool PreSolveData::isEnabled() const
{
    return m_contactData->IsEnabled();
}

void PreSolveData::setFriction(float friction)
{
    m_contactData->SetFriction(friction);
}

float PreSolveData::getFriction() const
{
    return m_contactData->GetFriction();
}

void PreSolveData::resetFriction()
{
    m_contactData->ResetFriction();
}

void PreSolveData::setRestitution(float restitution)
{
    m_contactData->SetRestitution(restitution);
}

float PreSolveData::getRestitution() const
{
    return m_contactData->GetRestitution();
}

void PreSolveData::resetRestitution()
{
    m_contactData->ResetRestitution();
}

void PreSolveData::setRestitutionThreshold(float threshold)
{
    m_contactData->SetRestitutionThreshold(threshold);
}

float PreSolveData::getRestitutionThreshold() const
{
    return m_contactData->GetRestitutionThreshold();
}

void PreSolveData::resetRestitutionThreshold()
{
    m_contactData->ResetRestitutionThreshold();
}

void PreSolveData::setTangentSpeed(float speed)
{
    m_contactData->SetTangentSpeed(speed);
}

float PreSolveData::getTangentSpeed() const
{
    return m_contactData->GetTangentSpeed();
}
