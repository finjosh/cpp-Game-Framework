#ifndef RECT_H
#define RECT_H

#pragma once

#include "SFML/Graphics/Shape.hpp"

#include "Object.hpp"
#include "Settings.hpp"
#include "Graphics/DrawableObject.hpp"
// #include "Physics/WorldHandler.hpp"

/// @brief renderer for any sf::Shape
/// @tparam T the wanted sf::Shape to render
template <typename T, typename std::enable_if_t<std::is_base_of<sf::Shape, T>::value>* = nullptr>
class Renderer : public virtual Object, public DrawableObject, public T
{
public:
    inline Renderer()
    {
        T::setScale(PIXELS_PER_METER, PIXELS_PER_METER);
    }

    using Object::setPosition;
    using Object::getPosition;
    using Object::setRotation;
    using Object::getRotation;
    using Object::setTransform;
    using Object::getTransform;
    using Object::move;
    using Object::rotate;

protected:
    inline void Draw(sf::RenderWindow& window) override
    {
        T::setPosition(Object::getPosition().x*PIXELS_PER_METER, Object::getPosition().y*PIXELS_PER_METER);
        T::setRotation(Object::getRotation()*180/PI);
        window.draw(*this);
    }

private:
    using T::setScale;
    using T::getScale;
};

#endif
