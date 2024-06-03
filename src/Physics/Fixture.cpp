#include "Physics/Fixture.hpp"
#include "Physics/Collider.hpp"

Fixture::Fixture(Collider* collider, const b2FixtureDef& fixtureDef)
{
    if (collider)
    {
        Fixture(*collider, fixtureDef);
    }
}

Fixture::Fixture(Collider& collider, const b2FixtureDef& fixtureDef)
{
    collider.m_body->CreateFixture(&fixtureDef);
}

void Fixture::setSensor(bool sensor)
{
    m_fixture->SetSensor(sensor);
}

bool Fixture::isSensor() const
{
    return m_fixture->IsSensor();
}

void Fixture::setFilterData(const b2Filter& filter)
{
    m_fixture->SetFilterData(filter);
}

const b2Filter& Fixture::getFilterData() const
{
    return m_fixture->GetFilterData();
}

bool Fixture::getNext()
{
    if (m_fixture->GetNext())
    {
        m_fixture = m_fixture->GetNext();
        return true;
    }
    return false;
}

Collider* Fixture::getCollider()
{
    return static_cast<Collider*>((void*)m_fixture->GetBody()->GetUserData().pointer);
}

const Collider* Fixture::getCollider() const
{
    return static_cast<Collider*>((void*)m_fixture->GetBody()->GetUserData().pointer);
}

bool Fixture::testPoint(const b2Vec2& p) const
{
    return m_fixture->TestPoint(p);
}

void Fixture::getMassData(b2MassData* massData) const
{
    m_fixture->GetMassData(massData);
}

void Fixture::setDensity(float density)
{
    m_fixture->SetDensity(density);
}

float Fixture::getDensity() const
{
    return m_fixture->GetDensity();
}

float Fixture::getFriction() const
{
    return m_fixture->GetFriction();
}

void Fixture::setFriction(float friction)
{
    m_fixture->SetFriction(friction);
}

float Fixture::getRestitution() const
{
    return m_fixture->GetRestitution();
}

void Fixture::setRestitution(float restitution)
{
    m_fixture->SetRestitution(restitution);
}

float Fixture::getRestitutionThreshold() const
{
    return m_fixture->GetRestitutionThreshold();
}

void Fixture::setRestitutionThreshold(float threshold)
{
    m_fixture->SetRestitutionThreshold(threshold);
}

Fixture::Fixture(b2Fixture* fixture) : m_fixture(fixture) {}
