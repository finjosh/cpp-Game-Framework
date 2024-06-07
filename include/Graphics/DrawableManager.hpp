#ifndef DRAWABLEMANAGER_H
#define DRAWABLEMANAGER_H

#pragma once

#include <set>

#include "SFML/Graphics/RenderWindow.hpp"

#include "DrawableObject.hpp"

class DrawableManager
{
public:
    /// @brief draws all drawable objs
    static void draw(sf::RenderWindow& window);
    static size_t getNumberOfObjects();

    /// @warning dont use this unless you know what you are doing
    static void addDrawable(DrawableObject* DrawableObject);
    /// @warning dont use this unless you know what you are doing
    static void removeDrawable(DrawableObject* DrawableObject);
protected:

private:
    inline DrawableManager() = default;
    
    static std::set<DrawableObject*, _drawableComp> m_drawables;
};

#endif
