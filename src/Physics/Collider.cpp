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
    m_destroyBody();
    CollisionManager::removeCollider(this);
}

void Collider::m_destroyBody()
{
    if (m_body)
    {
        WorldHandler::getWorld().DestroyBody(m_body);
    }
}

b2Body* Collider::operator->()
{
    return m_body;
}

const b2Body* Collider::operator->() const
{
    return m_body;
}

b2Body* Collider::operator*()
{
    return m_body;
}

const b2Body* Collider::operator*() const
{
    return m_body;
}

b2Body* Collider::getBody()
{
    return m_body;
}

const b2Body* Collider::getBody() const
{
    return m_body;
}

b2Fixture* Collider::createFixture(const b2FixtureDef& fixture)
{
    return m_body->CreateFixture(&fixture);
}

b2Fixture* Collider::createFixture(const b2Shape& shape, const float& density, const float& friction, 
                                    const float& restitution, const float& restitutionThreshold, const b2Filter& filter)
{
    b2FixtureDef def;
    def.density = density;
    def.friction = friction;
    def.restitution = restitution;
    def.restitutionThreshold = restitutionThreshold;
    def.filter = filter;
    def.shape = &shape;
    return m_body->CreateFixture(&def);
}

b2Fixture* Collider::createFixtureSensor(const b2Shape& shape, const float& density, const b2Filter& filter)
{
    b2FixtureDef def;
    def.isSensor = true;
    def.density = density;
    def.filter = filter;
    def.shape = &shape;
    return m_body->CreateFixture(&def);
}

void Collider::setPhysicsEnabled(const bool& enabled)
{
    m_enabled = enabled;

    if (m_body == nullptr)
        return;

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
    if (m_body)
        m_body->SetTransform(Object::getPosition(), Object::getRotation());
}

void Collider::m_update()
{
    if (m_body != nullptr)
        Object::setTransform(m_body->GetTransform()); //! NOTE this updates the object position which calls event to update this position (could result in errors later)
}

void Collider::setAwake(const bool& awake)
{
    m_body->SetAwake(awake);
}

//* Collision Data

CollisionData::CollisionData(Collider* collider, b2Fixture* thisFixture, b2Fixture* otherFixture) : m_collider(collider), m_thisFixture(thisFixture), m_otherFixture(otherFixture) {}

Collider* CollisionData::getCollider()
{
    return m_collider;
}

b2Fixture* CollisionData::getFixtureA()
{
    return m_thisFixture;
}

b2Fixture* CollisionData::getFixtureB()
{
    return m_otherFixture;
}

bool CollisionData::operator < (const CollisionData& data) const
{
    return data.m_collider > this->m_collider && data.m_otherFixture > this->m_otherFixture && data.m_thisFixture > this->m_thisFixture;
}

bool CollisionData::operator > (const CollisionData& data) const
{
    return data.m_collider < this->m_collider && data.m_otherFixture < this->m_otherFixture && data.m_thisFixture < this->m_thisFixture;
}

bool CollisionData::operator == (const CollisionData& data) const
{
    return data.m_collider == this->m_collider && data.m_otherFixture == this->m_otherFixture && data.m_thisFixture == this->m_thisFixture;
}

bool CollisionData::operator != (const CollisionData& data) const
{
    return !(*this == data);
}
