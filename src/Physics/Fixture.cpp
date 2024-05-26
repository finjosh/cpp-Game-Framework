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

void Fixture::SetSensor(const bool& sensor)
{
    m_fixture->SetSensor(sensor);
}

bool Fixture::IsSensor() const
{
    return m_fixture->IsSensor();
}

void Fixture::SetFilterData(const b2Filter& filter)
{
    m_fixture->SetFilterData(filter);
}

const b2Filter& Fixture::GetFilterData() const
{
    return m_fixture->GetFilterData();
}

bool Fixture::GetNext()
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

bool Fixture::TestPoint(const b2Vec2& p) const
{
    return m_fixture->TestPoint(p);
}

void Fixture::GetMassData(b2MassData* massData) const
{
    m_fixture->GetMassData(massData);
}

void Fixture::SetDensity(const float& density)
{
    m_fixture->SetDensity(density);
}

float Fixture::GetDensity() const
{
    return m_fixture->GetDensity();
}

float Fixture::GetFriction() const
{
    return m_fixture->GetFriction();
}

void Fixture::SetFriction(const float& friction)
{
    m_fixture->SetFriction(friction);
}

float Fixture::GetRestitution() const
{
    return m_fixture->GetRestitution();
}

void Fixture::SetRestitution(const float& restitution)
{
    m_fixture->SetRestitution(restitution);
}

float Fixture::GetRestitutionThreshold() const
{
    return m_fixture->GetRestitutionThreshold();
}

void Fixture::SetRestitutionThreshold(const float& threshold)
{
    m_fixture->SetRestitutionThreshold(threshold);
}

// Fixture::Fixture(b2Body* body, const b2FixtureDef& fixtureDef)
// {

// }

// Fixture::Fixture(b2Fixture* fixture)
// {

// }
