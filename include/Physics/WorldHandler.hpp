#ifndef WORLDHANDLER_H
#define WORLDHANDLER_H

#pragma once

#include "box2d/Box2D.h"
#include "Vector2.hpp"

class WorldHandler
{
public:
    static void init(const Vector2& gravity);
    static b2World& getWorld();
    static void updateWorld(double deltaTime);
    /// @note only call this before using any physics
    /// @param ticksPerSecond the number of updates the physics engine will take per second
    static void setTickRate(int32 ticksPreSecond = 60);
    static int32 getTickRate();
    /// @note only call this before using any physics
    static void setVelocityIterations(int32 iterations = 8);
    static int32 getVelocityIterations();
    /// @note only call this before using any physics
    static void setPositionIterations(int32 iterations = 3);
    static int32 getPositionIterations();
    /// @brief The max steps per frame
    static void setMaxUpdates(int32 maxUpdates = 8);
    static int32 getMaxUpdates();
    static void setGravity(const Vector2& gravity);
    static Vector2 getGravity();
    /// @returns the time that was not able to be calculated this frame
    static float getLeftOverTime();

private:
    inline WorldHandler() = default;
    
    static b2World m_world;
    static double m_accumulate;
    static int32 m_tickRate;
    static int32 m_velocityIterations;
    static int32 m_positionIterations; 
    static int32 m_maxUpdates;
};

#endif
