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
    static void initRenderWindow(sf::VideoMode mode, const sf::String &title, std::uint32_t style = 7U, sf::State state = sf::State::Windowed, const sf::ContextSettings &settings = sf::ContextSettings());

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
    static void setState(sf::State state = sf::State::Windowed);
    static sf::VideoMode getVideMode();
    /// @note this recreates the render window with the given style
    static void setStyle(std::uint32_t style);
    static std::uint32_t getStyle();
    static sf::State getState();
    static std::string getTitle();
    static void setTitle(const std::string& title);
    /// @brief sets the windows max fps
    /// @note this has to be done here or else it will not update properly when context settings are set
    /// @param value the max fps (0 means no limit)
    static void setFPSLimit(unsigned int value);
    static unsigned int getFPSLimit();

    static EventHelper::EventDynamic<sf::RenderWindow*> onRenderWindowChanged;

private:
    inline WindowHandler() = default;
    
    static sf::RenderWindow* m_renderWindow;
    static sf::VideoMode m_videoMode;
    static sf::ContextSettings m_contextSettings;
    static std::uint32_t m_style;
    static sf::State m_state;
    static std::string m_title;
    static Vector2 m_lastMousePos;
    static unsigned int m_FPSLimit;
};

#endif
