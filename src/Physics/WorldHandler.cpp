#include "Physics/WorldHandler.hpp"
#include "Physics/CollisionManager.hpp"

b2World WorldHandler::m_world({0.f,0.f});
double WorldHandler::m_accumulate = 0;
int32 WorldHandler::m_tickRate = 60;
int32 WorldHandler::m_velocityIterations = 8;
int32 WorldHandler::m_positionIterations = 3;
int32 WorldHandler::m_maxUpdates = 8;

void WorldHandler::init(const b2Vec2& gravity)
{
    m_world.SetGravity(gravity);
    m_world.SetContactListener(new CollisionManager());
}

b2World& WorldHandler::getWorld()
{
    return m_world;
}

void WorldHandler::updateWorld(double deltaTime)
{
    m_accumulate += deltaTime;
    int32 updates = std::min(int(m_accumulate*m_tickRate), m_maxUpdates);
    m_accumulate -= updates*(1.f/m_tickRate);
    while (updates > 0)
    {
        m_world.Step(1.f/m_tickRate, m_velocityIterations, m_positionIterations);
        updates--;
    }
}

void WorldHandler::setTickRate(int32 interval)
{
    m_tickRate = interval;
}

int32 WorldHandler::getTickRate()
{
    return m_tickRate;
}

void WorldHandler::setVelocityIterations(int32 iterations)
{
    m_velocityIterations = iterations;
}

int32 WorldHandler::getVelocityIterations()
{
    return m_velocityIterations;
}

void WorldHandler::setPositionIterations(int32 iterations)
{
    m_positionIterations = iterations;
}

int32 WorldHandler::getPositionIterations()
{
    return m_positionIterations;
}

void WorldHandler::setGravity(const b2Vec2& gravity)
{
    m_world.SetGravity(gravity);
}

b2Vec2 WorldHandler::getGravity()
{
    return m_world.GetGravity();
}
