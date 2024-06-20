#ifndef JOINT_H
#define JOINT_H

#pragma once

#include "Box2D/b2_joint.h"

class Collider;

/// @note this does not update when the collider is destroyed (only store in the colliders object or keep track if it is still alive)
class Joint
{
public:

protected:

private:
    b2Joint* m_joint;
};

#endif
