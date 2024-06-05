#ifndef VECTOR_CONVERSIONS_H
#define VECTOR_CONVERSIONS_H

#pragma once

#include "Box2D/b2_math.h"
#include "SFML/System/Vector2.hpp"

template<typename T = float>
b2Vec2 convertVec2(sf::Vector2<T> vec)
{
    return {static_cast<float>(vec.x), static_cast<float>(vec.y)};
}

inline b2Vec2 convertVec2f(sf::Vector2f vec)
{
    return {vec.x, vec.y};
}

template<typename T = float>
sf::Vector2<T> convertVec2(b2Vec2 vec)
{
    return {static_cast<T>(vec.x), static_cast<T>(vec.y)};
}

#endif
