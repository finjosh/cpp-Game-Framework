#include "Physics/CollisionManager.hpp"
#include "Physics/Collider.hpp"
#include "ObjectManager.hpp"

CollisionManager::m_contactData::m_contactData(Collider* A, Collider* B, b2Contact* contactData) : A(A), B(B), contactData(contactData) {}

std::unordered_set<Collider*> CollisionManager::m_objects;
std::list<CollisionManager::m_contactData> CollisionManager::m_beginContact;
std::list<CollisionManager::m_contactData> CollisionManager::m_endContact;

void CollisionManager::BeginContact(b2Contact* contact)
{
    Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    // updating all lists for new contact
    m_beginContact.push_back({A, B, contact});
    A->m_currentCollisions.insert({B, contact->GetFixtureA(), contact->GetFixtureB(), contact});
    B->m_currentCollisions.insert({A, contact->GetFixtureB(), contact->GetFixtureA(), contact});
}

void CollisionManager::EndContact(b2Contact* contact)
{
    Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    // updating all lists for end contact
    m_endContact.push_back({A, B, contact});
    A->m_currentCollisions.erase({B, contact->GetFixtureA(), contact->GetFixtureB(), contact});
    B->m_currentCollisions.erase({A, contact->GetFixtureB(), contact->GetFixtureA(), contact});
}

void CollisionManager::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    b2Body* body = contact->GetFixtureA()->GetBody();
    if (body != nullptr)
    {
        Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
        collider->PreSolve(contact, oldManifold);
    }
    body = contact->GetFixtureB()->GetBody();
    if (body != nullptr)
    {
        Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
        collider->PreSolve(contact, oldManifold);
    }
}

void CollisionManager::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    b2Body* body = contact->GetFixtureA()->GetBody();
    if (body != nullptr)
    {
        Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
        collider->PostSolve(contact, impulse);
    }
    body = contact->GetFixtureB()->GetBody();
    if (body != nullptr)
    {
        Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
        collider->PostSolve(contact, impulse);
    }
}

void CollisionManager::Update()
{
    for (auto obj: m_objects)
    {
        obj->m_update();
        for (auto collision: obj->m_currentCollisions)
        {
            obj->OnColliding(collision);
        }
    }

    for (auto data: m_beginContact)
    {
        data.A->BeginContact({data.B, data.contactData->GetFixtureA(), data.contactData->GetFixtureB(), data.contactData});
        data.B->BeginContact({data.A, data.contactData->GetFixtureB(), data.contactData->GetFixtureA(), data.contactData});
    }
    m_beginContact.clear();

    for (auto data: m_endContact)
    {
        data.A->EndContact({data.B, data.contactData->GetFixtureA(), data.contactData->GetFixtureB(), data.contactData});
        data.B->EndContact({data.A, data.contactData->GetFixtureB(), data.contactData->GetFixtureA(), data.contactData});
    }
    m_endContact.clear();
}

void CollisionManager::addCollider(Collider* Collider)
{
    m_objects.insert({Collider});
}

void CollisionManager::removeCollider(Collider* collider)
{
    m_objects.erase({collider});
}
