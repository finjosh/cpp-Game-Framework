#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#pragma once

#include "Vector2.hpp"
#include "Rotation.hpp"
#include "Transform.hpp"
#include "Color.hpp"

/*
- linear velocity
    - with curve
- acceleration
    - with curve
- color
    - curve over time
- rotational velocity
    - with curve
- Vector2 scale
    - with curve
*/

// TODO add the curves and make a curve class

class Particle : public Transform /*This particles transform*/
{
public:
    explicit Particle(float lifeTime);

    Vector2 velocity;
    /// @brief the speed at which the particle will rotate
    Rotation rotationalVelocity;
    /// @brief (1,1) is the same as the original shape
    Vector2 scale = Vector2{1,1};
    Color color;

protected:

private:
    float m_passedTime = 0.f;
    float m_lifeTime = 1.f;
};

#endif
