#ifndef RENDERER_HPP
#define RENDERER_HPP

#pragma once

#include "SFML/Graphics/Shape.hpp"

#include "Object.hpp"
#include "Graphics/DrawableObject.hpp"
#include "Settings.hpp"
// #include "Physics/WorldHandler.hpp"

// TODO make renderers for other sfml drawables

/// @brief renderer for any sf::Shape
/// @warning if derived from do NOT forget to use "createDestroy()"
/// @note this can be used as a stand alone object
/// @tparam T the wanted sf::Shape to render
template <typename T, typename std::enable_if_t<std::is_base_of<sf::Shape, T>::value>* = nullptr>
class Renderer : public virtual Object, public DrawableObject, public T
{
public:
    using Ptr = Object::Ptr<Renderer<T>>;

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

    inline void setScale(Vector2 scale)
    {
        T::setScale(PIXELS_PER_METER * scale.x, PIXELS_PER_METER * scale.y);
    }

    inline void setScale(float x, float y)
    {
        T::setScale(PIXELS_PER_METER * x, PIXELS_PER_METER * y);
    }

    inline Vector2 getScale() const
    {
        Vector2 temp = T::getScale();
        temp /= PIXELS_PER_METER;
        return temp;
    }

protected:
    inline void Draw(sf::RenderTarget* target, const Transform& thisTransform) override
    {
        // T::setPosition((sf::Vector2f)Object::getGlobalPosition()*PIXELS_PER_METER);
        // T::setRotation(Object::getGlobalRotation().getAngle()*180/PI);
        T::setPosition({thisTransform.position.x*PIXELS_PER_METER, thisTransform.position.y*PIXELS_PER_METER});
        T::setRotation(thisTransform.rotation.getAngle()*180/PI);
        target->draw(*this);
    }

private:
    using T::setScale;
    using T::getScale;
    using T::scale;

    createDestroy();
};

// TODO finish this 
// template <typename T, typename std::enable_if_t<std::is_same<sf::VertexArray, T>::value || std::is_same<sf::VertexBuffer, T>::value>* = nullptr>
// class Renderer : public virtual Object, public DrawableObject, public T
// {
// public:
//     using Ptr = Object::Ptr<Renderer<T>>;

//     inline Renderer() = default;

//     inline void setScale(Vector2 scale) {  }

// protected:
//     inline void Draw(sf::RenderTarget* target, const Transform& thisTransform) override
//     {
//         // T::setPosition((sf::Vector2f)Object::getGlobalPosition()*PIXELS_PER_METER);
//         // T::setRotation(Object::getGlobalRotation().getAngle()*180/PI);
//         T::setPosition((sf::Vector2f)thisTransform.position*PIXELS_PER_METER);
//         T::setRotation(thisTransform.rotation.getAngle()*180/PI);
//         target->draw(*this);
//     }

// private:
//     Vector2 m_scale;
//     Vector2 m_origin;
//     Vector2 m_bounds;
//     Transform m_lastTransform;
//     size_t m_lastVertexCount = 0;
//     bool m_updated = false;

//     createDestroy();
// };

#endif
