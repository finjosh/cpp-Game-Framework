#include "Physics/WorldHandler.hpp"
#include "Physics/CollisionManager.hpp"
#include "box2d/box2d.h"

void WorldHandler::init(const Vector2& gravity)
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2)gravity;
    m_world = b2CreateWorld(&worldDef);
    b2World_SetPreSolveCallback(m_world, (b2PreSolveFcn*)(&CollisionManager::b2PreSolveFcn), (void*)(&CollisionManager::get()));
    // m_world.SetContactListener(new CollisionManager());
}

WorldHandler& WorldHandler::get()
{
    static WorldHandler world;
    return world;
}

b2WorldId& WorldHandler::getWorld()
{
    return m_world;
}

void WorldHandler::updateWorld(double deltaTime)
{
    assert("World Handler must be initalized before trying to update" && b2World_IsValid(m_world));

    m_accumulate += deltaTime;
    int32_t updates = std::min(int(m_accumulate*m_tickRate), m_maxUpdates);
    m_accumulate -= updates*(m_timeStep);
    if (!m_keepLostSimulationTime)
        m_accumulate = m_accumulate > m_timeStep ? 0.f : m_accumulate;
    m_interpolateTime = m_accumulate > m_maxInterpolateTime ? m_maxInterpolateTime : m_accumulate;
    while (updates > 0)
    {
        b2World_Step(m_world, m_timeStep, 4);
        updates--;
    }

    CollisionManager::get().Update();
}

double WorldHandler::getLeftOverTime() const
{
    return m_accumulate;
}

void WorldHandler::setMaxInterpolationTime(double maxTime)
{
    m_maxInterpolateTime = maxTime;
}

double WorldHandler::getMaxInterpolationTime() const
{
    return m_maxInterpolateTime;
}

double WorldHandler::getInterpolationTime() const
{
    return m_interpolateTime;
}

void WorldHandler::setTickRate(int32_t tickRate)
{
    m_tickRate = tickRate;
    m_timeStep = 1.0/tickRate;
}

int32_t WorldHandler::getTickRate() const
{
    return m_tickRate;
}

void WorldHandler::setMaxUpdates(int32_t maxUpdates)
{
    m_maxUpdates = maxUpdates;
}

int32_t WorldHandler::getMaxUpdates() const
{
    return m_maxUpdates;
}

void WorldHandler::setGravity(const Vector2& gravity)
{
    b2World_SetGravity(m_world, (b2Vec2)gravity);
}

Vector2 WorldHandler::getGravity() const
{
    return b2World_GetGravity(m_world);
}

void WorldHandler::setKeepLostSimulationTime(bool flag)
{
    m_keepLostSimulationTime = flag;
}

bool WorldHandler::isKeepLostSimulationTime() const
{
    return m_keepLostSimulationTime;
}
