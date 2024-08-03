#ifndef INPUT_HPP
#define INPUT_HPP

#pragma once

#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"
#include "SFML/Window/Joystick.hpp"
#include "SFML/Window/Event.hpp"

#include <unordered_map>
#include <list>

// TODO implement mouse scroll wheel and joystick
class Input
{
public:
    Input();
    static Input& get();

    /// @param wasHandled if the event was handled by something else (this stops states from changing unless it is a release event)
    void HandelEvent(sf::Event event, bool wasHandled = true);

    enum State
    {
        Pressed, // just down
        Released, // just up
        Down, // held down
        Up // held up
    };

    /// @note this uses keyCode to search for the key (first converted to scan code and then searches)
    State getKeyState(sf::Keyboard::Key keyCode);
    /// @returns if the given key is down or just down (pressed)
    bool isDown(sf::Keyboard::Key keycode);
    /// @returns if the given key is up or just up (released)
    bool isUp(sf::Keyboard::Key keycode);
    /// @note this is more optimal as there are more possible keys so you can have more input settings
    State getKeyState(sf::Keyboard::Scancode scanCode);
    /// @returns if the given key/scanCode is down or just down (pressed)
    bool isDown(sf::Keyboard::Scancode scanCode);
    /// @returns if the given key/scanCode is up or just up (released)
    bool isUp(sf::Keyboard::Scancode scanCode);
    State getMouseState(sf::Mouse::Button button);
    /// @returns if the given mouse button is down or just down (pressed)
    bool isDown(sf::Mouse::Button button);
    /// @returns if the given mouse button is up or just up (released)
    bool isUp(sf::Mouse::Button button);

    static std::string toString(sf::Mouse::Button mouseButton);
    static std::string toString(sf::Keyboard::Key key);
    static std::string toString(sf::Keyboard::Scancode scanCode);

protected:
    struct FrameData
    {
        enum Type
        {
            MouseUp,
            MouseDown,
            KeyboardUp,
            KeyboardDown
        };

        inline FrameData(Type type, sf::Keyboard::Scancode scanCode)
        {
            this->type = type;
            this->scanCode = scanCode;
        }

        inline FrameData(Type type, sf::Mouse::Button mouseButton)
        {
            this->type = type;
            this->mouseButton = mouseButton;
        }

        FrameData::Type type;

        union 
        {
            sf::Keyboard::Scancode scanCode;
            sf::Mouse::Button mouseButton;
        };
    };

    std::unordered_map<sf::Mouse::Button, State> m_mouse;
    std::unordered_map<sf::Keyboard::Scancode, State> m_keyboard; // storing keys with Scancode for more possible keys that can be used as input
    std::list<FrameData> m_lastFrame;
private:

};

#endif
