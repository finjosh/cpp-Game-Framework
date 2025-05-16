#include "Physics/FixtureDef.hpp"

FixtureDef::FixtureDef() : m_shapeDef(b2DefaultShapeDef()) {}

float FixtureDef::getDensity() const { return m_shapeDef.density; }
Filter FixtureDef::getFilter() const { return Filter(m_shapeDef.filter); }
bool FixtureDef::isSensor() const { return m_shapeDef.isSensor; }
bool FixtureDef::areSensorEventsEnabled() const { return m_shapeDef.enableSensorEvents; }
bool FixtureDef::areContactEventsEnabled() const { return m_shapeDef.enableContactEvents; }
bool FixtureDef::areHitEventsEnabled() const { return m_shapeDef.enableHitEvents; }
bool FixtureDef::arePreSolveEventsEnabled() const { return m_shapeDef.enablePreSolveEvents; }
bool FixtureDef::isInvokingContactCreation() const { return m_shapeDef.invokeContactCreation; }
SurfaceMaterial FixtureDef::getSurfaceMaterial() const { return SurfaceMaterial(m_shapeDef.material); }
uint32_t FixtureDef::getCustomDebugColor() const { return m_shapeDef.material.customColor; }
float FixtureDef::getFriction() const { return m_shapeDef.material.friction; }
float FixtureDef::getRestitution() const { return m_shapeDef.material.restitution; }
float FixtureDef::getRollingRestitution() const { return m_shapeDef.material.rollingResistance; }
float FixtureDef::getTangentSpeed() const { return m_shapeDef.material.tangentSpeed; }
int FixtureDef::getUserMaterialId() const { return m_shapeDef.material.userMaterialId; }
void FixtureDef::setDensity(float density) { m_shapeDef.density = density; }
void FixtureDef::setFilter(const Filter& filter) { m_shapeDef.filter = filter.m_filter; }
void FixtureDef::setAsSensor(bool isSensor) { m_shapeDef.isSensor = isSensor; }
void FixtureDef::enableSensorEvents(bool enable) { m_shapeDef.enableSensorEvents = enable; }
void FixtureDef::enableContactEvents(bool enable) { m_shapeDef.enableContactEvents = enable; }
void FixtureDef::enablePreSolveEvents(bool enable) { m_shapeDef.enablePreSolveEvents = enable; }
void FixtureDef::setInvokeContactCreation(bool forceContactCreation) { m_shapeDef.invokeContactCreation = forceContactCreation; }
void FixtureDef::setSurfaceMaterial(const SurfaceMaterial& material) { m_shapeDef.material = material.m_material; }
void FixtureDef::setCustomDebugColor(uint32_t color) { m_shapeDef.material.customColor = color; }
void FixtureDef::setFriction(float friction) { m_shapeDef.material.friction = friction; }
void FixtureDef::setRestitution(float restitution) { m_shapeDef.material.restitution = restitution; }
void FixtureDef::setRollingRestitution(float rollingResistance) { m_shapeDef.material.rollingResistance = rollingResistance; }
void FixtureDef::setTangentSpeed(float tangentSpeed) { m_shapeDef.material.tangentSpeed = tangentSpeed; }
void FixtureDef::setUserMaterialId(int id) { m_shapeDef.material.userMaterialId = id; }
