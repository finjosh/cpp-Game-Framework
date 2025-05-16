#ifndef FIXTURE_HPP
#define FIXTURE_HPP

#pragma once

#include "box2d/types.h"
#include "Vector2.hpp"
#include "FixtureDef.hpp"

class Collider;
class FixtureArray;
class ContactData;
class HitData;
class PreSolveData;

// TODO implement joints wrapper
/// @note unless you are sure this still exists you should check if this is valid before use
/// @note all events are disabled by default
/// @note this is just a reference to a fixture on a body not the fixture itself
class Fixture
{
public:
	struct Shape
	{
		enum class Type
		{
			Circle = b2ShapeType::b2_circleShape,
			Capsule = b2ShapeType::b2_capsuleShape,
			Polygon = b2ShapeType::b2_polygonShape,
			Segment = b2ShapeType::b2_segmentShape,
			ChainSegment = b2ShapeType::b2_chainSegmentShape,
			Count = b2ShapeType::b2_shapeTypeCount,
		};

		/// @brief A solid circle
		class Circle
		{
		public:
			Circle();
			Circle(Vector2 center, float radius);

			Vector2 getCenter() const;
			float getRadius() const;

			void setCenter(Vector2 center);
			void setRadius(float radius);

		private:
			friend Fixture;
			friend Collider;
			Circle(b2Circle circle);
			
			b2Circle m_shape;
		};

		/// @brief A solid capsule can be viewed as two semicircles connected by a rectangle.
		class Capsule
		{
		public:
			Capsule();
			Capsule(Vector2 center1, Vector2 center2, float radius);

			Vector2 getCenter1() const;
			Vector2 getCenter2() const;
			float getRadius() const;

			void setCenter1(Vector2 center1);
			void setCenter2(Vector2 center2);
			void setRadius(float radius);

		protected:
			friend Fixture;
			friend Collider;
			Capsule(b2Capsule capsule);

			b2Capsule m_shape;
		};

		/// @brief A solid convex polygon. 
		/// @note It is assumed that the interior of the polygon is to the left of each edge.
		/// @warning Polygons have a maximum number of vertices equal to Polygon::MAX_VERTICES.
		class Polygon
		{
		public:
			#define MAX_POLYGON_VERTICES B2_MAX_POLYGON_VERTICES

			/// @note a hull MUST have at least 3 points
			/// @note a hull can have at most Polygon::MAX_VERTICES
			class Hull
			{
			public:
				/// @brief tries to create a convex hull from the given list of points 
				/// @warning This is an expensive operation
				/// @note if it fails to create a hull this will be invalid
				/// @note This welds close points and removes collinear points
				/// @note Some failure cases:
				/// @note - all points very close together
				/// @note - all points on a line
				/// @note - less than 3 points
				/// @note - more than Polygon::MAX_VERTICES points
				Hull(b2Vec2 points[], std::int32_t size);
				bool isValid() const;

			private:
				friend Polygon;
				Hull(b2Hull hull);

				b2Hull m_hull = {{} /*empty list*/, 0/*size zero list*/};
			};

			/// @warning this will be a invalid polygon and must be setup before usage
			Polygon();
			/// @brief makes this polygon a box of size x (width) by y (height)
			/// @ingroup Box
			Polygon(float x, float y);
			/// @brief makes this polygon a box of size x (width) by y (height).
			/// @note offset and angle are applied locally
			/// @ingroup Box
			Polygon(float x, float y, Vector2 center, Rotation rotation);
			/// @brief fills this polygon with points from the given hull
			/// @param radius how the polygon will be rounded (0 means no rounding). Radius is added around the entire shape and allows for a curve at edges
			/// @exception asserts that the hull is valid
			/// @ingroup Polygon
			Polygon(const Hull& hull, float radius = 0.f);
			/// @brief fills this polygon with points from the given hull
			/// @note offset and angle are applied locally
			/// @exception asserts that the hull is valid
			/// @ingroup Polygon
			Polygon(const Hull& hull, Vector2 center, Rotation rotation);
			/// @brief makes this polygon a rounded box of size x (width) by y (height)
			/// @param radius how the polygon will be rounded (0 means no rounding). Radius is added around the entire shape and allows for a curve at edges
			/// @ingroup RoundedBox
			Polygon(float x, float y, float radius);
			/// @brief makes this polygon a square with the given side length
			/// @param radius how the polygon will be rounded (0 means no rounding). Radius is added around the entire shape and allows for a curve at edges
			/// @ingroup Square
			Polygon(float sideLengths);
			
			bool isValid() const;

			/// @brief makes this polygon a box of size x (width) by y (height)
			/// @ingroup Box
			void makeBox(float x, float y);
			/// @brief makes this polygon a box of size x (width) by y (height).
			/// @note offset and angle are applied locally
			/// @ingroup Box
			void makeOffsetBox(float x, float y, Vector2 center, Rotation rotation);
			/// @brief fills this polygon with points from the given hull
			/// @param radius how the polygon will be rounded (0 means no rounding). Radius is added around the entire shape and allows for a curve at edges
			/// @exception asserts that the hull is valid
			/// @ingroup Polygon
			void makePolygon(const Hull& hull, float radius = 0.f);
			/// @brief fills this polygon with points from the given hull
			/// @note offset and angle are applied locally
			/// @exception asserts that the hull is valid
			/// @ingroup Polygon
			void makeOffsetPolygon(const Hull& hull, Vector2 center, Rotation rotation);
			/// @brief fills this polygon with points from the given hull
			/// @note offset and angle are applied locally
			/// @param radius how the polygon will be rounded (0 means no rounding). Radius is added around the entire shape and allows for a curve at edges
			/// @exception asserts that the hull is valid
			/// @ingroup Polygon
			void makeOffsetRoundedPolygon(const Hull& hull, float radius, Vector2 center, Rotation rotation);
			/// @brief makes this polygon a rounded box of size x (width) by y (height)
			/// @param radius how the polygon will be rounded (0 means no rounding). Radius is added around the entire shape and allows for a curve at edges
			/// @ingroup RoundedBox
			void makeRoundedBox(float x, float y, float radius);
			/// @brief makes this polygon a rounded box of size x (width) by y (height)
			/// @note offset and angle are applied locally
			/// @param radius how the polygon will be rounded (0 means no rounding). Radius is added around the entire shape and allows for a curve at edges
			/// @ingroup RoundedBox
			void makeOffsetRoundedBox(float x, float y, float radius, Vector2 center, Rotation rotation);
			/// @brief makes this polygon a square with the given side length
			/// @param radius how the polygon will be rounded (0 means no rounding). Radius is added around the entire shape and allows for a curve at edges
			/// @ingroup Square
			void makeSquare(float sideLengths);
			/// @brief makes this polygon a square with the given side length
			/// @note offset and angle are applied locally
			/// @param radius how the polygon will be rounded (0 means no rounding). Radius is added around the entire shape and allows for a curve at edges
			/// @ingroup Square
			void makeOffsetSquare(float sideLengths, Vector2 center, Rotation rotation);

		private:
			friend Fixture;
			friend Collider;
			Polygon(b2Polygon polygon);

			b2Polygon m_shape;
		};

		/// @brief A line segment with two-sided collision.
		class Segment
		{
		public:
			Segment();
			Segment(Vector2 point1, Vector2 point2);
			
			void setPoint1(Vector2 point1);
			void setPoint2(Vector2 point2);
			
			Vector2 getPoint1() const;
			Vector2 getPoint2() const;
			
		protected:
			friend Fixture;
			friend Collider;
			Segment(b2Segment segment);
			
			b2Segment m_shape;
		};

		// /// @brief this is a definition for a chain to be used in a fixture
		// class ChainDef
		// {
		// public:
		// 	inline ChainDef() = default;

		// 	/// @brief Set the points for the chain
		// 	/// @param points An array of at least 4 points
		// 	/// @param count The number of points, must be 4 or more
		// 	void setPoints(const b2Vec2* points, int32_t count);
			
		// 	/// @brief Set the friction coefficient, usually in the range [0,1]
		// 	void setFriction(float friction);
			
		// 	/// @brief Set the restitution (elasticity), usually in the range [0,1]
		// 	void setRestitution(float restitution);
			
		// 	/// @brief Set the collision filter data
		// 	void setFilter(b2Filter filter);
			
		// 	/// @brief Set whether this is a closed chain formed by connecting first and last points
		// 	/// @param isLoop True to make a closed loop, false for open chain
		// 	void setAsLoop(bool isLoop);

		// 	/// @returns the points for the chain

		// 	/// An array of at least 4 points. These are cloned and may be temporary.
		// 	const b2Vec2* points;

		// 	/// The point count, must be 4 or more.
		// 	int32_t count;

		// 	/// The friction coefficient, usually in the range [0,1].
		// 	float friction;

		// 	/// The restitution (elasticity) usually in the range [0,1].
		// 	float restitution;

		// 	/// Contact filtering data.
		// 	b2Filter filter;

		// 	/// Indicates a closed chain formed by connecting the first and last points
		// 	bool isLoop;

		// 	/// Used internally to detect a valid definition. DO NOT SET.
		// 	int32_t internalValue;

		// private:
		// 	friend Fixture;

		// 	b2ChainDef m_chain = b2DefaultChainDef();
		// };

		// /// @brief this is a pointer to the chain that is stored in a fixture
		// class Chain
		// {

		// };
	};

	Collider* getCollider();
	const Collider* getCollider() const;

	/// @returns true if this fixture is valid
	bool isValid() const;
	/// @returns the fixtures shape type
	Fixture::Shape::Type getType() const;
	/// @note same as getType() 
	inline Fixture::Shape::Type getShapeType() const { return getType(); }
	/// @note a fixture cannot start or stop being a sensor (only enable/disable event calls)
	/// @returns true if this is a sensor
	bool isSensor();
	/// @brief set the mass density, typically in kg/m^2
	/// @note if updateBodyProperties is not set to true the bodies mass properties will not update
	/// @param updateBodyProperties If true, the mass properties of the parent body will be updated automatically (possibly overriding manually set mass data)
	void setDensity(float density, bool updateBodyProperties = false);
	/// @returns mass density, typically in kg/m^2
	float getDensity() const;
	/// @brief set the friction, typically in the range [0,1]
	void setFriction(float friction);
	/// @returns the friction value, typically in the range [0,1]
	float getFriction() const;
	/// @brief set restitution (bounciness), typically in the range [0,1]
	void setRestitution(float restitution);
	/// @returns restitution, typically in the range [0,1]
	float getRestitution() const;
	/// @returns collision filter
	Filter getFilter() const;
	/// @brief set the current collision filter
	/// @note this is almost as expensive as recreating the fixture
	void setFilter(Filter filter);
	/// @brief Enable sensor events for this fixture
	/// @note Only applies to kinematic and dynamic bodies
	/// @note Ignored for sensors
	void enableSensorEvents(bool enabled = true); // TODO remove this if these are able to be handled without the box2d events
	/// @returns true if sensor events are enabled
	bool areSensorEventsEnabled();
	/// @brief Enable contact events for this fixture 
	/// @note Only applies to kinematic and dynamic bodies
	/// @note Ignored for sensors
	void enableContactEvents(bool enabled = true);
	/// @returns true if contact events are enabled
	bool areContactEventsEnabled();
	/// @brief Enable pre-solve contact events for this fixture
	/// @note Only applies to dynamic bodies
	/// @note Ignored for sensors.
	/// @note default is false
	/// @warning These are expensive and must be carefully handled due to multithreading
	void enablePreSolveEvents(bool enabled = true);
	/// @returns true if pre-solve events are enabled
	bool arePreSolveEventsEnabled();
	/// @brief Test a point for overlap with a fixture
	/// @param point The point to test
	/// @returns true if the point overlaps with the fixture
	bool testPoint(Vector2 point);
	/// @brief Ray cast this fixture directly
	/// @param origin The origin of the ray
	/// @param translation The translation of the ray
	/// @param maxFraction The maximum fraction of the translation to consider, typically 1
	/// @returns The result of the ray cast
	b2CastOutput rayCast(Vector2 origin, Vector2 translation, float maxFraction = 1.f); // TODO implement a wrapper for b2CastOutput
	/// @returns stored circle data
	/// @warning Asserts the type is correct
	/// @note if you update data in this struct it will not be updated in this fixture you must then use the required set function
	Fixture::Shape::Circle getCircle() const;
	/// @returns stored line segment data 
	/// @warning Asserts the type is correct
	/// @note if you update data in this struct it will not be updated in this fixture you must then use the required set function
	Fixture::Shape::Segment getSegment() const;
	/// @returns stored capsule data
	/// @warning Asserts the type is correct
	/// @note if you update data in this struct it will not be updated in this fixture you must then use the required set function
	Fixture::Shape::Capsule getCapsule() const;
	/// @returns stored convex polygon data 
	/// @warning Asserts the type is correct
	/// @note if you update data in this struct it will not be updated in this fixture you must then use the required set function
	Fixture::Shape::Polygon getPolygon() const;
	/// @brief Allows you to change this fixtures shape to be a circle or update the current circle
	/// @param updateBodyProperties If true, the mass properties of the parent body will be updated automatically (possibly overriding manually set mass data)
	void setCircle(const Fixture::Shape::Circle& circle, bool updateBodyProperties = false);
	/// @brief Allows you to change this fixtures shape to be a capsule or update the current capsule
	/// @param updateBodyProperties If true, the mass properties of the parent body will be updated automatically (possibly overriding manually set mass data)
	void setCapsule(const Fixture::Shape::Capsule& capsule, bool updateBodyProperties = false);
	/// @brief Allows you to change this fixtures shape to be a segment or update the current segment
	/// @param updateBodyProperties If true, the mass properties of the parent body will be updated automatically (possibly overriding manually set mass data)
	void setSegment(const Fixture::Shape::Segment& segment, bool updateBodyProperties = false);
	/// @brief Allows you to change this fixtures shape to be a polygon or update the current polygon
	/// @param updateBodyProperties If true, the mass properties of the parent body will be updated automatically (possibly overriding manually set mass data)
	void setPolygon(const Fixture::Shape::Polygon& polygon, bool updateBodyProperties = false);
	/// @returns this fixtures mass data if able to be calculated
	b2MassData calculateMassData() const;
	/// @returns the mass value from b2MassData collected from "Fixture::calculateMassData"
	float calculateMass() const;

	// /// @brief Get the parent chain id if the shape type is b2_smoothSegmentShape, otherwise
	// /// returns b2_nullChainId
	// /// @returns The parent chain id
	// b2ChainId getParentChain();
	// /// @brief Get the maximum capacity required for retrieving all the touching contacts on a shape
	// /// @returns The contact capacity
	// int getContactCapacity();
	// /// @brief Get the touching contact data for a shape. The provided shapeId will be either shapeIdA or shapeIdB on the contact data
	// /// @param contactData Pointer to the contact data array
	// /// @param capacity The capacity of the contact data array
	// /// @returns The number of contacts
	// int getContactData(b2ContactData* contactData, int capacity);

	/// @brief Get the current world AABB
	b2AABB getAABB() const;
	/// @brief get the closest point on this fixture to a target point
	/// @note Target and result are in world space
	/// @returns The closest point on the shape
	Vector2 getClosestPoint(Vector2 target) const;
	/// @brief destroys this fixture and removes it from the parent body
	/// @param updateMassData You may defer the body mass update which can improve performance if several shapes on a body are destroyed at once.
	void destroy(bool updateMassData = true);

protected:
	friend Collider;
	friend FixtureArray;
	friend ContactData;
	friend HitData;
	friend PreSolveData;
    Fixture(b2ShapeId fixture);
    Fixture(Collider& collider, const FixtureDef& fixtureDef, const Fixture::Shape::Circle& shape);
	Fixture(Collider& collider, const FixtureDef& fixtureDef, const Fixture::Shape::Capsule& shape);
	Fixture(Collider& collider, const FixtureDef& fixtureDef, const Fixture::Shape::Segment& shape);
	/// @warning asserts that the polygon is valid
	Fixture(Collider& collider, const FixtureDef& fixtureDef, const Fixture::Shape::Polygon& shape);

private:
    b2ShapeId m_fixture = b2_nullShapeId;
};

#endif
