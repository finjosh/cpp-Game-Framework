#ifndef VECTOR_CONVERSIONS_H
#define VECTOR_CONVERSIONS_H

#pragma once

#include "Box2D/b2_math.h"
#include "SFML/System/Vector2.hpp"
#include "TGUI/Vector2.hpp"

// TODO make a custom vector class that can easily convert between sfml, box2d, and tguis

template<typename T = float>
inline b2Vec2 convertVec2(sf::Vector2<T> vec)
{
    return {static_cast<float>(vec.x), static_cast<float>(vec.y)};
}

inline b2Vec2 convertVec2f(sf::Vector2f vec)
{
    return {vec.x, vec.y};
}

template<typename T = float>
inline sf::Vector2<T> convertVec2(b2Vec2 vec)
{
    return {static_cast<T>(vec.x), static_cast<T>(vec.y)};
}

// template <typename T1, typename T2>
// tgui::Vector2<T1> convertVec2(sf::Vector2<T2> vec)
// {
//     return {static_cast<T1>(vec.x), static_cast<T1>(vec.y)};
// }

// template <typename T1, typename T2>
// sf::Vector2<T1> convertVec2(tgui::Vector2<T2> vec)
// {
//     return {static_cast<T1>(vec.x), static_cast<T1>(vec.y)};
// }

// template <typename T>
// b2Vec2 convertVec2(tgui::Vector2<T> vec)
// {
//     return {static_cast<float>(vec.x), static_cast<float>(vec.y)};
// }

#endif
