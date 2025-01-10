#include "Physics/WorldHandler.hpp"
#include "Physics/CollisionManager.hpp"

#define CHECK_VALID_WORLD() assert(b2World_IsValid(m_world) && "World must be initalized before use!")

WorldHandler* WorldHandler::get()
{
    static WorldHandler handler;
    return &handler;
}

void WorldHandler::init(const Vector2& gravity, unsigned int workerCount)
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2)gravity;
    worldDef.workerCount = workerCount == 0 ? 1 : workerCount;

    m_world = b2CreateWorld(&worldDef);
    CollisionManager::initWorkerThreadLists(worldDef.workerCount);
    b2World_SetPreSolveCallback(m_world, &CollisionManager::PreSolve, CollisionManager::get());
}

b2WorldId WorldHandler::getWorld() const
{
    return m_world;
}

void WorldHandler::updateWorld(double deltaTime)
{
    CHECK_VALID_WORLD();
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

void WorldHandler::setTickRate(std::int32_t interval)
{
    m_tickRate = interval;
}

std::int32_t WorldHandler::getTickRate() const
{
    return m_tickRate;
}

void WorldHandler::setSubstepCount(std::int32_t iterations)
{
    m_substepCount = iterations;
}

std::int32_t WorldHandler::getSubstepCount() const
{
    return m_substepCount;
}

void WorldHandler::setMaxUpdates(std::int32_t maxUpdates)
{
    m_maxUpdates = maxUpdates;
}

std::int32_t WorldHandler::getMaxUpdates() const
{
    return m_maxUpdates;
}

void WorldHandler::setGravity(const Vector2& gravity)
{
    assert(!m_inPhysicsUpdate && "Can not change physics data while in physics update");
    CHECK_VALID_WORLD();
    b2World_SetGravity(m_world, (b2Vec2)gravity);
}

Vector2 WorldHandler::getGravity() const
{
    CHECK_VALID_WORLD();
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

bool WorldHandler::isInPhysicsUpdate() const
{
    return m_inPhysicsUpdate;
}
