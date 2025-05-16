#ifndef COLLISION_FILTER_HPP
#define COLLISION_FILTER_HPP

#pragma once

#include "box2d/types.h"

class Filter
{
public:
    /// @brief default filter
    Filter();

	uint64_t getCategoryBits() const;
    /// @brief The collision category bits. Normally you would just set one bit. The category bits should represent your application object types.
    ///
	/// @code{.cpp}
	/// enum MyCategories
	/// {
	///    Static  = 0x00000001,
	///    Dynamic = 0x00000002,
	///    Debris  = 0x00000004,
	///    Player  = 0x00000008,
	///    // etc
	/// };
	/// @endcode
    void setCategoryBits(uint64_t categoryBits);

	uint64_t getMaskBits() const;
	/// @brief The collision mask bits. This states the categories that the shape this is applied to would accept for collision.
	/// @note For example, you may want your player to only collide with static objects and other players.
    ///
	/// @code{.c}
	/// maskBits = Static | Player;
	/// @endcode
    void setMaskBits(uint64_t maskBits);

	int getGroupIndex() const;
    /// @brief Collision groups allow a certain group of objects to never collide (negative) or always collide (positive). 
    ///
    /// @note A group index of zero has no effect. 
    ///
    /// @note Non-zero group filtering always wins against the mask bits.
    ///
	/// @note For example, you may want ragdolls to collide with other ragdolls but you don't want ragdoll self-collision. 
    ///
    /// @note In this case you would give each ragdoll a unique negative group index and apply that group index to all shapes on the ragdoll.	
    void setGroupIndex(int groupIndex);

private:
	Filter(b2Filter filter);
	friend class Fixture;
	friend class FixtureDef;

    b2Filter m_filter;
};

#endif
