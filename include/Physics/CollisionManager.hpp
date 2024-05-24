#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#pragma once

#include <unordered_set>
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
        m_contactData(Collider* A, b2Fixture* AFix, Collider* B, b2Fixture* BFix);

        Collider* A = nullptr;
        b2Fixture* AFix = nullptr;
        Collider* B = nullptr;
        b2Fixture* BFix = nullptr;
    };

    static std::unordered_set<Collider*> m_objects;
    static std::list<m_contactData> m_beginContact;
    static std::list<m_contactData> m_endContact;
};

#endif
