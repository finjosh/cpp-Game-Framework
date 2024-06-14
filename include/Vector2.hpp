#ifndef VECTOR2_H
#define VECTOR2_H

#pragma once

// TODO only create conversion functions if the vector type is already included
#include "SFML/System/Vector2.hpp"
#include "TGUI/Vector2.hpp"
#include "Box2D/b2_math.h"

class Vector2
{
public:
    inline Vector2() = default;
    Vector2(float x, float y);
    /// @brief attempts to convert the given string to a vector2 and initializes this
    Vector2(tgui::String str);
    Vector2(const b2Vec2& box2DVector); 
    /// @note converts from the given typename to a float
    template <typename T>
    inline Vector2(const sf::Vector2<T>& SFMLVector) : x(static_cast<float>(SFMLVector.x)), y(static_cast<float>(SFMLVector.y)) {}     
    /// @note converts from the given typename to a float   
    template <typename T>
    inline Vector2(const tgui::Vector2<T>& TGUIVector) : x(static_cast<float>(TGUIVector.x)), y(static_cast<float>(TGUIVector.y)) {}

    /// @brief adds given vector to this
    void operator+=(const Vector2& vector);
    /// @note if you call this a lot do think about using the += operator
    Vector2 operator+(const Vector2& vector) const;
    /// @brief subtracts given vector from this
    void operator-=(const Vector2& vector);
    /// @note if you call this a lot do think about using the -= operator
    Vector2 operator-(const Vector2& vector) const;
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
    template <typename T>
    void operator+=(const sf::Vector2<T>& vector)
    {
        x += static_cast<float>(vector.x);
        y += static_cast<float>(vector.y);
    }
    template <typename T>
    void operator-=(const sf::Vector2<T>& vector)
    {
        x -= static_cast<float>(vector.x);
        y -= static_cast<float>(vector.y);
    }
    /// @note converts from the given typename to a float
    template <typename T>
    void operator=(const sf::Vector2<T>& vector)
    {
        x = static_cast<float>(vector.x);
        y = static_cast<float>(vector.y);
    }
    /// @note converts to the given typename from a float
    /// @returns the equivelent sf Vector2
    template <typename T = float>
    sf::Vector2<T> getSFVector2() const
    {
        return sf::Vector2<T>{static_cast<T>(x), static_cast<T>(y)};
    }

    //* box2d
    void operator+=(const b2Vec2& vector);
    void operator-=(const b2Vec2& vector);
    void operator=(const b2Vec2& vector);
    /// @returns the equivalent b2Vec2
    b2Vec2 getb2Vec2() const;

    //* tgui
    template <typename T>
    void operator+=(const tgui::Vector2<T>& vector)
    {
        x += static_cast<float>(vector.x);
        y += static_cast<float>(vector.y);
    }
    template <typename T>
    void operator-=(const tgui::Vector2<T>& vector)
    {
        x -= static_cast<float>(vector.x);
        y -= static_cast<float>(vector.y);
    }
    /// @note converts from the given typename to a float
    template <typename T>
    void operator=(const tgui::Vector2<T>& vector)
    {
        x = static_cast<float>(vector.x);
        y = static_cast<float>(vector.y);
    }
    /// @note converts to the given typename from a float
    /// @returns the equivelent tgui Vector2
    template <typename T = float>
    tgui::Vector2<T> getTGUIVector2() const
    {
        return tgui::Vector2<T>{static_cast<T>(x), static_cast<T>(y)};
    }

	/// @returns true if this vector contains finite coordinates
	bool IsValid() const;
    float Length() const;
    float LengthSquared() const;
	/// @brief Convert this vector into a unit vector. Returns the length.
    float Normalize();
    tgui::String toString() const;

    float x = 0;
    float y = 0;
};

#endif
