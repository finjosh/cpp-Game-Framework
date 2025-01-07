#ifndef COLLISION_MANAGER_HPP
#define COLLISION_MANAGER_HPP

#pragma once

#include <unordered_set>
#include <set>
#include <list>
#include <mutex>

#include "box2d/box2d.h"

#include "Physics/Collider.hpp"

class WorldHandler;

class CollisionManager
{
public:
    static CollisionManager* get();
    static bool PreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context);

    /// @brief Make sure to call this every frame after box2d update
    void Update();

protected:
    /// @brief adds the collider to the manager
    void addCollider(Collider* collider);
    /// @brief removes the collider for the manager
    void removeCollider(Collider* collider);
    static void initWorkerThreadLists(unsigned int workers);

    friend Collider;
    friend WorldHandler;
    friend PreSolveData;

private:
    struct m_contactData
    {
        m_contactData(b2ShapeId A, b2ShapeId B);
        
        bool operator < (const m_contactData& data) const;
        bool operator == (const m_contactData& data) const;

        b2ShapeId A = b2_nullShapeId;
        b2ShapeId B = b2_nullShapeId;
    };

    std::unordered_set<Collider*> m_objects;
    /// @brief the set of contact data for all colliding objects
    std::set<m_contactData> m_colliding;
    /// @brief if the colliding set is currently being used
    bool m_usingCollidingSet = false;
    std::list<m_contactData> m_collidingEraseQueue;
    /// @note theses events are called after the physics update
    static std::pair<std::mutex, EventHelper::Event>* m_threadedEvents;
    static unsigned int m_threadedEventsSize;
    #ifdef DEBUG
    static std::atomic<int> m_inPhysicsUpdate;
    #endif
};

#endif
