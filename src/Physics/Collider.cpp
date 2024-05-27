#include "Physics/Collider.hpp"
#include "Physics/CollisionManager.hpp"

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
    bodyDef.position = Object::getPosition();
    bodyDef.angle = Object::getRotation();
    m_body = WorldHandler::getWorld().CreateBody(&bodyDef);
    m_body->GetUserData().pointer = (uintptr_t)this;
}

Collider::~Collider()
{
    CollisionManager::removeCollider(this);
}

void Collider::m_destroyBody()
{
    WorldHandler::getWorld().DestroyBody(m_body);
    m_body = nullptr;
}

Fixture Collider::createFixture(const b2Shape& shape, const float& friction, const float& restitution, const float& restitutionThreshold, 
                      const float& density, const b2Filter& filter)
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

Fixture Collider::createFixtureSensor(const b2Shape& shape, const float& density, const b2Filter& filter)
{
    b2FixtureDef temp;
    temp.density = density;
    temp.filter = filter;
    temp.shape = &shape;
    temp.isSensor = true;
    return Fixture(this, temp);
}

void Collider::DestroyFixture(const Fixture& fixture)
{
    m_body->DestroyFixture(fixture.m_fixture);
}

void Collider::setPhysicsEnabled(const bool& enabled)
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
    m_body->SetTransform(Object::getPosition(), Object::getRotation());
}

void Collider::m_update()
{
    Object::setTransform(m_body->GetTransform()); //! NOTE this updates the object position which calls event to update this position (could result in errors later)
}

void Collider::setAwake(const bool& awake)
{
    m_body->SetAwake(awake);
}

const b2Vec2& Collider::GetWorldCenter() const
{
    return m_body->GetWorldCenter();
}

const b2Vec2& Collider::GetLocalCenter() const
{
    return m_body->GetLocalCenter();
}

void Collider::SetLinearVelocity(const b2Vec2& v)
{
    m_body->SetLinearVelocity(v);
}

const b2Vec2& Collider::GetLinearVelocity() const
{
    return m_body->GetLinearVelocity();
}

void Collider::SetAngularVelocity(const float& omega)
{
    m_body->SetAngularVelocity(omega);
}

float Collider::GetAngularVelocity() const
{
    return m_body->GetAngularVelocity();
}

void Collider::ApplyForce(const b2Vec2& force, const b2Vec2& point, const bool& wake)
{
    m_body->ApplyForce(force, point, wake);
}

void Collider::ApplyForceToCenter(const b2Vec2& force, const bool& wake)
{
    m_body->ApplyForceToCenter(force, wake);
}

void Collider::ApplyTorque(const float& torque, const bool& wake)
{
    m_body->ApplyTorque(torque, wake);
}

void Collider::ApplyLinearImpulse(const b2Vec2& impulse, const b2Vec2& point, const bool& wake)
{
    m_body->ApplyLinearImpulse(impulse, point, wake);
}

void Collider::ApplyLinearImpulseToCenter(const b2Vec2& impulse, const bool& wake)
{
    m_body->ApplyLinearImpulseToCenter(impulse, wake);
}

void Collider::ApplyAngularImpulse(const float& impulse, const bool& wake)
{
    m_body->ApplyAngularImpulse(impulse, wake);
}

float Collider::GetMass() const
{
    return m_body->GetMass();
}

float Collider::GetInertia() const
{
    return m_body->GetInertia();
}

b2MassData Collider::GetMassData() const
{
    return m_body->GetMassData();
}

void Collider::SetMassData(const b2MassData* data)
{
    m_body->SetMassData(data);
}

void Collider::ResetMassData()
{
    m_body->ResetMassData();
}

float Collider::GetLinearDamping() const
{
    return m_body->GetLinearDamping();
}

void Collider::SetLinearDamping(const float& linearDamping)
{
    m_body->SetLinearDamping(linearDamping);
}

float Collider::GetAngularDamping() const
{
    return m_body->GetAngularDamping();
}

void Collider::SetAngularDamping(const float& angularDamping)
{
    m_body->SetAngularDamping(angularDamping);
}

float Collider::GetGravityScale() const
{
    return m_body->GetGravityScale();
}

void Collider::SetGravityScale(const float& scale)
{
    m_body->SetAngularDamping(scale);
}

void Collider::SetType(const b2BodyType& type)
{
    m_body->SetType(type);
}

b2BodyType Collider::GetType() const
{
    return m_body->GetType();
}

void Collider::SetBullet(const bool& flag)
{
    m_body->SetBullet(flag);
}

bool Collider::IsBullet() const
{
    return m_body->IsBullet();
}

void Collider::SetSleepingAllowed(const bool& flag)
{
    m_body->SetSleepingAllowed(flag);
}

bool Collider::IsSleepingAllowed() const
{
    return m_body->IsSleepingAllowed();
}

bool Collider::IsAwake() const
{
    return m_body->IsAwake();
}

void Collider::SetFixedRotation(const bool& flag)
{
    m_body->SetFixedRotation(flag);
}

bool Collider::IsFixedRotation() const
{
    return m_body->IsFixedRotation();
}

Fixture Collider::GetFixtureList()
{
    return Fixture(m_body->GetFixtureList());
}

//* Collision Data

ContactData::ContactData(Collider* collider, b2Fixture* thisFixture, b2Fixture* otherFixture) : m_collider(collider), m_thisFixture(thisFixture), m_otherFixture(otherFixture) {}

Collider* ContactData::getCollider()
{
    return m_collider;
}

Fixture ContactData::getFixtureA()
{
    return Fixture(m_thisFixture);
}

Fixture ContactData::getFixtureB()
{
    return Fixture(m_otherFixture);
}

bool ContactData::operator < (const ContactData& data) const
{
    return data.m_collider > this->m_collider && data.m_otherFixture > this->m_otherFixture && data.m_thisFixture > this->m_thisFixture;
}

bool ContactData::operator > (const ContactData& data) const
{
    return data.m_collider < this->m_collider && data.m_otherFixture < this->m_otherFixture && data.m_thisFixture < this->m_thisFixture;
}

bool ContactData::operator == (const ContactData& data) const
{
    return data.m_collider == this->m_collider && data.m_otherFixture == this->m_otherFixture && data.m_thisFixture == this->m_thisFixture;
}

bool ContactData::operator != (const ContactData& data) const
{
    return !(*this == data);
}
