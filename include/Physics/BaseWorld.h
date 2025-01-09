#ifndef BASE_WORLD_H
#define BASE_WORLD_H

#pragma once

#include "src/contact.h"

// DONT USE THIS unless you know what you are doing
b2Contact* getWorldContacts(b2WorldId world);
int getWorldContactCount(b2WorldId world);

typedef struct _b2ContactData
{
    b2ShapeId shapeIdA;
    b2ShapeId shapeIdB;
    b2Manifold manifold;
} _b2ContactData;

_b2ContactData getContactData(b2WorldId world, b2Contact* contact);

#endif
