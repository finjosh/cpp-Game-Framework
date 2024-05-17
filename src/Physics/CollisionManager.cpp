#include "Physics/CollisionManager.hpp"
#include "Physics/Collider.hpp"
#include "ObjectManager.hpp"

std::unordered_set<Collider*> CollisionManager::m_objects;
std::list<std::pair<Collider*, CollisionData>> CollisionManager::m_beginContact;
std::list<std::pair<Collider*, CollisionData>> CollisionManager::m_endContact;

void CollisionManager::BeginContact(b2Contact* contact)
{
    Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);
    if (A != nullptr)
    {
        CollisionData data = {B, contact->GetFixtureA(), contact->GetFixtureB()};
        m_beginContact.push_back({A, data}); // TODO redo this storage
        A->m_currentCollisions.insert(data);
        // A->BeginContact({B, contact->GetFixtureA(), contact->GetFixtureB()});
    }
    if (B != nullptr)
    {
        CollisionData data = {A, contact->GetFixtureB(), contact->GetFixtureA()};
        m_beginContact.push_back({B, data}); // TODO redo this storage
        B->m_currentCollisions.insert(data);
        // B->BeginContact({A, contact->GetFixtureB(), contact->GetFixtureA()});
    }
}

void CollisionManager::EndContact(b2Contact* contact)
{
    Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);
    // TODO fix this
    if (A != nullptr)
    {
        A->m_currentCollisions.erase({B, contact->GetFixtureA(), contact->GetFixtureB()});
    }
    if (B != nullptr)
    {
        B->m_currentCollisions.erase({A, contact->GetFixtureB(), contact->GetFixtureA()});
    }
    if (A == nullptr || B == nullptr) return;
    // if (A != nullptr)
    // {
        m_endContact.push_back({A, {B, contact->GetFixtureA(), contact->GetFixtureB()}});
        // A->EndContact({B, contact->GetFixtureA(), contact->GetFixtureB()});
    // }
    // if (B != nullptr)
    // {
        m_endContact.push_back({B, {A, contact->GetFixtureB(), contact->GetFixtureA()}});
        // B->EndContact({A, contact->GetFixtureB(), contact->GetFixtureA()});
    // }
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
    for (auto data: m_beginContact)
    {
        data.first->BeginContact(data.second);
    }
    m_beginContact.clear();

    for (auto data: m_endContact)
    {
        data.first->EndContact(data.second);
    }
    m_endContact.clear();

    ObjectManager::ClearDestroyQueue();

    for (auto obj: m_objects)
    {
        for (auto collision: obj->m_currentCollisions) // TODO implement this feature better
        {
            obj->OnColliding(collision);
        }
    }
}

void CollisionManager::addCollider(Collider* Collider)
{
    m_objects.insert({Collider});
}

void CollisionManager::removeCollider(Collider* collider)
{
    m_objects.erase({collider});
}
