#ifndef PARTICLE_EMITTER_HPP
#define PARTICLE_EMITTER_HPP

#pragma once

// #include "SFML/Graphics/VertexArray.hpp"
#include <list>
#include <ctype.h>
#include <limits.h>
#include "Vector2.hpp"

class Particle;

/// @brief base class for all particle emitters
class ParticleEmitter
{
public:
    virtual ~ParticleEmitter() = default;

    virtual void emit(size_t amount, Vector2 centerPos) = 0;
    void setEmitting(bool state = true);
    bool isEmitting() const;

    void updateTime(float time);

    //* RandomData
    float randomEmissionRate = 0.f;
    float randomSpawnAmount = 0.f;

protected:

private:
    //* RegularData
    float m_emissionRate = 1;
    float m_accumulate = 0.f;
    size_t m_maxParticles = SIZE_MAX;
    bool m_emitting = false;

    std::list<Particle> m_particles;
};

#endif
