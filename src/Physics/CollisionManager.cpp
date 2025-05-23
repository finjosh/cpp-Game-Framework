#include "Physics/CollisionManager.hpp"
#include "Physics/Collider.hpp"
#include "Physics/WorldHandler.hpp"
#include "ObjectManager.hpp"
#include "Physics/ContactData.hpp"
extern "C" {
    #include "BaseWorld.h"
}

CollisionManager::m_contactData::m_contactData(b2ShapeId A, b2ShapeId B) : A(A), B(B) {}
bool CollisionManager::m_contactData::operator < (const m_contactData& data) const
{
    return this->A.world0 < data.A.world0 || this->B.world0 < data.B.world0 ||
           this->A.generation < data.A.generation || this->B.generation < data.B.generation ||
           this->A.index1 < data.A.index1 || this->B.index1 < data.B.index1;
}
bool CollisionManager::m_contactData::operator == (const m_contactData& data) const
{
    return this->A.world0 == data.A.world0 && this->B.world0 == data.B.world0 &&
           this->A.generation == data.A.generation && this->B.generation == data.B.generation &&
           this->A.index1 == data.A.index1 && this->B.index1 == data.B.index1;
}

std::pair<std::mutex, EventHelper::Event>* CollisionManager::m_threadedEvents = nullptr;
unsigned int CollisionManager::m_threadedEventsSize = 0;

CollisionManager* CollisionManager::get()
{
    static CollisionManager manager;
    return &manager;
}

#define GET_COLLIDER(b2Shape) ((Collider*)b2Body_GetUserData(b2Shape_GetBody(b2Shape)))

bool CollisionManager::PreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context)
{
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

    bool shouldCollide = A->PreSolve(PreSolveData{shapeIdA, shapeIdB, manifold, &openEvent->second}) 
                   && B->PreSolve(PreSolveData{shapeIdB, shapeIdA, manifold, &openEvent->second});

    openEvent->first.unlock();
    return shouldCollide;
}

void CollisionManager::Update()
{
    // There should be no need to care about multiple threads here
    for (unsigned int i = 0; i < m_threadedEventsSize; i++)
    {
        m_threadedEvents[i].second.invoke();
        m_threadedEvents[i].second.disconnectAll();
    }

    // updating the position of the objects for bodies that moved
    {
        b2BodyEvents events = b2World_GetBodyEvents(WorldHandler::get().getWorld());

        for (std::int32_t i = 0; i < events.moveCount; i++)
        {
            if (b2Body_IsValid(events.moveEvents[i].bodyId))
                ((Collider*)(events.moveEvents[i].userData))->m_update(&events.moveEvents[i].transform);
        }
    }

    // !!!!!! This could end up being very bad if box2d ever uses the last bit of the flags
    // !!!!!! also could be very bad if the flags are ever reset by box2d
    #define WAS_TOUCHING_FLAG 0b10000000000000000000000000000000
    void* world = getWorld(WorldHandler::get().getWorld());
    b2Contact* contact = getContactsFromWorld(world);
    for (int i = 0; i < getContactCountFromWorld(world); i++)
    {
        if (contact == nullptr || contact->contactId == B2_NULL_INDEX)
            continue;

        std::uint32_t flags = contact->flags;
        bool isTouching = flags & b2_contactTouchingFlag;
        bool wasTouching = flags & WAS_TOUCHING_FLAG;
        
        if (flags & b2_contactEnableContactEvents && (isTouching || wasTouching) && contact->localIndex != B2_NULL_INDEX)
        {   
            _b2ContactData contactData = getContactDataFromWorld(world, contact);
            Collider* A = GET_COLLIDER(contactData.shapeIdA);
            Collider* B = GET_COLLIDER(contactData.shapeIdB);

            A->OnColliding(ContactData{contactData.shapeIdA, contactData.shapeIdB, contactData.manifold});
            B->OnColliding(ContactData{contactData.shapeIdB, contactData.shapeIdA, contactData.manifold});

            if (!wasTouching) // we started contact
            {
                if (isTouching)
                {
                    if (b2Shape_IsSensor(contactData.shapeIdA) || b2Shape_IsSensor(contactData.shapeIdB))
                    {
                        if (b2Shape_AreSensorEventsEnabled(contactData.shapeIdA))
                            A->BeginContactSensor(ContactData{contactData.shapeIdA, contactData.shapeIdB, contactData.manifold});
                        if (b2Shape_AreSensorEventsEnabled(contactData.shapeIdB))
                            B->BeginContactSensor(ContactData{contactData.shapeIdB, contactData.shapeIdA, contactData.manifold});
                    }
                    else
                    {
                        A->BeginContact(ContactData{contactData.shapeIdA, contactData.shapeIdB, contactData.manifold});
                        B->BeginContact(ContactData{contactData.shapeIdB, contactData.shapeIdA, contactData.manifold});
                    }
                }
            }
            else // we lost contact
            {
                if (!isTouching)
                {
                    if (b2Shape_IsSensor(contactData.shapeIdA) || b2Shape_IsSensor(contactData.shapeIdB))
                    {
                        if (b2Shape_AreSensorEventsEnabled(contactData.shapeIdA))
                            A->EndContactSensor(ContactData{contactData.shapeIdA, contactData.shapeIdB, contactData.manifold});
                        if (b2Shape_AreSensorEventsEnabled(contactData.shapeIdB))
                            B->EndContactSensor(ContactData{contactData.shapeIdB, contactData.shapeIdA, contactData.manifold});
                    }
                    else
                    {
                        A->EndContact(ContactData{contactData.shapeIdA, contactData.shapeIdB, contactData.manifold});
                        B->EndContact(ContactData{contactData.shapeIdB, contactData.shapeIdA, contactData.manifold});
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
}

#undef GET_COLLIDER

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
