#ifndef ENGINE_HPP
#define ENGINE_HPP

#pragma once

#include "SFML/Window/Event.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include <list>

/// @brief this is not required to be used and is more as a helper for simple setup
class Engine
{
public:
    static Engine& get();

    /// @brief should be called before any engine functionality is used
    /// @note the params are for the window that will be opend
    void init(sf::VideoMode mode = sf::VideoMode::getDesktopMode(), const sf::String &title = "Some title", uint32_t style = 7U, sf::State state = sf::State::Windowed, const sf::ContextSettings &settings = sf::ContextSettings());
    /// @brief should be called immediately before the while loop
    void preLoop();
    /// @brief call this before handling any events
    /// @note this updates the delta time
    /// @note this is ideal to call at the begining of the while loop
    void preEventHandling();
    /// @brief call or each SFML event and whether the command prompt or canvas manager should handle the event
    /// @note no matter what the input manager will handle the event
    /// @returns whether the event was handled via the command prompt or canvas manager
    bool handleEvent(const sf::Event& event, bool handled = false);
    /// @brief call before your code in the while loop
    void preUserCode();
    void postUserCode();
    void close();
    float getDeltaTime() const;
    /// @param themes in order of most wanted
    /// @param directories directories to check in ("" for current)
    void tryLoadTheme(std::list<std::string> themes, std::list<std::string> directories);

private:
    inline Engine() = default;
    inline Engine(Engine const&) = delete;
    inline void operator=(Engine const&) = delete;
    
    sf::Clock m_deltaClock;
    float m_deltaTime = 0;
    float m_fixedUpdate = 0;
};

#endif
