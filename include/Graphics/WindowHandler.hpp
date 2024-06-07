#ifndef WINDOWHANDLER_H
#define WINDOWHANDLER_H

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
// #include "TGUI/Backend/SFML-Graphics.hpp"
#include "Box2D/b2_math.h"

class WindowHandler
{
public:
    static sf::RenderWindow* getRenderWindow();
    /// @warning does NOT clear memory from last window if already set
    static void setRenderWindow(sf::RenderWindow* renderWindow);

    static void Display();

    /// @note based off of mainCamera
    /// @returns mouse position in global position 
    static b2Vec2 getMousePos();

private:
    inline WindowHandler() = default;
    
    static sf::RenderWindow* m_renderWindow;
};

#endif
