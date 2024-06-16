#ifndef TRANSFORM_H
#define TRANSFORM_H

#pragma once

// #include "Box2D/b2_math.h" (B2_MATH_H)

#include "Vector2.hpp"
#include "Rotation.hpp"

/// @note if you want helper functions to work with tgui, sfml, and box2d vectors make sure to include them before include "Vector2"
class Transform
{
public:
    /// @brief position = (0,0) rotation = 0
    inline Transform() = default;
    Transform(const Vector2& position, Rotation rotation);
	void set(const Vector2& position, Rotation rotation);
    /// @brief sets position to (0,0) and rotation to 0
    void setZero();
    Vector2 getLocalPoint(Vector2 point) const;
    Vector2 getGlobalPoint(Vector2 point) const;
    Vector2 getLocalVector(Vector2 vector) const;
    Vector2 getGlobalVector(Vector2 vector) const;

    //* box2d functions
    #ifdef B2_MATH_H
    inline Transform(b2Transform transform) : position(transform.p), rotation(transform.q) {}
    explicit inline operator b2Transform() const
    {
        return b2Transform{(b2Vec2)position, (b2Rot)rotation};
    }
    #endif
    // ----------------

    Vector2 position = Vector2(0,0);
    Rotation rotation = Rotation(0);
protected:

private:

};

#endif
