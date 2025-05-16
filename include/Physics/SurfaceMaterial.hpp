#ifndef SURFACE_MATERIAL_HPP
#define  SURFACE_MATERIAL_HPP

#pragma once

#include <box2d/types.h>

class FixtureDef;

// TODO add functionality in querying
class SurfaceMaterial
{
public:
    /// @brief Default surface material constructor
    inline SurfaceMaterial() = default;
    inline SurfaceMaterial(const b2SurfaceMaterial& material) : m_material(material) {}
    inline SurfaceMaterial(const SurfaceMaterial& material) : m_material(material.m_material) {}
    /// @brief The material id 
    /// @note this is not used internally. This is passed with query results and to friction and restitution combining functions.
    /// @returns the current user material id
    inline int getUserMaterialId() const { return m_material.userMaterialId; }
    /// @brief The material id 
    /// @note this is not used internally. This is passed with query results and to friction and restitution combining functions.
    inline void setUserMaterialId(int id) { m_material.userMaterialId = id; }
    /// @brief Custom debug draw color.
    inline uint32_t getCustomDebugColor() const { return m_material.customColor; }
    /// @brief Custom debug draw color.
    inline void setCustomDebugColor(uint32_t color) { m_material.customColor = color; }
    /// @brief The Coulomb (dry) friction coefficient, usually in the range [0,1].
    inline float getFriction() const { return m_material.friction; }
    /// @brief The Coulomb (dry) friction coefficient, usually in the range [0,1].
    inline void setFriction(float friction) { m_material.friction = friction; }
    /// @brief The coefficient of restitution (bounce) usually in the range [0,1].
    inline float getRestitution() const { return m_material.restitution; }
    /// @brief The coefficient of restitution (bounce) usually in the range [0,1].
    inline void setRestitution(float restitution) { m_material.restitution = restitution; }
    /// @brief The rolling resistance usually in the range [0,1].
    inline float getRollingRestitution() const { return m_material.rollingResistance; }
    /// @brief The rolling resistance usually in the range [0,1].
    inline void setRollingRestitution(float rollingResistance) { m_material.rollingResistance = rollingResistance; }
    /// @brief The tangent speed for conveyor belts
    inline float getTangentSpeed() const { return m_material.tangentSpeed; }
    /// @brief The tangent speed for conveyor belts
    inline void setTangentSpeed(float tangentSpeed) { m_material.tangentSpeed = tangentSpeed; }

protected:
    friend FixtureDef;

private:
    b2SurfaceMaterial m_material = b2DefaultSurfaceMaterial();
};

#endif
