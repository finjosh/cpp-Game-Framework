#ifndef JOINT_HPP
#define JOINT_HPP

#pragma once

#include "box2d/types.h"

class Collider;

/// @note this does not update when the collider is destroyed (only store in the colliders object or keep track if it is still alive)
class Joint
{
public:

protected:

private:
    b2JointId m_joint;
};

#endif
