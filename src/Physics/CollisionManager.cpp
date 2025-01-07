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

std::pair<std::mutex, EventHelper::Event>* CollisionManager::m_threadedEvents = nullptr;
unsigned int CollisionManager::m_threadedEventsSize = 0;
#ifdef DEBUG
std::atomic<int> CollisionManager::m_inPhysicsUpdate = 0;
#endif

CollisionManager* CollisionManager::get()
{
    static CollisionManager manager;
    return &manager;
}

#define GET_COLLIDER(b2Shape) ((Collider*)b2Body_GetUserData(b2Shape_GetBody(b2Shape)))

bool CollisionManager::PreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context)
{
    #ifdef DEBUG
    m_inPhysicsUpdate += 1;
    #endif
    Collider* A = GET_COLLIDER(shapeIdA);
    Collider* B = GET_COLLIDER(shapeIdB);

    std::pair<std::mutex, EventHelper::Event>* openEvent = nullptr;
    for (unsigned int i = 0; i < m_threadedEventsSize; ++i)
    {
        if (!m_threadedEvents[i].first.try_lock())
            continue;
        openEvent = &m_threadedEvents[i];
        break;
    }
    assert(openEvent != nullptr && "ERROR | No open event found!"); // this should never happen

    A->PreSolve(PreSolveData{A, shapeIdA, B, shapeIdB, manifold, &openEvent->second});
    B->PreSolve(PreSolveData{B, shapeIdB, A, shapeIdA, manifold, &openEvent->second});
    #ifdef DEBUG
    m_inPhysicsUpdate -= 1;
    #endif

    openEvent->first.unlock();
    return true; // continue with the collision
}

void CollisionManager::Update()
{
    // There should be no need to care about multiple threads here
    for (unsigned int i = 0; i < m_threadedEventsSize; ++i)
    {
        m_threadedEvents[i].second.invoke();
        m_threadedEvents[i].second.disconnectAll();
    }

    // updating all collider transforms before doing any callbacks
    for (auto obj: m_objects)
    {
        obj->m_update();
    }

    b2ContactEvents contactEvents = b2World_GetContactEvents(WorldHandler::get()->getWorld());

    //* Calling being contacts
    for (int i = 0; i < contactEvents.beginCount; ++i)
    {
        b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;
        
        Collider* A = GET_COLLIDER(beginEvent->shapeIdA);
        Collider* B = GET_COLLIDER(beginEvent->shapeIdB);
        m_colliding.emplace(beginEvent->shapeIdA,beginEvent->shapeIdB);

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
        m_colliding.erase(m_contactData{endEvent->shapeIdA,endEvent->shapeIdB});

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
}

void CollisionManager::addCollider(Collider* Collider)
{
    m_objects.insert({Collider});
}

void CollisionManager::removeCollider(Collider* collider)
{
    m_objects.erase({collider});
}

void CollisionManager::initWorkerThreadLists(unsigned int workers)
{
    m_threadedEventsSize = workers;
    m_threadedEvents = new std::pair<std::mutex, EventHelper::Event>[workers];
}
