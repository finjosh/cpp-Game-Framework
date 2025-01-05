#ifndef WORLD_HANDLER_HPP
#define WORLD_HANDLER_HPP

#pragma once

#include "box2d/box2d.h"
#include "Vector2.hpp"

class WorldHandler
{
public:
    static void init(const Vector2& gravity);
    static b2WorldId getWorld();
    static void updateWorld(double deltaTime);
    /// @note only call this before using any physics
    /// @param ticksPerSecond the number of updates the physics engine will take per second
    static void setTickRate(std::int32_t ticksPreSecond = 60);
    static std::int32_t getTickRate();
    /// @brief sets how many substeps are taken per update
    /// @note only call this before using any physics
    static void setSubstepCount(std::int32_t iterations = 4);
    static std::int32_t getSubstepCount();
    /// @brief The max steps per frame
    static void setMaxUpdates(std::int32_t maxUpdates = 8);
    static std::int32_t getMaxUpdates();
    static void setGravity(const Vector2& gravity);
    static Vector2 getGravity();
    /// @returns the time that was not able to be calculated this frame
    static double getLeftOverTime();
    /// @brief the max time that interpolation will be set to
    /// @note interpolationTime = leftOverTime > maxTime ? maxTime : leftOverTime
    static void setMaxInterpolationTime(double maxTime);
    static double getMaxInterpolationTime();
    /// @returns the amount of time that should be interpolated for
    static double getInterpolationTime();
    /// @brief this affects what happens to physics at low frames
    /// @note any time left over after updating physics that is larger than 1/tickRate will be cleared if false
    /// @note default: True
    static void setKeepLostSimulationTime(bool flag = true);
    static bool isKeepLostSimulationTime();
    

private:
    inline WorldHandler() = default;
    
    static b2WorldId m_world;
    /// @brief if at low frames do we keep the time lost and make up for it later?
    static bool m_keepLostSimulationTime;
    static double m_accumulate;
    static double m_interpolateTime;
    static double m_maxInterpolateTime;
    static std::int32_t m_tickRate;
    static std::int32_t m_substepCount; 
    static std::int32_t m_maxUpdates;
};

#endif
