#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#pragma once

// #include "SFML/System/Vector2.hpp" (SFML_VECTOR2_HPP)
// #include "TGUI/Vector2.hpp" (TGUI_VECTOR2_HPP)
// #include "Box2D/b2_math.h" (B2_MATH_H)

#include "TGUI/String.hpp"
#include "Rotation.hpp"

/// @note if you want helper functions to work with tgui, sfml, and box2d vectors make sure to include them before include "Vector2"
class Vector2
{
public:
    inline Vector2() = default;
    Vector2(float x, float y);
    Vector2(const Vector2& vector);
    /// @brief attempts to convert the given string to a vector2 and initializes this
    Vector2(tgui::String str);

    /// @brief adds given vector to this
    void operator+=(const Vector2& vector);
    /// @note if you call this a lot do think about using the += operator
    Vector2 operator+(const Vector2& vector) const;
    /// @brief subtracts given vector from this
    void operator-=(const Vector2& vector);
    /// @note if you call this a lot do think about using the -= operator
    Vector2 operator-(const Vector2& vector) const;
    /// @returns a vector with this vectors components negated
    Vector2 operator-() const;
    /// @brief multiplies this vector by a scalar
    void operator*=(float scaler);
    /// @note if you call this a lot do think about using the *= operator
    Vector2 operator*(float scaler) const;
    void operator/=(float scaler);
    /// @note if you call this a lot do think about using the /= operator
    Vector2 operator/(float scaler) const;
    void operator=(const Vector2& vector);
    /// @returns true if both x and y are equal
    bool operator==(const Vector2& vector) const;
    /// @returns true if both x and y are equal
    bool operator!=(const Vector2& vector) const;

    //* sfml additions and subtractions
    #ifdef SFML_VECTOR2_HPP
    /// @note you MUST include the sfml vector header BEFORE including this Vector2 header
    /// @note converts from the given typename to a float
    template <typename T>
    inline Vector2(const sf::Vector2<T>& SFMLVector) : x(static_cast<float>(SFMLVector.x)), y(static_cast<float>(SFMLVector.y)) {}     
    template <typename T>
    inline void operator+=(const sf::Vector2<T>& vector)
    {
        x += static_cast<float>(vector.x);
        y += static_cast<float>(vector.y);
    }
    template <typename T>
    inline void operator-=(const sf::Vector2<T>& vector)
    {
        x -= static_cast<float>(vector.x);
        y -= static_cast<float>(vector.y);
    }
    /// @note converts from the given typename to a float
    template <typename T>
    inline void operator=(const sf::Vector2<T>& vector)
    {
        x = static_cast<float>(vector.x);
        y = static_cast<float>(vector.y);
    }
    /// @note converts to the given typename from a float
    /// @returns the equivelent sf Vector2
    template <typename T = float>
    explicit inline operator sf::Vector2<T>() const
    {
        return sf::Vector2<T>{static_cast<T>(x), static_cast<T>(y)};
    }
    #endif

    //* box2d
    #ifdef B2_MATH_H
    /// @note you MUST include the box2d math header BEFORE including this Vector2 header
    inline Vector2(const b2Vec2& box2DVector) : x(box2DVector.x), y(box2DVector.y) {}
    inline void operator+=(const b2Vec2& vector)
    {
        x += vector.x;
        y += vector.y;
    }
    inline void operator-=(const b2Vec2& vector)
    {
        x -= vector.x;
        y -= vector.y;
    }
    inline void operator=(const b2Vec2& vector)
    {
        x = vector.x;
        y = vector.y;
    }
    /// @returns the equivalent b2Vec2
    explicit inline operator b2Vec2() const
    {
        return b2Vec2{x,y};
    }
    #endif

    //* tgui
    #ifdef TGUI_VECTOR2_HPP
    /// @note you MUST include the tgui vector header BEFORE including this Vector2 header
    /// @note converts from the given typename to a float   
    template <typename T>
    inline Vector2(const tgui::Vector2<T>& TGUIVector) : x(static_cast<float>(TGUIVector.x)), y(static_cast<float>(TGUIVector.y)) {}
    template <typename T>
    inline void operator+=(const tgui::Vector2<T>& vector)
    {
        x += static_cast<float>(vector.x);
        y += static_cast<float>(vector.y);
    }
    template <typename T>
    inline void operator-=(const tgui::Vector2<T>& vector)
    {
        x -= static_cast<float>(vector.x);
        y -= static_cast<float>(vector.y);
    }
    /// @note converts from the given typename to a float
    template <typename T>
    inline void operator=(const tgui::Vector2<T>& vector)
    {
        x = static_cast<float>(vector.x);
        y = static_cast<float>(vector.y);
    }
    /// @note converts to the given typename from a float
    /// @returns the equivelent tgui Vector2
    template <typename T = float>
    explicit inline operator tgui::Vector2<T>() const
    {
        return tgui::Vector2<T>{static_cast<T>(x), static_cast<T>(y)};
    }
    #endif

	/// @returns true if this vector contains finite coordinates
	bool isValid() const;
    float length() const;
    float lengthSquared() const;
	/// @brief Convert this vector into a unit vector
    /// @note returns the original length
    float normalize();
    static Vector2 normalize(const Vector2& vector);
    tgui::String toString() const;
    /// @brief rotates the given Vector2 around the given center
    /// @param point the point that will be rotated
    /// @param center the point to rotate around
    /// @param rot rotation in radians
    /// @returns the point rotated
    static Vector2 rotateAround(const Vector2& point, const Vector2& center, Rotation rot);
    /// @returns the dot product of the two vectors
    static float dot(const Vector2& a, const Vector2& b);
    /// @returns the distance between the two vectors
    static float distance(const Vector2& a, const Vector2& b);
    /// @returns the distance between this and the given vector
    float distance(const Vector2& vector) const;
    /// @note angle is +-[0,PI]
    /// @returns the smallest absolute angle from a to b in radians
    static Rotation angle(Vector2 a, Vector2 b);
    /// @note the returned angle is from 0 to PI
    /// @returns the rotation of this vector from the positive x axis
    static Rotation rotation(Vector2 vector);
    /// @note a relativeDistance of 0 will return current, 1 will return target, and 0.5 will return the point midway between current and target
    /// @note relativeDistance can be outside of the range (0-1)
    /// @param relativeDistance the relative distance the returned vector will be (0-1)
    /// @returns a point between the two points
    static Vector2 lerp(const Vector2& current, const Vector2& target, float relativeDistance);
    /// @returns a vector with the max components from a and b
    static Vector2 max(const Vector2& a, const Vector2& b);
    /// @returns a vector with the min components from a and b
    static Vector2 min(const Vector2& a, const Vector2& b);
    /// @param maxDistance that max distance that the returned vector will be from current
    /// @returns a vector between current and target moving no further than maxDistance
    static Vector2 moveTowards(const Vector2& current, const Vector2& target, float maxDistance);
    /// @brief rotates the vector a towards b
    /// @param maxRadians the max change in radians
    /// @param maxMagnitude the max change in magnitude
    /// @returns the new vector
    static Vector2 rotateTowards(const Vector2& current, const Vector2& target, float maxRadians, float maxMagnitude);
    /// @returns a vector where the two vectors are multiplied component wise
    static Vector2 scale(const Vector2& a, const Vector2& b);
    /// @brief multiplies this and the given vector component wise and applies result to this vector
    void scale(const Vector2& vector);
    /// @param rot rotation in radians
    /// @returns the rotated vector
    static Vector2 rotate(const Vector2& vector, Rotation rot);
    /// @brief sets this vector to (0,0)
    void setZero();
    /// @returns a Vector with each component of this vector rounded to the nearest whole value
    Vector2 round() const;

    float x = 0;
    float y = 0;
};

#endif
