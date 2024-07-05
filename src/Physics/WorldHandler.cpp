#include "Physics/WorldHandler.hpp"
#include "Physics/CollisionManager.hpp"

b2World WorldHandler::m_world({0.f,0.f});
bool WorldHandler::m_keepLostSimulationTime = true;
double WorldHandler::m_accumulate = 0;
int32 WorldHandler::m_tickRate = 60;
int32 WorldHandler::m_velocityIterations = 8;
int32 WorldHandler::m_positionIterations = 3;
int32 WorldHandler::m_maxUpdates = 8;
double WorldHandler::m_interpolateTime = 0;
double WorldHandler::m_maxInterpolateTime = 1/30;

void WorldHandler::init(const Vector2& gravity)
{
    m_world.SetGravity((b2Vec2)gravity);
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
    if (!m_keepLostSimulationTime)
        m_accumulate = m_accumulate > 1.f/m_tickRate ? 0.f : m_accumulate;
    m_interpolateTime = m_accumulate > m_maxInterpolateTime ? m_maxInterpolateTime : m_accumulate;
    while (updates > 0)
    {
        m_world.Step(1.f/m_tickRate, m_velocityIterations, m_positionIterations);
        updates--;
    }
}

double WorldHandler::getLeftOverTime()
{
    return m_accumulate;
}

void WorldHandler::setMaxInterpolationTime(double maxTime)
{
    m_maxInterpolateTime = maxTime;
}

double WorldHandler::getMaxInterpolationTime()
{
    return m_maxInterpolateTime;
}

double WorldHandler::getInterpolationTime()
{
    return m_interpolateTime;
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

void WorldHandler::setGravity(const Vector2& gravity)
{
    m_world.SetGravity((b2Vec2)gravity);
}

Vector2 WorldHandler::getGravity()
{
    return m_world.GetGravity();
}

void WorldHandler::setKeepLostSimulationTime(bool flag)
{
    m_keepLostSimulationTime = flag;
}

bool WorldHandler::isKeepLostSimulationTime()
{
    return m_keepLostSimulationTime;
}
