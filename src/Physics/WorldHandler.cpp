#include "Physics/WorldHandler.hpp"

b2World WorldHandler::_world({0.f,0.f});
double WorldHandler::_accumulate = 0;
int32 WorldHandler::_tickRate = 60;
int32 WorldHandler::_velocityIterations = 8;
int32 WorldHandler::_positionIterations = 3;
int32 WorldHandler::_maxUpdates = 8;

b2World& WorldHandler::getWorld()
{
    return _world;
}

// TODO make a simple lerp function to fix "jittery" physics
void WorldHandler::updateWorld(const double& deltaTime)
{
    _accumulate += deltaTime;
    int32 updates = std::min(int(_accumulate*_tickRate), _maxUpdates);
    _accumulate -= updates*(1.f/_tickRate);
    while (updates > 0)
    {
        _world.Step(1.f/_tickRate, int32(8), int32(3));
        updates--;
    }
}

void WorldHandler::setTickRate(const int32& interval)
{
    _tickRate = interval;
}

int32 WorldHandler::getTickRate()
{
    return _tickRate;
}

void WorldHandler::setVelocityIterations(const int32& iterations)
{
    _velocityIterations = iterations;
}

int32 WorldHandler::getVelocityIterations()
{
    return _velocityIterations;
}

void WorldHandler::setPositionIterations(const int32& iterations)
{
    _positionIterations = iterations;
}

int32 WorldHandler::getPositionIterations()
{
    return _positionIterations;
}
