#ifndef ROTATION_H
#define ROTATION_H

#pragma once

// #include "Box2D/b2_math.h" (B2_MATH_H)

#define PI 3.14159265359f

class Vector2;

/// @note if you want helper functions to work with tgui, sfml, and box2d vectors make sure to include them before include "Vector2"
class Rotation
{
public:
    inline Rotation() = default;
    Rotation(float radians);
    Rotation(const Rotation& rotation);
    Rotation(float cos, float sin);

    /// @brief sets the stored sin and cos values with and angle in radians
    void set(float radians);
    /// @returns the rotation/angle in radians
    float getRadians() const;
    /// @returns the rotation/angle in radians
    float getAngle() const;
    /// @brief sets the stored rotation to 0
    void setZero();
	/// @returns the rotated x axis (1,0) in terms of this rotation
	Vector2 GetXAxis() const;
    /// @returns the rotated y axis (0,1) in terms of this rotation
	Vector2 GetYAxis() const;

    //* box2d functions
    #ifdef B2_MATH_H
    inline Rotation(b2Rot rot) : cos(rot.c), sin(rot.s) {}
    explicit inline operator b2Rot() const
    {
        b2Rot temp;
        temp.c = cos;
        temp.s = sin;
        return temp;
    }
    #endif
    // ----------------

    // TODO test floating point errors by using these functions
    // TODO test performance on these with trig identities vs recalculating using trig functions
    Rotation operator + (Rotation rotation) const;
    void operator += (Rotation rotation);
    Rotation operator - (Rotation rotation) const;
    void operator -= (Rotation rotation);
    void operator = (Rotation rotation);

    float sin = 0.f, cos = 1.f;
protected:

private:

};

#endif
