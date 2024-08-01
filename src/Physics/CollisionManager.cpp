#include "Physics/CollisionManager.hpp"
#include "Physics/Collider.hpp"
#include "Physics/WorldHandler.hpp"
#include "ObjectManager.hpp"
#include "box2d/box2d.h"

CollisionManager::CollisionManager()
{
    assert("World Handler MUST have a valid world" && b2World_IsValid(WorldHandler::get().getWorld()));
}

CollisionManager& CollisionManager::get()
{
    static CollisionManager manager;
    return manager;
}

bool CollisionManager::b2PreSolveFcn(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context)
{
    CollisionManager* temp = static_cast<CollisionManager*>(context);
    std::lock_guard<std::mutex> lock(temp->m_threadLock);
    temp->m_inPhysicsUpdate = true;
    Collider* A = static_cast<Collider*>(b2Body_GetUserData(b2Shape_GetBody(shapeIdA)));
    Collider* B = static_cast<Collider*>(b2Body_GetUserData(b2Shape_GetBody(shapeIdB)));

    bool hasContact = A->PreSolve({B, shapeIdA, shapeIdB, manifold}) || B->PreSolve({A, shapeIdB, shapeIdA, manifold});
    temp->m_inPhysicsUpdate = false;
    return hasContact;
}

void CollisionManager::Update()
{
    m_updateBodyEvent.invoke();
    m_updateBodyEvent.disconnectAll();

    size_t events = 0;
    
    //* update position and sleeping event
    {
        auto eventData = b2World_GetBodyEvents(WorldHandler::get().getWorld());
        events = eventData.moveCount;

        for (size_t i = 0; i < events; i++) 
        {
            Collider* object = static_cast<Collider*>(eventData.moveEvents[i].userData);
            if (eventData.moveEvents[i].fellAsleep)
                object->OnFellAsleep();

            object->m_update(eventData.moveEvents[i].transform);
        }
    }

    auto eventData = b2World_GetContactEvents(WorldHandler::get().getWorld());
    events = eventData.beginCount;

    Collider* A = nullptr;
    Collider* B = nullptr;
    for (size_t i = 0; i < events; i++)
    {
        A = static_cast<Collider*>(b2Body_GetUserData(b2Shape_GetBody(eventData.beginEvents[i].shapeIdA)));
        B = static_cast<Collider*>(b2Body_GetUserData(b2Shape_GetBody(eventData.beginEvents[i].shapeIdB)));
        A->BeginContact(ContactData{B, eventData.beginEvents[i].shapeIdB, eventData.beginEvents[i].shapeIdA});
        B->BeginContact(ContactData{A, eventData.beginEvents[i].shapeIdA, eventData.beginEvents[i].shapeIdB});
    }

    events = eventData.hitCount;
    for (size_t i = 0; i < events; i++)
    {
        A = static_cast<Collider*>(b2Body_GetUserData(b2Shape_GetBody(eventData.hitEvents[i].shapeIdA)));
        B = static_cast<Collider*>(b2Body_GetUserData(b2Shape_GetBody(eventData.hitEvents[i].shapeIdB)));
        A->OnContactHit(HitData{B, eventData.hitEvents[i].shapeIdB, eventData.hitEvents[i].shapeIdA, eventData.hitEvents[i].normal, eventData.hitEvents[i].point, eventData.hitEvents[i].approachSpeed});
        B->OnContactHit(HitData{A, eventData.hitEvents[i].shapeIdA, eventData.hitEvents[i].shapeIdB, eventData.hitEvents[i].normal, eventData.hitEvents[i].point, eventData.hitEvents[i].approachSpeed});
    }

    events = eventData.endCount;
    for (size_t i = 0; i < events; i++)
    {
        A = static_cast<Collider*>(b2Body_GetUserData(b2Shape_GetBody(eventData.endEvents[i].shapeIdA)));
        B = static_cast<Collider*>(b2Body_GetUserData(b2Shape_GetBody(eventData.endEvents[i].shapeIdB)));
        A->EndContact(ContactData{B, eventData.endEvents[i].shapeIdB, eventData.endEvents[i].shapeIdA});
        B->EndContact(ContactData{A, eventData.endEvents[i].shapeIdA, eventData.endEvents[i].shapeIdB});
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
