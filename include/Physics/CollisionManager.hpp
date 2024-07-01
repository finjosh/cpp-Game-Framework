#ifndef COLLISION_MANAGER_HPP
#define COLLISION_MANAGER_HPP

#pragma once

#include <unordered_set>
#include <set>
#include <list>

#include "box2d/b2_world_callbacks.h"
#include "box2d/b2_contact.h"
#include "box2d/b2_collision.h"

#include "Physics/Collider.hpp"

class CollisionManager : public b2ContactListener
{
public:
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

    /// @brief Make sure to call this every frame after box2d update
    static void Update();

protected:
    static void addCollider(Collider* Collider);
    static void removeCollider(Collider* collider);
    
    friend Collider;

private:
    struct m_contactData
    {
        m_contactData(Collider* A, Collider* B, b2Contact* contactData);
        
        bool operator < (const m_contactData& data) const;
        bool operator == (const m_contactData& data) const;

        Collider* A = nullptr;
        Collider* B = nullptr;
        b2Contact* contactData = nullptr;
    };

    static std::unordered_set<Collider*> m_objects;
    static std::list<m_contactData> m_beginContact;
    static std::list<m_contactData> m_endContact;
    static std::set<m_contactData> m_colliding;
    static bool m_usingCollidingSet;
    static std::list<m_contactData> m_collidingEraseQueue;
};

#endif
