#include "Physics/Collider.hpp"
#include "Physics/CollisionManager.hpp"

Collider::Collider()
{
    Object::m_onDisabled(&Collider::updatePhysicsState, this);
    Object::m_onEnabled(&Collider::updatePhysicsState, this);
    Object::m_onDestroyQueue(&Collider::destroyBody, this);
    // Object::_onParentRemoved(&CollisionManager::addCollider, this);
    // Object::_onParentSet(&CollisionManager::removeCollider, this);

    CollisionManager::addCollider(this);
}

Collider::~Collider()
{
    if (m_body != nullptr)
    {
        WorldHandler::getWorld().DestroyBody(m_body);
    }

    CollisionManager::removeCollider(this);
}

void Collider::destroyBody()
{
    CollisionManager::removeCollider(this);
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

void Collider::initCollider(const b2Vec2& pos, const b2Rot& rot)
{
    this->initCollider(pos.x, pos.y, rot.GetAngle());
}

void Collider::initCollider(const float& x, const float& y, const float& rot)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x,y);
    bodyDef.angle = rot;
    this->initCollider(bodyDef);
}

void Collider::initCollider(const b2BodyDef& bodyDef)
{
    m_body = WorldHandler::getWorld().CreateBody(&bodyDef);
    m_body->GetUserData().pointer = (uintptr_t)this;
    Object::setTransform(m_body->GetTransform());
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

void Collider::updatePhysicsState()
{
    m_body->SetEnabled(m_enabled && Object::isEnabled());
}

void Collider::_update()
{
    if (m_body != nullptr)
        Object::setTransform(m_body->GetTransform());
}

void Collider::setPosition(const b2Vec2& position) 
{
    if (m_body != nullptr)
    {
        m_body->SetTransform(position, m_body->GetAngle());
    }
    Object::setPosition(position);
}

void Collider::setRotation(const float& rotation) 
{
    if (m_body != nullptr)
    {
        m_body->SetTransform(m_body->GetPosition(), rotation);
    }
    Object::setRotation(rotation);
}

void Collider::setTransform(const b2Transform& transform) 
{
    if (m_body != nullptr)
    {
        m_body->SetTransform(transform.p, transform.q.GetAngle());
    }
    Object::setTransform(transform);
}

void Collider::move(const b2Vec2& move)
{
    if (m_body != nullptr)
    {
        m_body->SetTransform(m_body->GetPosition() + move, m_body->GetAngle());
    }
    Object::move(move);
}

void Collider::rotate(const float& rot)
{
    if (m_body != nullptr)
    {
        m_body->SetTransform(m_body->GetPosition(), m_body->GetAngle() + rot);
    }
    Object::rotate(rot);
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
