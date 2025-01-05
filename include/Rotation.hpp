#ifndef ROTATION_HPP
#define ROTATION_HPP

#pragma once

#include "box2d/math_functions.h"
#include "SFML/System/Angle.hpp"

#ifndef PI
#define PI 3.14159265358979323846f
#endif

class Vector2;

/// @note if you want the to string function to be defined include this the string include
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
    float getAngle() const;
    /// @returns the rotation/angle in degrees
    float getAngleInDegrees() const;
    /// @brief same as "getAngleInDegrees()"
    float getDegrees() const { return getAngleInDegrees(); }
    /// @brief same as "getAngle()"
    float getRadians() const { return getAngle(); }
    /// @brief sets the stored rotation to 0
    void setZero();
	/// @returns the rotated x axis (1,0) in terms of this rotation
	Vector2 GetXAxis() const;
    /// @returns the rotated y axis (0,1) in terms of this rotation
	Vector2 GetYAxis() const;
    /// @note this is clamped so the relative rotation is guarantied to be in the range [0,1]
    /// @param current the current rotation
    /// @param wanted the wanted rotation
    /// @param relativeRotation the relative rotation to travel
    /// @returns the rotation from current to target
    static Rotation lerp(const Rotation& current, const Rotation& target, float relativeRotation);
    /// @note this is not clamped so the relative rotation can be outside of the range [0,1]
    /// @param current the current rotation
    /// @param wanted the wanted rotation
    /// @param relativeRotation the relative rotation to travel
    /// @returns the rotation from current to target
    static Rotation lerpUnclamped(const Rotation& current, const Rotation& target, float relativeRotation);

    //* box2d functions
    inline Rotation(b2Rot rot) : cos(rot.c), sin(rot.s) {}
    explicit inline operator b2Rot() const
    {
        b2Rot temp;
        temp.c = cos;
        temp.s = sin;
        return temp;
    }
    // ----------------

    //* SFML Funcitons
    inline Rotation(sf::Angle angle) 
    { this->set(angle.asRadians()); }
    explicit inline operator sf::Angle() const
    { return sf::radians(this->getAngle()); }
    // ---------------

    //* string conversion function
    #ifdef _BASIC_STRING_H
    /// @note this calculates the angle and converts to a string
    inline std::basic_string<char> toString()
    {
        return std::to_string(this->getAngle());
    }
    /// @brief returns this rotation in degrees as a string
    /// @note this calculates the angle and converts to a string
    inline std::basic_string<char> toStringDegrees()
    {
        return std::to_string(this->getAngle() * 180 / PI);
    }
    #endif
    // ----------------

    Rotation operator + (Rotation rotation) const;
    void operator += (Rotation rotation);
    Rotation operator * (float scaler) const;
    void operator *= (float scaler);
    Rotation operator - (Rotation rotation) const;
    void operator -= (Rotation rotation);
    Rotation operator - () const;
    void operator = (Rotation rotation);

    float sin = 0.f, cos = 1.f;
protected:

private:

};

#endif
