#ifndef WORLDHANDLER_H
#define WORLDHANDLER_H

#pragma once

#include "box2d/Box2D.h"

const float PIXELS_PER_METER = 10;

class WorldHandler
{
public:
    static b2World& getWorld();
    static void updateWorld(const double& deltaTime);
    /// @note only call this before using any physics
    /// @param ticksPerSecond the number of updates the physics engine will take per second
    static void setTickRate(const int32& ticksPreSecond = 60);
    static int32 getTickRate();
    /// @note only call this before using any physics
    static void setVelocityIterations(const int32& iterations = 8);
    static int32 getVelocityIterations();
    /// @note only call this before using any physics
    static void setPositionIterations(const int32& iterations = 3);
    static int32 getPositionIterations();
    /// @brief The max steps per frame
    static void setMaxUpdates(const int32& maxUpdates = 8);
    static int32 getMaxUpdates();

private:
    inline WorldHandler() = default;
    
    static b2World _world;
    static double _accumulate;
    static int32 _tickRate;
    static int32 _velocityIterations;
    static int32 _positionIterations; 
    static int32 _maxUpdates;
};

#endif
