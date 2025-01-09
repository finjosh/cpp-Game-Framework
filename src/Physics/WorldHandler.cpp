#include "Physics/WorldHandler.hpp"
#include "Physics/CollisionManager.hpp"

#ifdef DEBUG
#define CHECK_IF_IN_PHYSICS_UPDATE() assert(CollisionManager::get()->m_inPhysicsUpdate == 0 && "Must not read/write to any physics related data during a physics update")
#else
#define CHECK_IF_IN_PHYSICS_UPDATE()
#endif

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

b2WorldId WorldHandler::getWorld()
{
    CHECK_IF_IN_PHYSICS_UPDATE();
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
    CHECK_IF_IN_PHYSICS_UPDATE();
    CHECK_VALID_WORLD();
    b2World_SetGravity(m_world, (b2Vec2)gravity);
}

Vector2 WorldHandler::getGravity()
{
    CHECK_IF_IN_PHYSICS_UPDATE();
    CHECK_VALID_WORLD();
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
