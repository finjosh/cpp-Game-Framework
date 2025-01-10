#ifndef BASE_WORLD_H
#define BASE_WORLD_H

#pragma once

#include "src/contact.h"

/// @brief DONT USE THIS unless you know what you are doing
b2Contact* getWorldContacts(b2WorldId world);
/// @brief DONT USE THIS unless you know what you are doing
/// @warning world MUST be a stored in a void* b2World*
b2Contact* getWorldContactsFromWorld(void* worldPtr);
/// @brief DONT USE THIS unless you know what you are doing
int getWorldContactCount(b2WorldId world);
/// @brief DONT USE THIS unless you know what you are doing
/// @warning world MUST be a stored in a void* b2World*
int getWorldContactCountFromWorld(void* worldPtr);
/// @brief DONT USE THIS unless you know what you are doing
void* getWorld(b2WorldId world);

typedef struct _b2ContactData
{
    b2ShapeId shapeIdA;
    b2ShapeId shapeIdB;
    b2Manifold* manifold;
} _b2ContactData;

/// @brief DONT USE THIS unless you know what you are doing
_b2ContactData getContactData(b2WorldId world, b2Contact* contact);
/// @brief DONT USE THIS unless you know what you are doing
/// @warning world MUST be a stored in a void* b2World*
_b2ContactData getContactDataFromWorld(void* world, b2Contact* worldPtr);

#endif
