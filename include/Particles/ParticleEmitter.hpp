#ifndef PARTICLE_EMITTER_HPP
#define PARTICLE_EMITTER_HPP

#pragma once

#include <list>
#include <math.h>

#include "SFML/Graphics/Shape.hpp"

#include "UpdateInterface.hpp"
#include "Graphics/DrawableObject.hpp"
#include "Object.hpp"

class ParticleEmitter : virtual public Object, public UpdateInterface, public DrawableObject
{
public:
    using Ptr = Object::Ptr<ParticleEmitter>;

    /// @param shape make sure that the given drawable is accessible over the lifetime of this object
    ParticleEmitter(sf::Shape* shape);
    /// @param shape make sure that the given drawable is accessible over the lifetime of this object
    /// @param pos the position of the emitter
    /// @param vel the initial velocity of the particles
    /// @param rotation the rotation in radians
    /// @param spawnRate the spawn rate of particles if spawning
    /// @param lifetime the particle lifetime
    /// @param spawnAmount the amount of partials spawned at once
    /// @param fadeOutTime the time for a particle to fade out 
    /// @param spread the max angle of spread (degrees)
    /// @param randomColor the max random addition to the default color
    ParticleEmitter(sf::Shape* shape,
                    const Vector2& pos, float vel, Rotation rotation = 0.f, float spawnRate = 0.1f, 
                    float lifetime = 1.f, int spawnAmount = 0, float fadeOutTime = 0.f, 
                    float spread = 0.f, sf::Color randomColor = sf::Color{0,0,0,0});

    /// @brief particles will still be emitted but not rendered
    /// @note remove the shape before deleting it
    void removeShape();
    /// @warning shape has to be accessible over the lifetime of this object
    void setShape(sf::Shape* shape);
    const sf::Shape* getShape() const;
    sf::Shape* getShape();

    /// @brief if the particles are spawning
    bool isSpawning() const;
    /// @brief if the particles are spawning
    void setSpawning(bool value = true);
    /// @brief the spawn rate of particles if spawning
    float getSpawnRate() const;
    /// @brief the spawn rate of particles if spawning
    void setSpawnRate(float value = 0.1f);
    /// @brief the amount of random in the spawn rate
    float getRandomSpawnRate() const;
    /// @brief the amount of random in the spawn rate
    void setRandomSpawnRate(float value = 0.f);
    /// @brief the amount of partials spawned at once
    int getSpawnAmount() const;
    /// @brief the amount of partials spawned at once
    void setSpawnAmount(int value = 1);
    /// @brief the greatest random spawn amount (subtracts from spawn amount) 1-(RAND_MAX)
    unsigned int getRandomSpawnAmount() const;
    /// @brief the greatest random spawn amount (subtracts from spawn amount) 1-(RAND_MAX)
    void setRandomSpawnAmount(unsigned int value = 1);
    /// @brief the max angle of spread
    float getSpread() const;
    /// @brief the max angle of spread (degrees)
    void setSpread(float value = 0.f);
    /// @brief how random the rotation will be
    float getRandomRotation() const;
    /// @brief how random the rotation will be
    /// @note this is the rotation of the shape
    void setRandomRotation(float value = 0.1f);
    /// @brief the particle lifetime
    float getLifetime() const;
    /// @brief the particle lifetime
    void setLifetime(float value = 1.f);
    /// @brief the time for a particle to fade out
    float getFadeOutTime() const;
    /// @brief the time for a particle to fade out
    void setFadeOutTime(float value = 0.f);
    /// @brief the initial velocity of the particles
    float getVelocity() const;
    /// @brief the initial velocity of the particles
    void setVelocity(float value = 0.f);
    float getRandomVelocity() const;
    /// @brief how much the velocity can differ + or -
    void setRandomVelocity(float value = 0.f);
    /// @param color each value in color is the max random value   
    void setRandomColor(sf::Color color);
    sf::Color getRandomColor() const;
    /// @brief if not set this will be set the the given shapes color
    void setDefaultColor(sf::Color color);
    sf::Color getDefaultColor() const;
    /// @brief force emits the particles ignoring the spawn rate
    void emit();
    /// @brief removes all particles
    void clear();

protected:
    virtual void Update(float deltaTime) override;
    virtual void Draw(sf::RenderTarget* target, const Transform& parentTransform) override;

    /// @returns a random float from 0 to the given x
    float randX(float x);

    class Particle
    {
    public:
        Particle() = default;
        Particle(const Vector2& pos, const Vector2& vel, sf::Angle rotation, sf::Color color);
        void setPosition(const Vector2& position);
        Vector2 getPosition() const;
        void setVelocity(const Vector2& velocity);
        Vector2 getVelocity() const;
        /// @param rot in degrees
        void setRotation(sf::Angle rot);
        /// @returns rotation in degrees
        sf::Angle getRotation() const;
        float getLifetime() const;
        sf::Color getColor() const;
        void setColor(sf::Color color);
        void setAlpha(int8_t alpha);

        void move(const Vector2& vec);
        void draw(sf::Shape* shape, sf::RenderTarget* target);

    protected:
        /// @brief updates position based on velocity
        void update(float deltaTime);

        friend ParticleEmitter;

    private:
        Vector2 m_position = {0,0};
        Vector2 m_velocity = {0,0};
        /// @brief in degrees
        sf::Angle m_rotation = sf::Angle::Zero;
        float m_lifetime = 0.f;
        sf::Color m_color;
    };

private:
    sf::Shape* m_particleShape = nullptr;

    /// @brief if the particles are spawning
    bool m_spawning = false;
    /// @brief the spawn rate of particles if spawning
    float m_spawnRate = 0.1f;
    float m_spawnTimer = 0.f;
    /// @brief the amount of random in the spawn rate
    float m_randomSpawnRate = 0.f;
    /// @brief the amount of partials spawned at once
    int m_spawnAmount = 1;
    /// @brief the greatest random spawn amount (subtracts from spawn amount)
    unsigned int m_randomSpawnAmount = 1;
    /// @brief the max angle of spread
    float m_spread = 0.f;
    /// @brief how random the rotation will be
    float m_randomRotation = 0.1f;
    /// @brief the particle lifetime
    float m_lifetime = 1.f;
    /// @brief the time for a particle to fade out
    float m_fadeOutTime = 0.f;
    /// @brief the initial velocity of the particles
    float m_velocity = 0.f;
    /// @brief the randomness of the velocity
    float m_randomVelocity = 0.f;
    /// @brief the max random colour value for r,g,b,a
    sf::Color m_randomColor = {0,0,0,0};
    sf::Color m_defaultColor = {255,255,255,255};

    std::list<Particle> m_particles;
};

#endif
