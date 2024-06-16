#ifndef FIXTURE_H
#define FIXTURE_H

#pragma once

#include "Box2D/b2_fixture.h"
#include "Vector2.hpp"

class Collider;
class ContactData;
class PreSolveData;

/// @note this does not update when the collider is destroyed (only store in the colliders object or keep track if it is still alive)
class Fixture
{
public:
	/// Set if this fixture is a sensor.
	void setSensor(bool sensor);
	/// Is this fixture a sensor (non-solid)?
	/// @return the true if the shape is a sensor.
	bool isSensor() const;
	/// Set the contact filtering data. This will not update contacts until the next time
	/// step when either parent body is active and awake.
	/// This automatically calls Refilter.
	void setFilterData(const b2Filter& filter);
	/// Get the contact filtering data.
	const b2Filter& getFilterData() const;
	/// @brief updates the current fixture to the next fixture for this fixtures collider
    /// @returns false if there are no more fixtures in this fixtures collider (this fixture stays the same)
	bool getNext();
	Collider* getCollider();
	const Collider* getCollider() const;
	/// Test a point for containment in this fixture.
	/// @param p a point in world coordinates.
	bool testPoint(const Vector2& p) const;
	/// Get the mass data for this fixture. The mass data is based on the density and
	/// the shape. The rotational inertia is about the shape's origin. This operation
	/// may be expensive.
	void getMassData(b2MassData* massData) const;
	/// Set the density of this fixture. This will _not_ automatically adjust the mass
	/// of the body. You must call b2Body::ResetMassData to update the body's mass.
	void setDensity(float density);
	/// Get the density of this fixture.
	float getDensity() const;
	/// Get the coefficient of friction.
	float getFriction() const;
	/// Set the coefficient of friction. This will _not_ change the friction of
	/// existing contacts.
	void setFriction(float friction);
	/// Get the coefficient of restitution.
	float getRestitution() const;
	/// Set the coefficient of restitution. This will _not_ change the restitution of
	/// existing contacts.
	void setRestitution(float restitution);
	/// Get the restitution velocity threshold.
	float getRestitutionThreshold() const;
	/// Set the restitution threshold. This will _not_ change the restitution threshold of
	/// existing contacts.
	void setRestitutionThreshold(float threshold);

protected:
    friend Collider;
	friend ContactData;
	friend PreSolveData;
    Fixture(b2Fixture* fixture);
	Fixture(Collider* collider, const b2FixtureDef& fixtureDef);
    Fixture(Collider& collider, const b2FixtureDef& fixtureDef);

private:
    b2Fixture* m_fixture;
};

#endif
