#include "Physics/ContactData.hpp"

//* Collision Data

#define GET_COLLIDER(b2Shape) ((Collider*)b2Body_GetUserData(b2Shape_GetBody(b2Shape)))

ContactData::ContactData(b2ShapeId thisFixture, b2ShapeId otherFixture) : m_thisShape(thisFixture), m_otherShape(otherFixture) {}

Collider* ContactData::getCollider()
{
    return GET_COLLIDER(m_thisShape);
}

const Collider* ContactData::getCollider() const
{
    return GET_COLLIDER(m_thisShape);
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

//* Hit Data

HitData::HitData(b2ShapeId thisFixture, b2ShapeId otherFixture, const b2ContactHitEvent* m_hitData) 
    : m_thisShape(thisFixture), m_otherShape(otherFixture), m_hitData(m_hitData) {}

Collider* HitData::getCollider()
{ return GET_COLLIDER(m_otherShape); }

const Collider* HitData::getCollider() const
{ return GET_COLLIDER(m_otherShape); }

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

PreSolveData::PreSolveData(Collider* thisCollider, b2ShapeId thisFixture, Collider* otherCollider, b2ShapeId otherFixture, b2Manifold* contactData, EventHelper::Event* updateBodyEvent) : 
    m_otherCollider(otherCollider), m_thisCollider(thisCollider), m_thisShape(thisFixture), m_otherShape(otherFixture), m_manifold(contactData), m_updateBodyEvent(updateBodyEvent) {}

const Collider* PreSolveData::getCollider() const
{
    return m_otherCollider;
}

Collider* PreSolveData::getNoneConstCollider()
{
    return m_otherCollider;
}

const Fixture PreSolveData::getThisFixture() const
{
    return Fixture(m_thisShape);
}

const Fixture PreSolveData::getOtherFixture() const
{
    return Fixture(m_otherShape);
}

void PreSolveData::destroyOtherCollider()
{
    // TODO implement this via the event system
    // m_otherCollider->destroy();
    // m_updateBodyEvent->connect(Collider::destroy, m_otherCollider);
}

void PreSolveData::destroyThisCollider()
{
    // TODO implement this via the event system
    // m_thisCollider->destroy();
    // m_updateBodyEvent->connect(Collider::destroy, m_thisCollider);
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
