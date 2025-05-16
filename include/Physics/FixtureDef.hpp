#ifndef FIXTURE_DEF_HPP
#define FIXTURE_DEF_HPP

#pragma once

#include <box2d/types.h>
#include "Physics/SurfaceMaterial.hpp"
#include "Physics/CollisionFilter.hpp"

// TODO make a simple construction with all the possible values being given
class FixtureDef 
{
public:
    /// @brief default FixtureDef
    FixtureDef();

    // Getters
    /// @returns mass density, typically in kg/m^2
    float getDensity() const;
    /// @returns collision filter
    Filter getFilter() const;
    /// @returns true if this is a sensor
    bool isSensor() const;
    bool areSensorEventsEnabled() const;
    bool areContactEventsEnabled() const;
    bool areHitEventsEnabled() const;
    bool arePreSolveEventsEnabled() const;
    /// @brief Normally shapes on static bodies don't invoke contact creation when they are added to the world. This overrides
	/// @brief that behavior and causes contact creation. This significantly slows down static body creation which can be important
	/// @brief when there are many static shapes.
    bool isInvokingContactCreation() const;
    /// @warning this is not a reference but a copy of the material
    SurfaceMaterial getSurfaceMaterial() const;
    /// @brief Custom debug draw color.
    uint32_t getCustomDebugColor() const;
    /// @brief The Coulomb (dry) friction coefficient, usually in the range [0,1].
    float getFriction() const;
    /// @brief The coefficient of restitution (bounce) usually in the range [0,1].
    float getRestitution() const;
    /// @brief The rolling resistance usually in the range [0,1].
    float getRollingRestitution() const;
    /// @brief The tangent speed for conveyor belts
    float getTangentSpeed() const;
    /// @brief The material id
    /// @note this is not used internally. This is passed with query results and to friction and restitution combining functions.
    /// @returns the current user material id
    int getUserMaterialId() const;

    // Setters
    /// @brief set the mass density, typically in kg/m^2
    void setDensity(float density);
    /// @brief set the current collision filter
    void setFilter(const Filter& filter);
    /// @brief set if this is a sensor
    void setAsSensor(bool isSensor);
    /// @brief Enable sensor events for this fixture
    void enableSensorEvents(bool enable = true);
    /// @brief Enable contact events for this fixture
    void enableContactEvents(bool enable = true);
    /// @brief Enable pre-solve contact events for this fixture
    /// @note default is false
    void enablePreSolveEvents(bool enable = true);
    /// @brief Normally shapes on static bodies don't invoke contact creation when they are added to the world. This overrides
	/// @brief that behavior and causes contact creation. This significantly slows down static body creation which can be important
	/// @brief when there are many static shapes.
    void setInvokeContactCreation(bool forceContactCreation);
    void setSurfaceMaterial(const SurfaceMaterial& material);
    /// @brief Custom debug draw color.
    void setCustomDebugColor(uint32_t color);
    /// @brief The Coulomb (dry) friction coefficient, usually in the range [0,1].
    void setFriction(float friction);
    /// @brief The coefficient of restitution (bounce) usually in the range [0,1].
    void setRestitution(float restitution);
    /// @brief The rolling resistance usually in the range [0,1].
    void setRollingRestitution(float rollingResistance);
    /// @brief The tangent speed for conveyor belts
    void setTangentSpeed(float tangentSpeed);
    /// @note this is not used internally. This is passed with query results and to friction and restitution combining functions.
    /// @param id the new user material id
    void setUserMaterialId(int id);

private:
    friend class Fixture;
    friend class Collider;

    b2ShapeDef m_shapeDef;
};

#endif
