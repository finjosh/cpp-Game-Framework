#ifndef WORLD_HANDLER_HPP
#define WORLD_HANDLER_HPP

#pragma once

#include "box2d/box2d.h"
#include "Vector2.hpp"
#include <thread>
#include <atomic>

class WorldHandler
{
public:
    static WorldHandler* get();

    void init(const Vector2& gravity, unsigned int workerCount = std::thread::hardware_concurrency());
    /// @note only use this if you know what you are doing
    b2WorldId getWorld() const;
    void updateWorld(double deltaTime);
    /// @note only call this before using any physics
    /// @param ticksPerSecond the number of updates the physics engine will take per second
    void setTickRate(std::int32_t ticksPreSecond = 60);
    std::int32_t getTickRate() const;
    /// @brief sets how many substeps are taken per update
    /// @note only call this before using any physics
    void setSubstepCount(std::int32_t iterations = 4);
    std::int32_t getSubstepCount() const;
    /// @brief The max steps per frame
    void setMaxUpdates(std::int32_t maxUpdates = 8);
    std::int32_t getMaxUpdates() const;
    void setGravity(const Vector2& gravity);
    Vector2 getGravity() const;
    /// @returns the time that was not able to be calculated this frame
    double getLeftOverTime() const;
    /// @brief the max time that interpolation will be set to
    /// @note interpolationTime = leftOverTime > maxTime ? maxTime : leftOverTime
    void setMaxInterpolationTime(double maxTime);
    double getMaxInterpolationTime() const;
    /// @returns the amount of time that should be interpolated for
    double getInterpolationTime() const;
    /// @brief this affects what happens to physics at low frames
    /// @note any time left over after updating physics that is larger than 1/tickRate will be cleared if false
    /// @note default: True
    void setKeepLostSimulationTime(bool flag = true);
    bool isKeepLostSimulationTime() const;
    bool isInPhysicsUpdate() const;

protected:
    /// @returns the task that was enqueued which can be used to finish the task
    static void* enqueueTask(b2TaskCallback* task, int32_t itemCount, int32_t minRange, void* taskContext, void* userContext);
    static void finishTask(void* userTask, void* userContext);

private:
    WorldHandler() = default;

    b2WorldId m_world = b2_nullWorldId;
    std::atomic<bool> m_inPhysicsUpdate = false;
    /// @brief if at low frames do we keep the time lost and make up for it later?
    bool m_keepLostSimulationTime = true;
    double m_accumulate = 0;
    double m_interpolateTime = 0;
    double m_maxInterpolateTime = 1/30.0;
    std::int32_t m_tickRate = 60;
    std::int32_t m_substepCount = 4; 
    std::int32_t m_maxUpdates = 8;
};

#endif
