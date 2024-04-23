#include "Physics/CollisionManager.hpp"
#include "Physics/Collider.hpp"
#include "ObjectManager.hpp"

std::unordered_set<Collider*> CollisionManager::_objects;
std::list<std::pair<Collider*, CollisionData>> CollisionManager::_beginContact;
std::list<std::pair<Collider*, CollisionData>> CollisionManager::_endContact;

void CollisionManager::BeginContact(b2Contact* contact)
{
    Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);
    if (A != nullptr)
    {
        _beginContact.push_back({A, {B, contact->GetFixtureA(), contact->GetFixtureB()}});
        // A->BeginContact({B, contact->GetFixtureA(), contact->GetFixtureB()});
    }
    if (B != nullptr)
    {
        _beginContact.push_back({B, {A, contact->GetFixtureB(), contact->GetFixtureA()}});
        // B->BeginContact({A, contact->GetFixtureB(), contact->GetFixtureA()});
    }
}

void CollisionManager::EndContact(b2Contact* contact)
{
    Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);
    // TODO fix this
    if (A == nullptr || B == nullptr) return;
    // if (A != nullptr)
    // {
        _endContact.push_back({A, {B, contact->GetFixtureA(), contact->GetFixtureB()}});
        // A->EndContact({B, contact->GetFixtureA(), contact->GetFixtureB()});
    // }
    // if (B != nullptr)
    // {
        _endContact.push_back({B, {A, contact->GetFixtureB(), contact->GetFixtureA()}});
        // B->EndContact({A, contact->GetFixtureB(), contact->GetFixtureA()});
    // }
}

void CollisionManager::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    b2Body* body = contact->GetFixtureA()->GetBody();
    if (body != nullptr)
    {
        Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
        collider->PreSolve(contact, oldManifold);
    }
    body = contact->GetFixtureB()->GetBody();
    if (body != nullptr)
    {
        Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
        collider->PreSolve(contact, oldManifold);
    }
}

void CollisionManager::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    b2Body* body = contact->GetFixtureA()->GetBody();
    if (body != nullptr)
    {
        Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
        collider->PostSolve(contact, impulse);
    }
    body = contact->GetFixtureB()->GetBody();
    if (body != nullptr)
    {
        Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
        collider->PostSolve(contact, impulse);
    }
}

void CollisionManager::Update()
{
    for (auto data: _beginContact)
    {
        data.first->BeginContact(data.second);
    }
    _beginContact.clear();

    // TODO fix bug where box2d calls end contact after the body is destroyed and then this crashes
    for (auto data: _endContact)
    {
        data.first->EndContact(data.second);
    }
    _endContact.clear();

    ObjectManager::ClearDestroyQueue();

    for (auto obj: _objects)
    {
        obj->_update();
    }
}

void CollisionManager::addCollider(Collider* Collider)
{
    _objects.insert({Collider});
}

void CollisionManager::removeCollider(Collider* collider)
{
    _objects.erase({collider});
}
