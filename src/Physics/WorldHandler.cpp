#include "Physics/WorldHandler.hpp"
#include "Physics/CollisionManager.hpp"
#include "ThreadPool.hpp"

#define CHECK_VALID_WORLD() assert(b2World_IsValid(m_world) && "World must be initalized before use!")

WorldHandler& WorldHandler::get()
{
    static WorldHandler handler;
    return handler;
}

void WorldHandler::init(const Vector2& gravity, unsigned int workerCount)
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2)gravity;
    worldDef.workerCount = workerCount == 0 ? 1 : workerCount;
    worldDef.finishTask = &WorldHandler::finishTask;
    worldDef.enqueueTask = &WorldHandler::enqueueTask;

    m_world = b2CreateWorld(&worldDef);
    CollisionManager::initWorkerThreadLists(worldDef.workerCount);
    b2World_SetPreSolveCallback(m_world, &CollisionManager::PreSolve, CollisionManager::get());
}

void* WorldHandler::enqueueTask(b2TaskCallback* task, int32_t itemCount, int32_t minRange, void* taskContext, void* userContext)
{
    std::list<std::future<void>>* tasks = new std::list<std::future<void>>();
    std::int32_t end = 0;
    std::int32_t i = 0;
    std::int32_t range = std::ceil(itemCount/(float)ThreadPool::get().get_thread_ids().size());
    range = range < minRange ? minRange : range;
    for (std::int32_t start = 0; end < itemCount; start += range)
    {
        assert(i < ThreadPool::get().get_thread_ids().size() && "Too many tasks for the thread pool");
        end = start + range;
        end = end > itemCount ? itemCount : end;
        tasks->emplace_back(
            ThreadPool::get().submit_task([task, start, end, i, taskContext]()
            { 
                task(start, end, i, taskContext);
            })
        );
        i++;
    }
    return tasks;
}

void WorldHandler::finishTask(void* userTask, void* userContext)
{   
    // ThreadPool::get().detach_task(userTask);
    for (auto& task : *(static_cast<std::list<std::future<void>>*>(userTask)))
        task.wait();
    delete static_cast<std::list<std::future<void>>*>(userTask);
}

b2WorldId WorldHandler::getWorld() const
{
    CHECK_VALID_WORLD();
    return m_world;
}

void WorldHandler::updateWorld(double deltaTime)
{
    CHECK_VALID_WORLD();
    m_accumulate += deltaTime;
    std::int32_t updates = std::min(int(m_accumulate*m_tickRate), m_maxUpdates);
    double tickTime = 1.0/m_tickRate;
    m_accumulate -= updates*(tickTime);
    if (!m_keepLostSimulationTime)
        m_accumulate = m_accumulate > tickTime ? 0.f : m_accumulate;
    m_interpolateTime = m_accumulate > m_maxInterpolateTime ? m_maxInterpolateTime : m_accumulate;
    while (updates > 0)
    {
        b2World_Step(m_world, tickTime, m_substepCount);
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

double WorldHandler::getMaxDeltaTime() const
{
    return m_maxDeltaTime;
}

void WorldHandler::setTickRate(std::int32_t interval)
{
    m_tickRate = interval;
    m_maxDeltaTime = 1.0/m_tickRate*m_maxUpdates;
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
    m_maxDeltaTime = 1.0/m_tickRate*m_maxUpdates;
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

void WorldHandler::explode(ExplosionDef def)
{
    b2ExplosionDef input = b2DefaultExplosionDef();
    input.radius = def.radius;
    input.position = (b2Vec2)def.position;
    input.impulsePerLength = def.impulsePerLength;
    b2World_Explode(m_world, &input);
}

int WorldHandler::getFixtureCount() const
{
    return b2World_GetCounters(m_world).shapeCount;
}
