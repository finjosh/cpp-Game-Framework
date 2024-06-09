#ifndef RECT_H
#define RECT_H

#pragma once

#include "SFML/Graphics/Shape.hpp"

#include "Object.hpp"
#include "Settings.hpp"
#include "Graphics/DrawableObject.hpp"
// #include "Physics/WorldHandler.hpp"

// TODO manually override all the sfml drawing classes (maybe)

/// @brief renderer for any sf::Shape
/// @warning if derived from do NOT forget to use "createDestroy()"
/// @note this can be used as a stand alone object
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

    inline void setScale(float x, float y)
    {
        T::setScale(PIXELS_PER_METER + x, PIXELS_PER_METER + y);
    }

    inline b2Vec2 getScale() const
    {
        sf::Vector2f temp = T::getScale();
        return {temp.x - PIXELS_PER_METER, temp.y - PIXELS_PER_METER};
    }

protected:
    inline void Draw(sf::RenderTarget* target) override
    {
        T::setPosition(Object::getPosition().x*PIXELS_PER_METER, Object::getPosition().y*PIXELS_PER_METER);
        T::setRotation(Object::getRotation()*180/PI);
        target->draw(*this);
    }

private:
    float m_xScale = 1, m_yScale = 1;

    using T::setScale;
    using T::getScale;
    using T::scale;

    createDestroy();
};

#endif
