#ifndef COLLISION_MANAGER_HPP
#define COLLISION_MANAGER_HPP

#pragma once

#include <unordered_set>
#include <set>
#include <list>

#include "box2d/box2d.h"

#include "Physics/Collider.hpp"

class CollisionManager
{
public:
    // void BeginContact(b2Contact* contact) override;
    // void EndContact(b2Contact* contact) override;
    // void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
    // void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

    /// @brief Make sure to call this every frame after box2d update
    static void Update();

protected:
    /// @brief adds the collider to the manager
    static void addCollider(Collider* collider);
    /// @brief removes the collider for the manager
    static void removeCollider(Collider* collider);
    
    friend Collider;

private:
    struct m_contactData
    {
        m_contactData(b2ShapeId A, b2ShapeId B);
        
        bool operator < (const m_contactData& data) const;
        bool operator == (const m_contactData& data) const;

        b2ShapeId A = b2_nullShapeId;
        b2ShapeId B = b2_nullShapeId;
    };

    static std::unordered_set<Collider*> m_objects;
    /// @brief the set of contact data for all colliding objects
    static std::set<m_contactData> m_colliding;
    /// @brief if the colliding set is currently being used
    static bool m_usingCollidingSet;
    static std::list<m_contactData> m_collidingEraseQueue;
    /// @brief so that you can change the enabled state of a collider in a physics callback (destroying in pre solve sets enabled false)
    /// @note this is only for the deletion of the body not the entire object
    static EventHelper::Event m_updateBodyEvent;
    static bool m_inPhysicsUpdate;
};

#endif
