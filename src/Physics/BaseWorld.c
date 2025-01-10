#include "Physics/BaseWorld.h"
#include "src/world.h"
#include "src/shape.h"

b2Contact* getWorldContacts(b2WorldId world)
{
    return b2GetWorldFromId(world)->contactArray;
}

b2Contact* getWorldContactsFromWorld(void* worldPtr)
{
    return ((b2World*)worldPtr)->contactArray;
}

int getWorldContactCountFromWorld(void* worldPtr)
{
    return b2Array((b2World*)worldPtr).count;
}

int getWorldContactCount(b2WorldId world)
{
    return b2Array(b2GetWorldFromId(world)->contactArray).count;
}

_b2ContactData getContactData(b2WorldId worldId, b2Contact* contact)
{
    _b2ContactData rtn = {0};
    b2World* world = b2GetWorldFromId(worldId);
    b2ContactSim* contactSim = b2GetContactSim(world, contact);
    rtn.manifold = &contactSim->manifold;

    b2Shape* shapeA = world->shapeArray + contact->shapeIdA;
    b2Shape* shapeB = world->shapeArray + contact->shapeIdB;
    rtn.shapeIdA = (b2ShapeId){shapeA->id + 1, worldId.index1 - 1, shapeA->revision};
    rtn.shapeIdB = (b2ShapeId){shapeB->id + 1, worldId.index1 - 1, shapeB->revision};

    return rtn;
}

void* getWorld(b2WorldId world)
{
    return (void*)b2GetWorldFromId(world);
}

_b2ContactData getContactDataFromWorld(void* worldPtr, b2Contact* contact)
{
    _b2ContactData rtn = {0};
    b2World* world = (b2World*)worldPtr;
    b2ContactSim* contactSim = b2GetContactSim(world, contact);
    rtn.manifold = &contactSim->manifold;

    b2Shape* shapeA = world->shapeArray + contact->shapeIdA;
    b2Shape* shapeB = world->shapeArray + contact->shapeIdB;
    rtn.shapeIdA = (b2ShapeId){shapeA->id + 1, world->worldId, shapeA->revision};
    rtn.shapeIdB = (b2ShapeId){shapeB->id + 1, world->worldId, shapeB->revision};

    return rtn;
}
