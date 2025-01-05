#include "Physics/CollisionManager.hpp"
#include "Physics/Collider.hpp"
#include "Physics/WorldHandler.hpp"
#include "ObjectManager.hpp"

CollisionManager::m_contactData::m_contactData(b2ShapeId A, b2ShapeId B) : A(A), B(B) {}
bool CollisionManager::m_contactData::operator < (const m_contactData& data) const
{
    return this->A.world0 < data.A.world0 || this->B.world0 < data.B.world0 ||
           this->A.revision < data.A.revision || this->B.revision < data.B.revision ||
           this->A.index1 < data.A.index1 || this->B.index1 < data.B.index1;
}
bool CollisionManager::m_contactData::operator == (const m_contactData& data) const
{
    return this->A.world0 == data.A.world0 && this->B.world0 == data.B.world0 &&
           this->A.revision == data.A.revision && this->B.revision == data.B.revision &&
           this->A.index1 == data.A.index1 && this->B.index1 == data.B.index1;
}

std::unordered_set<Collider*> CollisionManager::m_objects;
std::set<CollisionManager::m_contactData> CollisionManager::m_colliding;
bool CollisionManager::m_usingCollidingSet = false;
std::list<CollisionManager::m_contactData> CollisionManager::m_collidingEraseQueue;
EventHelper::Event CollisionManager::m_updateBodyEvent;
bool CollisionManager::m_inPhysicsUpdate = false;

// void CollisionManager::BeginContact(b2Contact* contact)
// {
//     Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
//     Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);

//     // updating all lists for new contact
//     m_beginContact.emplace_back(A, B, contact);
//     m_colliding.emplace(A,B,contact);
// }

// void CollisionManager::EndContact(b2Contact* contact)
// {
//     Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
//     Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);

//     // updating all lists for end contact
//     m_endContact.emplace_back(A, B, contact);

//     if (m_usingCollidingSet)
//         m_collidingEraseQueue.emplace_back(A,B,contact);
//     else
//         m_colliding.erase({A,B,contact});
// }

// void CollisionManager::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
// {
//     m_inPhysicsUpdate = true;
//     Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
//     Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);

//     A->PreSolve({B, contact->GetFixtureA(), contact->GetFixtureB(), contact});
//     B->PreSolve({A, contact->GetFixtureB(), contact->GetFixtureA(), contact});
//     m_inPhysicsUpdate = false;
// }

void CollisionManager::Update()
{
    #define GET_COLLIDER(b2Shape) ((Collider*)b2Body_GetUserData(b2Shape_GetBody(b2Shape)))
    m_updateBodyEvent.invoke();
    m_updateBodyEvent.disconnectAll();

    // updating all collider transforms before doing any callbacks
    for (auto obj: m_objects)
    {
        obj->m_update();
    }

    b2ContactEvents contactEvents = b2World_GetContactEvents(WorldHandler::getWorld());

    //* Calling being contacts
    for (int i = 0; i < contactEvents.beginCount; ++i)
    {
        b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;
        
        Collider* A = GET_COLLIDER(beginEvent->shapeIdA);
        Collider* B = GET_COLLIDER(beginEvent->shapeIdB);

        if (A->isDestroyQueued() || B->isDestroyQueued()) // dont want to call on begin contact because it no longer exists
            continue;

        A->BeginContact(ContactData{B, beginEvent->shapeIdA, beginEvent->shapeIdB});
        B->BeginContact(ContactData{A, beginEvent->shapeIdB, beginEvent->shapeIdA});
    }
    // -----------------------

    //* Calling end contacts
    for (int i = 0; i < contactEvents.endCount; ++i)
    { 
        b2ContactEndTouchEvent* endEvent = contactEvents.endEvents + i;
        
        Collider* A = GET_COLLIDER(endEvent->shapeIdA);
        Collider* B = GET_COLLIDER(endEvent->shapeIdB);

        // might want to know end contact even if it is destroyed therefor no if statement
        A->EndContact(ContactData{B, endEvent->shapeIdA, endEvent->shapeIdB});
        B->EndContact(ContactData{A, endEvent->shapeIdB, endEvent->shapeIdA});
    }
    // ---------------------

    //* Calling hit events
    for (int i = 0; i < contactEvents.hitCount; ++i)
    { 
        b2ContactHitEvent* hitEvent = contactEvents.hitEvents + i;
        
        Collider* A = GET_COLLIDER(hitEvent->shapeIdA);
        Collider* B = GET_COLLIDER(hitEvent->shapeIdB);

        // might want to know end contact even if it is destroyed therefor no if statement
        A->OnHit(HitData{B, hitEvent->shapeIdA, hitEvent->shapeIdB, hitEvent});
        B->OnHit(HitData{A, hitEvent->shapeIdB, hitEvent->shapeIdA, hitEvent});
    }
    // ---------------------

    //* Calling ongoing collisions
    m_usingCollidingSet = true;
    for (auto data: m_colliding)
    {
        Collider* A = GET_COLLIDER(data.A);
        Collider* B = GET_COLLIDER(data.B);
        A->OnColliding(ContactData{B, data.A, data.B});
        B->OnColliding(ContactData{A, data.B, data.A});
    }
    m_usingCollidingSet = false;
    for (auto eraseData: m_collidingEraseQueue)
    {
        m_colliding.erase(eraseData);
    }
    m_colliding.clear();
    // ---------------------------
    #undef GET_COLLIDER
}

void CollisionManager::addCollider(Collider* Collider)
{
    m_objects.insert({Collider});
}

void CollisionManager::removeCollider(Collider* collider)
{
    m_objects.erase({collider});
}
