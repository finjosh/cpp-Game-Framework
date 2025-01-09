#include "Physics/CollisionManager.hpp"
#include "Physics/Collider.hpp"
#include "Physics/WorldHandler.hpp"
#include "ObjectManager.hpp"
#include "Physics/ContactData.hpp"
extern "C" {
    #include "Physics/BaseWorld.h"
}

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

    A->PreSolve(PreSolveData{A, shapeIdA, B, shapeIdB, manifold, &openEvent->second}); // TODO handle a way to stop collision (return true or false)
    B->PreSolve(PreSolveData{B, shapeIdB, A, shapeIdA, manifold, &openEvent->second}); // TODO handle a way to stop collision (return true or false)
    #ifdef DEBUG
    m_inPhysicsUpdate -= 1;
    #endif

    openEvent->first.unlock();
    return true; // continue with the collision
}

#include <iostream>
#include <bitset>
void CollisionManager::Update()
{
    // updating all collider transforms before doing any callbacks
    for (auto obj: m_objects)
    {
        obj->m_update(); // TODO do this only to bodies that where moved using b2World_GetBodyEvents()
    }

    // !!!!!! This could end up being very bad if box2d ever uses the last bit of the flags
    // !!!!!! also could be very bad if the flags are ever reset by box2d
    #define WAS_TOUCHING_FLAG 0b10000000000000000000000000000000
    int count = 0;
    b2Contact* contact = getWorldContacts(WorldHandler::get()->getWorld());
    for (int i = 0; i < getWorldContactCount(WorldHandler::get()->getWorld()); i++)
    {
        // only one of the following are set at a time, touching or sensor touching
        std::uint32_t flags = contact->flags;
        bool isTouching = flags & b2_contactTouchingFlag;
        bool wasTouching = flags & WAS_TOUCHING_FLAG;
        if (flags & b2_contactSensorTouchingFlag)
        {
            if (flags & b2_contactEnableSensorEvents)
            {

            }
        }
        else
        {
            if ((isTouching || wasTouching) && contact->localIndex != B2_NULL_INDEX && flags & b2_contactEnableContactEvents)
            {   
                _b2ContactData contactData = getContactData(WorldHandler::get()->getWorld(), contact);
                Collider* A = GET_COLLIDER(contactData.shapeIdA);
                Collider* B = GET_COLLIDER(contactData.shapeIdB);

                A->OnColliding(ContactData{contactData.shapeIdA, contactData.shapeIdB});
                B->OnColliding(ContactData{contactData.shapeIdB, contactData.shapeIdA});

                if (!wasTouching) // we started contact
                {
                    if (isTouching)
                    {
                        A->BeginContact(ContactData{contactData.shapeIdA, contactData.shapeIdB});
                        B->BeginContact(ContactData{contactData.shapeIdB, contactData.shapeIdA});
                    }
                }
                else // we lost contact
                {
                    if (!isTouching)
                    {
                        A->EndContact(ContactData{contactData.shapeIdA, contactData.shapeIdB});
                        B->EndContact(ContactData{contactData.shapeIdB, contactData.shapeIdA});
                    }
                }
            }
        }

        if (isTouching)
            contact->flags |= WAS_TOUCHING_FLAG;
        else
            contact->flags &= ~WAS_TOUCHING_FLAG;

        contact = &contact[1];
    }
    #undef WAS_TOUCHING_FLAG

    // TODO note that the body is only destroyed after all events are called in the preSolveClass
    // There should be no need to care about multiple threads here
    for (unsigned int i = 0; i < m_threadedEventsSize; i++)
    {
        m_threadedEvents[i].second.invoke();
        m_threadedEvents[i].second.disconnectAll();
    }

    // b2ContactEvents contactEvents = b2World_GetContactEvents(WorldHandler::get()->getWorld());

    // //* Calling being contacts
    // for (int i = 0; i < contactEvents.beginCount; i++)
    // {
    //     b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;
        
    //     if (!b2Shape_IsValid(beginEvent->shapeIdA) || !b2Shape_IsValid(beginEvent->shapeIdB))
    //         continue;

    //     Collider* A = GET_COLLIDER(beginEvent->shapeIdA);
    //     Collider* B = GET_COLLIDER(beginEvent->shapeIdB);
    //     m_colliding.emplace(beginEvent->shapeIdA,beginEvent->shapeIdB);

    //     // if (A->isDestroyQueued() || B->isDestroyQueued()) // dont want to call on begin contact because it no longer exists
    //     //     continue;

    //     A->BeginContact(ContactData{beginEvent->shapeIdA, beginEvent->shapeIdB});
    //     B->BeginContact(ContactData{beginEvent->shapeIdB, beginEvent->shapeIdA});
    // }
    // // -----------------------

    // //* Calling end contacts
    // for (int i = 0; i < contactEvents.endCount; i++)
    // { 
    //     b2ContactEndTouchEvent* endEvent = contactEvents.endEvents + i;

    //     if (!b2Shape_IsValid(endEvent->shapeIdA) || !b2Shape_IsValid(endEvent->shapeIdB))
    //         continue;

    //     Collider* A = GET_COLLIDER(endEvent->shapeIdA);
    //     Collider* B = GET_COLLIDER(endEvent->shapeIdB);
    //     m_colliding.erase(m_contactData{endEvent->shapeIdA,endEvent->shapeIdB});

    //     A->EndContact(ContactData{endEvent->shapeIdA, endEvent->shapeIdB});
    //     B->EndContact(ContactData{endEvent->shapeIdB, endEvent->shapeIdA});
    // }
    // // ---------------------

    // //* Calling hit events
    // for (int i = 0; i < contactEvents.hitCount; i++)
    // { 
    //     b2ContactHitEvent* hitEvent = contactEvents.hitEvents + i;
        
    //     if (!b2Shape_IsValid(hitEvent->shapeIdA) || !b2Shape_IsValid(hitEvent->shapeIdB))
    //         continue;

    //     Collider* A = GET_COLLIDER(hitEvent->shapeIdA);
    //     Collider* B = GET_COLLIDER(hitEvent->shapeIdB);

    //     A->OnHit(HitData{hitEvent->shapeIdA, hitEvent->shapeIdB, hitEvent});
    //     B->OnHit(HitData{hitEvent->shapeIdB, hitEvent->shapeIdA, hitEvent});
    // }
    // // ---------------------

    // //* Calling ongoing collisions
    // m_usingCollidingSet = true;
    // for (auto data: m_colliding)
    // {
    //     Collider* A = GET_COLLIDER(data.A);
    //     Collider* B = GET_COLLIDER(data.B);
    //     A->OnColliding(ContactData{data.A, data.B});
    //     B->OnColliding(ContactData{data.B, data.A});
    // }
    // m_usingCollidingSet = false;
    // for (auto eraseData: m_collidingEraseQueue)
    // {
    //     m_colliding.erase(eraseData);
    // }
    // m_colliding.clear();
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
