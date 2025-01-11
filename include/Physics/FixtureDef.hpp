#ifndef FIXTURE_DEF_HPP
#define FIXTURE_DEF_HPP

#pragma once

#include <box2d/types.h>

class Fixture;
class Collider;

// TODO make a simple construction with all the possible values being given
class FixtureDef 
{
public:
    // Getters
    /// @returns friction value, typically in the range [0,1]
    inline float getFriction() const { return m_shapeDef.friction; }
    /// @returns restitution, typically in the range [0,1]
    inline float getRestitution() const { return m_shapeDef.restitution; }
    /// @returns mass density, typically in kg/m^2
    inline float getDensity() const { return m_shapeDef.density; }
    /// @returns collision filter
    inline b2Filter getFilter() const { return m_shapeDef.filter; }
    /// @returns true if this is a sensor
    inline bool isSensor() const { return m_shapeDef.isSensor; }
    inline bool areSensorEventsEnabled() const { return m_shapeDef.enableSensorEvents; }
    inline bool areContactEventsEnabled() const { return m_shapeDef.enableContactEvents; }
    inline bool areHitEventsEnabled() const { return m_shapeDef.enableHitEvents; }
    inline bool arePreSolveEventsEnabled() const { return m_shapeDef.enablePreSolveEvents; }
    /// @brief Normally shapes on static bodies don't invoke contact creation when they are added to the world. This overrides
	/// @brief that behavior and causes contact creation. This significantly slows down static body creation which can be important
	/// @brief when there are many static shapes.
    inline bool isForceContactCreation() const { return m_shapeDef.forceContactCreation; }

    // Setters
    /// @brief set the friction, typically in the range [0,1]
    inline void setFriction(float friction) { m_shapeDef.friction = friction; }
    /// @brief set restitution (bounciness), typically in the range [0,1]
    inline void setRestitution(float restitution) { m_shapeDef.restitution = restitution; }
    /// @brief set the mass density, typically in kg/m^2
    inline void setDensity(float density) { m_shapeDef.density = density; }
    /// @brief set the current collision filter
    inline void setFilter(const b2Filter& filter = b2DefaultFilter()) { m_shapeDef.filter = filter; } // TODO make a really simple filter wrapper
    /// @brief set if this is a sensor
    inline void setAsSensor(bool isSensor) { m_shapeDef.isSensor = isSensor; }
    /// @brief Enable sensor events for this fixture
    inline void setEnableSensorEvents(bool enable = true) { m_shapeDef.enableSensorEvents = enable; }
    /// @brief Enable contact events for this fixture
    inline void setEnableContactEvents(bool enable = true) { m_shapeDef.enableContactEvents = enable; }
    /// @brief Enable contact hit events for this fixture
    /// @note default is false
    inline void setEnableHitEvents(bool enable = true) { m_shapeDef.enableHitEvents = enable; }
    /// @brief Enable pre-solve contact events for this fixture
    /// @note default is false
    inline void setEnablePreSolveEvents(bool enable = true) { m_shapeDef.enablePreSolveEvents = enable; }
    /// @brief Normally shapes on static bodies don't invoke contact creation when they are added to the world. This overrides
	/// @brief that behavior and causes contact creation. This significantly slows down static body creation which can be important
	/// @brief when there are many static shapes.
    inline void setForceContactCreation(bool forceContactCreation) { m_shapeDef.forceContactCreation = forceContactCreation; }

private:
    friend Fixture;
    friend Collider;

    b2ShapeDef m_shapeDef = b2DefaultShapeDef();
};

#endif
