#ifndef COLLISION_MANAGER_HPP
#define COLLISION_MANAGER_HPP

#pragma once

#include <unordered_set>
#include <set>
#include <list>

#include "box2d/collision.h"

#include "Physics/Collider.hpp"

class CollisionManager
{
public:
    CollisionManager();
    static CollisionManager& get();

    static bool b2PreSolveFcn(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context);

    /// @brief Make sure to call this every frame after box2d update
    /// @note should already be called by WorldHandler
    void Update();

protected:
    /// @brief adds the collider to the manager
    void addCollider(Collider* collider);
    /// @brief removes the collider for the manager
    void removeCollider(Collider* collider);
    
    friend Collider;
    friend PreSolveData;

private:
    std::unordered_set<Collider*> m_objects;
    /// @brief so that you can change the enabled state of a collider in a physics callback (destroying in pre solve sets enabled false)
    /// @note this is only for the deletion of the body not the entire object
    EventHelper::Event m_updateBodyEvent;
    bool m_inPhysicsUpdate = false;
    std::mutex m_threadLock; // not the best way to deal with pre solve having to be thread safe (but works for now)
};

#endif
