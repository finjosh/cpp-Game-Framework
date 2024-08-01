#ifndef WORLD_HANDLER_HPP
#define WORLD_HANDLER_HPP

#pragma once

#include "box2d/types.h"
#include <cstdint>
#define B2_MATH_H
#include "Vector2.hpp"

class WorldHandler
{
public:
    /// @warning this HAS to be called before doing anything with physics
    void init(const Vector2& gravity = {0,-9.8f});
    WorldHandler() = default;
    static WorldHandler& get();

    b2WorldId& getWorld();
    void updateWorld(double deltaTime);
    /// @note only call this before using any physics
    /// @param TickRate the number of times physics should update per second (default is 60 which is recommended by Box2d)
    void setTickRate(int32_t TickRate = 60.0);
    int32_t getTickRate() const;
    /// @note only call this before using any physics
    /// @note default is 4
    void setSubSteps(int32_t iterations = 4);
    int32_t getSubSteps() const;
    /// @brief The max steps per frame
    void setMaxUpdates(int32_t maxUpdates = 8);
    int32_t getMaxUpdates() const;
    void setGravity(const Vector2& gravity);
    Vector2 getGravity() const;
    /// @returns the time that was not able to be calculated this frame
    double getLeftOverTime() const;
    /// @brief the max time that interpolation will be set to
    /// @note leftOverTime > maxTime ? maxTime : leftOverTime
    void setMaxInterpolationTime(double maxTime);
    double getMaxInterpolationTime() const;
    /// @returns the amount of time that should be interpolated for
    double getInterpolationTime() const;
    /// @brief this affects what happens to physics at low frames
    /// @note any time left over after updating physics that is larger than 1/tickRate will be cleared
    /// @note default: True
    void setKeepLostSimulationTime(bool flag);
    bool isKeepLostSimulationTime() const;

private:
    b2WorldId m_world = b2_nullWorldId;
    /// @brief if at low frames do we keep the time lost and make up for it later?
    bool m_keepLostSimulationTime = true;
    double m_accumulate = 0;
    double m_interpolateTime = 0;
    double m_maxInterpolateTime = 1/30.0;
    int32_t m_tickRate = 60;
    double m_timeStep = 1.0/m_tickRate;
    int32_t m_subSteps = 4;
    int32_t m_maxUpdates = 1/30.0;
};


#endif
