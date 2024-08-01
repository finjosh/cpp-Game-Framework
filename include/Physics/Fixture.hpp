#ifndef FIXTURE_HPP
#define FIXTURE_HPP

#pragma once

#include "box2d/types.h"
#define B2_MATH_H
#include "Vector2.hpp"

class Collider;
class ContactData;
class PreSolveData;
class FixtureList;
class HitData;

// TODO update to show all b2Shape functions
/// @note a fixture is just a b2Shape
class Fixture
{
public:
	/// @note if not valid calling any function on this fixture will result in undefined behaviour
	/// @returns true if this fixture is valid
	bool isValid() const;
	/// Is this fixture a sensor (non-solid)?
	/// @return the true if the shape is a sensor.
	bool isSensor() const;
	/// Set the contact filtering data. This will not update contacts until the next time
	/// step when either parent body is active and awake.
	/// This automatically calls Refilter.
	void setFilter(const b2Filter& filter);
	/// Get the contact filtering data.
	b2Filter getFilter() const;
	Collider* getCollider();
	const Collider* getCollider() const;
	/// Test a point for containment in this fixture.
	/// @param p a point in world coordinates.
	bool testPoint(const Vector2& p) const;
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
	void destroy();

protected:
    friend Collider;
	friend ContactData;
	friend PreSolveData;
	friend FixtureList;
	friend HitData;
    Fixture(b2ShapeId shape);
	Fixture(Collider* collider, const b2ShapeDef& shapeDef, const b2Polygon& polygonDef);
    Fixture(Collider& collider, const b2ShapeDef& shapeDef, const b2Polygon& polygonDef);

private:
    b2ShapeId m_shape;
};

class FixtureList
{
public:
	FixtureList(b2BodyId id);

	/// @returns the fixture at the given index (will be invalid if non existant)
	Fixture getFixture(int index) const;
	/// @returns the next fixture in the list
	Fixture getNextFixture();
	Fixture getCurrentFixture() const;
	int getSize() const;
	/// @brief starts at -1
	int getCurrentIndex() const;
	void setCurrentFixture(int index);

protected:

private:
	b2ShapeId* m_array;
	int m_size = 0;
	int m_current = -1;
};

#endif
