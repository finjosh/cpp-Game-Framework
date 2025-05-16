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
    /// @brief initializes the render window with the given settings
    /// @note you still need to create the render window after this
    static void initRenderWindowSettings(sf::VideoMode mode, const sf::String &title, std::uint32_t style = 7U, sf::State state = sf::State::Windowed, const sf::ContextSettings &settings = sf::ContextSettings());
    /// @brief creates a new render window with the settings that are set
    /// @note default settings are used if none are set
    /// @warning reopens window if it is already open
    static void createRenderWindow();
    /// @brief closes and create a new render window
    /// @note same as createRenderWindow but asserts that the window is open
    static inline void reopenRenderWindow() { assert("Render window must already exist to reopen" && m_renderWindow); createRenderWindow(); }

    static void Display();

    /// @note based off of mainCamera
    /// @note asserts that the render window exists
    /// @returns mouse position in global position (meters)
    static Vector2 getMousePos();
    /// @note asserts that the render window exists
    /// @returns the mouse position in screen position (meters)
    static Vector2 getMouseScreenPos();
    /// @note asserts that the render window exists
    /// @returns the screen size in meters
    static Vector2 getScreenSize();

    /// @note you must reopen the window to apply these settings
    static void setContextSettings(sf::ContextSettings contextSettings);
    static sf::ContextSettings getContextSettings();
    /// @note you must reopen the window to apply these settings
    static void setVideMode(sf::VideoMode mode);
    /// @note you must reopen the window to apply these settings
    static void setState(sf::State state = sf::State::Windowed);
    static sf::VideoMode getVideMode();
    /// @note you must reopen the window to apply these settings
    static void setStyle(std::uint32_t style);
    static std::uint32_t getStyle();
    static sf::State getState();
    static std::string getTitle();
    /// @brief sets the title of the window
    /// @note if the window is open it will update the title right away, no reopen required
    static void setTitle(const std::string& title);
    /// @brief sets the windows max fps
    /// @param value the max fps (0 means no limit)
    /// @note if the window is open it will update the title right away, no reopen required
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
