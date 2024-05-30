#ifndef FIXTURE_H
#define FIXTURE_H

#pragma once

#include "Box2D/b2_fixture.h"

class Collider;
class ContactData;
// TODO implement a enum used for easy comparison of fixtures
/// @note this does not update when the collider is destroyed (only store in the colliders object or keep track if it is still alive)
class Fixture
{
public:
	/// Set if this fixture is a sensor.
	void SetSensor(const bool& sensor);
	/// Is this fixture a sensor (non-solid)?
	/// @return the true if the shape is a sensor.
	bool IsSensor() const;
	/// Set the contact filtering data. This will not update contacts until the next time
	/// step when either parent body is active and awake.
	/// This automatically calls Refilter.
	void SetFilterData(const b2Filter& filter);
	/// Get the contact filtering data.
	const b2Filter& GetFilterData() const;
	/// @brief updates the current fixture to the next fixture for this fixtures collider
    /// @returns false if there are no more fixtures in this fixtures collider (this fixture stays the same)
	bool GetNext();
	Collider* getCollider();
	/// Test a point for containment in this fixture.
	/// @param p a point in world coordinates.
	bool TestPoint(const b2Vec2& p) const;
	/// Get the mass data for this fixture. The mass data is based on the density and
	/// the shape. The rotational inertia is about the shape's origin. This operation
	/// may be expensive.
	void GetMassData(b2MassData* massData) const;
	/// Set the density of this fixture. This will _not_ automatically adjust the mass
	/// of the body. You must call b2Body::ResetMassData to update the body's mass.
	void SetDensity(const float& density);
	/// Get the density of this fixture.
	float GetDensity() const;
	/// Get the coefficient of friction.
	float GetFriction() const;
	/// Set the coefficient of friction. This will _not_ change the friction of
	/// existing contacts.
	void SetFriction(const float& friction);
	/// Get the coefficient of restitution.
	float GetRestitution() const;
	/// Set the coefficient of restitution. This will _not_ change the restitution of
	/// existing contacts.
	void SetRestitution(const float& restitution);
	/// Get the restitution velocity threshold.
	float GetRestitutionThreshold() const;
	/// Set the restitution threshold. This will _not_ change the restitution threshold of
	/// existing contacts.
	void SetRestitutionThreshold(const float& threshold);

protected:
    friend Collider;
	friend ContactData;
    Fixture(b2Fixture* fixture);
	Fixture(Collider* collider, const b2FixtureDef& fixtureDef);
    Fixture(Collider& collider, const b2FixtureDef& fixtureDef);

private:
    b2Fixture* m_fixture;
};

#endif
