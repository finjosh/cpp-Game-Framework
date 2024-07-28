#ifndef DRAWABLE_MANAGER_HPP
#define DRAWABLE_MANAGER_HPP

#pragma once

#include <set>

#include "SFML/Graphics/RenderWindow.hpp"
#include "DrawableObject.hpp"

class DrawableManager
{
public:
    /// @brief draws all drawable objs
    static void draw(sf::RenderTarget* target, sf::ContextSettings contextSettings);
    static size_t getNumberOfObjects();

    /// @warning dont use this unless you know what you are doing
    static void addDrawable(DrawableObject* DrawableObject);
    /// @warning dont use this unless you know what you are doing
    static void removeDrawable(DrawableObject* DrawableObject);
protected:
    /// @param start the starting iterator
    /// @param end one past the last iterator
    /// @param renderTarget the render target to draw to
    // static void m_draw(std::set<DrawableObject*, _drawableComp>::iterator start, std::set<DrawableObject*, _drawableComp>::iterator end, sf::RenderTarget* renderTarget);

private:
    inline DrawableManager() = default;

    static std::set<DrawableObject*, _drawableComp> m_drawables;
};

#endif
