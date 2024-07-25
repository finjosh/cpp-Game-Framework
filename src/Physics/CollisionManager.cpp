#include "Physics/CollisionManager.hpp"
#include "Physics/Collider.hpp"
#include "Physics/WorldHandler.hpp"
#include "ObjectManager.hpp"

CollisionManager::m_contactData::m_contactData(Collider* A, Collider* B, b2Contact* contactData) : A(A), B(B), contactData(contactData) {}
bool CollisionManager::m_contactData::operator < (const m_contactData& data) const
{
    return this->A < data.A || this->B < data.B || this->contactData < data.contactData;
}
bool CollisionManager::m_contactData::operator == (const m_contactData& data) const
{
    return this->A == data.A && this->B == data.B && this->contactData == data.contactData;
}

std::unordered_set<Collider*> CollisionManager::m_objects;
std::list<CollisionManager::m_contactData> CollisionManager::m_beginContact;
std::list<CollisionManager::m_contactData> CollisionManager::m_endContact;
std::set<CollisionManager::m_contactData> CollisionManager::m_colliding;
bool CollisionManager::m_usingCollidingSet = false;
std::list<CollisionManager::m_contactData> CollisionManager::m_collidingEraseQueue;
std::list<Collider*> CollisionManager::m_deleteQueue;
bool CollisionManager::m_inPhysicsUpdate = false;

void CollisionManager::BeginContact(b2Contact* contact)
{
    Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    // updating all lists for new contact
    m_beginContact.emplace_back(A, B, contact);
    m_colliding.emplace(A,B,contact);
}

void CollisionManager::EndContact(b2Contact* contact)
{
    Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    // updating all lists for end contact
    m_endContact.emplace_back(A, B, contact);

    if (m_usingCollidingSet)
        m_collidingEraseQueue.emplace_back(A,B,contact);
    else
        m_colliding.erase({A,B,contact});
}

void CollisionManager::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    m_inPhysicsUpdate = true;
    Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    A->PreSolve({B, contact->GetFixtureA(), contact->GetFixtureB(), contact});
    B->PreSolve({A, contact->GetFixtureB(), contact->GetFixtureA(), contact});
    m_inPhysicsUpdate = false;
}

void CollisionManager::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    // b2Body* body = contact->GetFixtureA()->GetBody();
    // if (body != nullptr)
    // {
    //     Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
    //     collider->PostSolve(contact, impulse);
    // }
    // body = contact->GetFixtureB()->GetBody();
    // if (body != nullptr)
    // {
    //     Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
    //     collider->PostSolve(contact, impulse);
    // }
}

void CollisionManager::Update()
{
    for (auto obj: m_objects)
    {
        obj->m_update();
    }

    m_usingCollidingSet = true;
    for (auto data: m_colliding)
    {
        data.A->OnColliding({data.B, data.contactData->GetFixtureA(), data.contactData->GetFixtureB(), data.contactData});
        data.B->OnColliding({data.A, data.contactData->GetFixtureB(), data.contactData->GetFixtureA(), data.contactData});
    }
    m_usingCollidingSet = false;
    for (auto eraseData: m_collidingEraseQueue)
    {
        m_colliding.erase(eraseData);
    }
    m_colliding.clear();

    for (auto data: m_beginContact)
    {
        if (data.A->isDestroyed() || data.B->isDestroyed()) // dont want to call on begin contact because it no longer exists
            continue;

        data.A->BeginContact({data.B, data.contactData->GetFixtureA(), data.contactData->GetFixtureB(), data.contactData});
        data.B->BeginContact({data.A, data.contactData->GetFixtureB(), data.contactData->GetFixtureA(), data.contactData});
    }
    m_beginContact.clear();

    for (auto data: m_endContact)
    { // might want to know end contact even if it is destroyed
        data.A->EndContact({data.B, data.contactData->GetFixtureA(), data.contactData->GetFixtureB(), data.contactData});
        data.B->EndContact({data.A, data.contactData->GetFixtureB(), data.contactData->GetFixtureA(), data.contactData});
    }
    m_endContact.clear();

    for (auto obj: m_deleteQueue)
    {
        WorldHandler::getWorld().DestroyBody(obj->m_body);
    }
    m_deleteQueue.clear();
}

void CollisionManager::addCollider(Collider* Collider)
{
    m_objects.insert({Collider});
}

void CollisionManager::removeCollider(Collider* collider)
{
    m_objects.erase({collider});
}
