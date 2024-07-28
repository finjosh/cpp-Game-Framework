#ifndef WINDOW_HANDLER_HPP
#define WINDOW_HANDLER_HPP

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
// #include "TGUI/Backend/SFML-Graphics.hpp"
// #include "Box2D/b2_math.h"
#include "TGUI/Vector2.hpp"
#include "Vector2.hpp"
#include "Utils/EventHelper.hpp"

class WindowHandler
{
public:
    static sf::RenderWindow* getRenderWindow();
    /// @note if called after window is already made will replace the window
    static void initRenderWindow(sf::VideoMode mode, const sf::String &title, sf::Uint32 style = 7U, const sf::ContextSettings &settings = sf::ContextSettings());

    static void Display();

    /// @note based off of mainCamera
    /// @returns mouse position in global position (meters)
    static Vector2 getMousePos();
    /// @returns the mouse position in screen position (meters)
    static Vector2 getMouseScreenPos();
    /// @returns the screen size in meters
    static Vector2 getScreenSize();

    /// @note this recreates the render window with the given contextSettings
    static void setContextSettings(sf::ContextSettings contextSettings);
    static sf::ContextSettings getContextSettings();
    /// @note this recreates the render window with the given video mode
    static void setVideMode(sf::VideoMode mode);
    static sf::VideoMode getVideMode();
    /// @note this recreates the render window with the given style
    static void setStyle(sf::Uint32 style);
    static sf::Uint32 getStyle();
    static std::string getTitle();
    static void setTitle(const std::string& title);

    static EventHelper::EventDynamic<sf::RenderWindow*> onRenderWindowChanged;

private:
    inline WindowHandler() = default;
    
    static sf::RenderWindow* m_renderWindow;
    static sf::VideoMode m_videoMode;
    static sf::ContextSettings m_contextSettings;
    static sf::Uint32 m_style;
    static std::string m_title;
    static Vector2 m_lastMousePos;
};

#endif
