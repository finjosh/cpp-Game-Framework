#ifndef WINDOW_HANDLER_HPP
#define WINDOW_HANDLER_HPP

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
// #include "TGUI/Backend/SFML-Graphics.hpp"
// #include "Box2D/b2_math.h"
#include "TGUI/Vector2.hpp"
#include "Vector2.hpp"

class WindowHandler
{
public:
    static sf::RenderWindow* getRenderWindow();
    /// @warning does NOT clear memory from last window if already set
    static void setRenderWindow(sf::RenderWindow* renderWindow);

    static void Display();

    /// @note based off of mainCamera
    /// @returns mouse position in global position (meters)
    static Vector2 getMousePos();
    /// @returns the mouse position in screen position (meters)
    static Vector2 getMouseScreenPos();
    /// @returns the screen size in meters
    static Vector2 getScreenSize();

private:
    inline WindowHandler() = default;
    
    static sf::RenderWindow* m_renderWindow;
    static Vector2 m_lastMousePos;
};

#endif
