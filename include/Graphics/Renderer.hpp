#ifndef RENDERER_HPP
#define RENDERER_HPP

#pragma once

#include "SFML/Graphics/Shape.hpp"

#include "Object.hpp"
#include "Graphics/DrawableObject.hpp"
#include "EngineSettings.hpp"
// #include "Physics/WorldHandler.hpp"

// TODO make renderers for other sfml drawables
// TODO make a base renderer class and completely encapsulate sfml drawable types

/// @brief renderer for any sf::Shape
/// @tparam T the wanted sf::Shape to render
template <typename T, typename std::enable_if_t<std::is_base_of<sf::Shape, T>::value>* = nullptr>
class Renderer : public virtual Object, public DrawableObject, public T
{
public:
    using Ptr = Object::Ptr<Renderer<T>>;

    inline Renderer()
    {
        T::setScale({PIXELS_PER_METER, PIXELS_PER_METER});
    }

    inline Renderer(const T& shape) : T(shape)
    {
        T::setScale({PIXELS_PER_METER, PIXELS_PER_METER});
    }

    inline virtual ~Renderer() = default;

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
        T::setScale({PIXELS_PER_METER * scale.x, PIXELS_PER_METER * scale.y});
    }

    inline void setScale(float x, float y)
    {
        T::setScale({PIXELS_PER_METER * x, PIXELS_PER_METER * y});
    }

    inline Vector2 getScale() const
    {
        return {T::getScale().x/PIXELS_PER_METER, T::getScale().y/PIXELS_PER_METER};
    }

    inline sf::FloatRect getGlobalBounds() const
    {
        sf::FloatRect globalBounds = T::getGlobalBounds();
        globalBounds.size.x /= PIXELS_PER_METER;
        globalBounds.size.y /= PIXELS_PER_METER;
        globalBounds.position.x /= PIXELS_PER_METER;
        globalBounds.position.y /= PIXELS_PER_METER;
        return globalBounds;
    }

protected:
    inline void Draw(sf::RenderTarget* target, const Transform& thisTransform) override
    {
        T::setPosition({thisTransform.position.x*PIXELS_PER_METER, thisTransform.position.y*PIXELS_PER_METER});
        T::setRotation((sf::Angle)thisTransform.rotation);
        target->draw(*this);
    }

private:
    using T::setScale;
    using T::getScale;
    using T::scale;
    using T::getGlobalBounds;
};

/// @brief renderer for any sf::Drawable and sf::Transformable
/// @tparam T the wanted sf::Shape to render
template <typename T, typename std::enable_if_t<std::is_base_of<sf::Drawable, T>::value && std::is_base_of<sf::Transformable, T>::value>* = nullptr>
class DrawRenderer : public virtual Object, public DrawableObject, public T
{
public:
    using Ptr = Object::Ptr<DrawRenderer<T>>;

    inline DrawRenderer()
    {
        T::setScale(PIXELS_PER_METER, PIXELS_PER_METER);
    }

    inline DrawRenderer(const T& shape) : T(shape)
    {
        T::setScale(PIXELS_PER_METER, PIXELS_PER_METER);
    }

    inline virtual ~DrawRenderer() = default;

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
        T::setScale({PIXELS_PER_METER * scale.x, PIXELS_PER_METER * scale.y});
    }

    inline void setScale(float x, float y)
    {
        T::setScale({PIXELS_PER_METER * x, PIXELS_PER_METER * y});
    }

    inline Vector2 getScale() const
    {
        return {T::getScale().x/PIXELS_PER_METER, T::getScale().y/PIXELS_PER_METER};
    }

protected:
    inline void Draw(sf::RenderTarget* target, const Transform& thisTransform) override
    {
        T::setPosition({thisTransform.position.x*PIXELS_PER_METER, thisTransform.position.y*PIXELS_PER_METER});
        T::setRotation((sf::Angle)thisTransform.rotation);
        target->draw(*this);
    }

private:
    using T::setScale;
    using T::getScale;
    using T::scale;
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
//     uint64_t m_lastVertexCount = 0;
//     bool m_updated = false;

// };

#endif
