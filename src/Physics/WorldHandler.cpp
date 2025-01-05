#include "Physics/WorldHandler.hpp"
#include "Physics/CollisionManager.hpp"

b2WorldId WorldHandler::m_world;
bool WorldHandler::m_keepLostSimulationTime = true;
double WorldHandler::m_accumulate = 0;
std::int32_t WorldHandler::m_tickRate = 60;
std::int32_t WorldHandler::m_substepCount = 4;
std::int32_t WorldHandler::m_maxUpdates = 8;
double WorldHandler::m_interpolateTime = 0;
double WorldHandler::m_maxInterpolateTime = 1/30.0;

void WorldHandler::init(const Vector2& gravity)
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2)gravity;

    m_world = b2CreateWorld(&worldDef);
    // m_world.SetContactListener(new CollisionManager());
}

b2WorldId WorldHandler::getWorld()
{
    return m_world;
}

void WorldHandler::updateWorld(double deltaTime)
{
    m_accumulate += deltaTime;
    std::int32_t updates = std::min(int(m_accumulate*m_tickRate), m_maxUpdates);
    m_accumulate -= updates*(1.f/m_tickRate);
    if (!m_keepLostSimulationTime)
        m_accumulate = m_accumulate > 1.f/m_tickRate ? 0.f : m_accumulate;
    m_interpolateTime = m_accumulate > m_maxInterpolateTime ? m_maxInterpolateTime : m_accumulate;
    while (updates > 0)
    {
        b2World_Step(m_world, 1.f/m_tickRate, m_substepCount);
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

void WorldHandler::setTickRate(std::int32_t interval)
{
    m_tickRate = interval;
}

std::int32_t WorldHandler::getTickRate()
{
    return m_tickRate;
}

void WorldHandler::setSubstepCount(std::int32_t iterations)
{
    m_substepCount = iterations;
}

std::int32_t WorldHandler::getSubstepCount()
{
    return m_substepCount;
}

void WorldHandler::setMaxUpdates(std::int32_t maxUpdates)
{
    m_maxUpdates = maxUpdates;
}

std::int32_t WorldHandler::getMaxUpdates()
{
    return m_maxUpdates;
}

void WorldHandler::setGravity(const Vector2& gravity)
{
    b2World_SetGravity(m_world, (b2Vec2)gravity);
}

Vector2 WorldHandler::getGravity()
{
    return b2World_GetGravity(m_world);
}

void WorldHandler::setKeepLostSimulationTime(bool flag)
{
    m_keepLostSimulationTime = flag;
}

bool WorldHandler::isKeepLostSimulationTime()
{
    return m_keepLostSimulationTime;
}
