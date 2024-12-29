#ifndef INPUT_HPP
#define INPUT_HPP

#pragma once

#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"
#include "SFML/Window/Joystick.hpp"
#include "SFML/Window/Event.hpp"

#include <unordered_map>
#include <list>
#include <vector>
#include <set>

// TODO implement mouse scroll wheel and joystick
class Input
{
public:
    /// @note compares by name
    class Action
    {
    public:
        class Event
        {
        public:
            Event(const std::list<sf::Keyboard::Key>& keyCodes = {}, const std::list<sf::Mouse::Button>& mouseButtons = {});
            // Event(const std::list<sf::Keyboard::Key>& keyCodes, const std::list<sf::Mouse::Button>& mouseButtons);
            void setEvent(const std::list<sf::Keyboard::Key>& keyCodes = {}, const std::list<sf::Mouse::Button>& mouseButtons = {});
            void setEvent_sc(const std::list<sf::Keyboard::Scancode>& scanCodes = {}, const std::list<sf::Mouse::Button>& mouseButtons = {});
            /// @brief add the given input to this event
            void addInput(sf::Keyboard::Key keyCode);
            /// @brief add the given input to this event
            void addInput(sf::Keyboard::Scancode scanCode);
            /// @brief add the given input to this event
            void addInput(sf::Mouse::Button button);
            /// @note the key codes are stored as scanCodes and you should localize them to get the sf::Keyboard::Key
            const std::list<sf::Keyboard::Scancode>& getKeyCodes() const;
            const std::list<sf::Mouse::Button>& getMouseButtons() const;

            bool isPressed() const;
            bool isJustPressed() const;
            bool isReleased() const;
            bool isJustReleased() const;
            /// @returns true if both lists are NOT empty
            bool isValid() const;

            /// @param stored if the string is sorted by ascii values
            std::string toString(bool stored = true) const;

        protected:
            std::list<sf::Keyboard::Scancode> m_keys;
            std::list<sf::Mouse::Button> m_mouseButtons;
        };

        Action(const std::string& name, const Action::Event& event = {});

        std::string getName() const;
        /// @brief adds another way for this action to be pressed/released
        /// @returns the new events index
        size_t addEvent(const Action::Event& event = {});
        /// @brief if the index exists removes the event
        void removeEvent(size_t index);
        /// @note if the index does not exist nothing happens
        void setEvent(size_t index, const Action::Event& event = {});
        /// @note if index is none existant then a nullptr will be returned
        /// @param index the index of the wanted event
        /// @returns the event at the given index
        const Action::Event* getEvent(size_t index) const;
        /// @returns the number of events attached to this action
        size_t getEventCount() const;

        /// @returns true if the action is held down or just pressed
        bool isPressed() const;
        /// @note this will happen as long as one of the keys/buttons on an event is just pressed and the others are pressed
        bool isJustPressed() const;
        /// @note this happens only if all events are released (if there is a group of keys at least one has to be released for the event to be released)
        /// @returns true if the action is held up or just released
        bool isReleased() const;
        /// @note this will happen if ALL events are released and at least one is just released
        bool isJustReleased() const;

        bool operator <(const Action& action) const;
        bool operator <=(const Action& action) const;
        bool operator >(const Action& action) const;
        bool operator >=(const Action& action) const;
        bool operator ==(const Action& action) const;
        bool operator !=(const Action& action) const;
        bool operator <(const std::string& actionName) const;
        bool operator <=(const std::string& actionName) const;
        bool operator >(const std::string& actionName) const;
        bool operator >=(const std::string& actionName) const;
        bool operator ==(const std::string& actionName) const;
        bool operator !=(const std::string& actionName) const;

        std::string toString() const;

    protected:
        std::string m_name;
        std::vector<Action::Event> m_events;
    };

    Input();
    static Input& get();

    /// @param wasHandled if the event was handled by something else (this stops states from changing unless it is a release event)
    /// @note if window focus is lost then all inputs that are pressed or just pressed will be set to just released
    void HandelEvent(sf::Event event, bool wasHandled = false);
    /// @note call this before handling any sfml input but NOT in the event loop
    void UpdateJustStates();

    enum State
    {
        JustPressed, 
        JustReleased,
        Pressed, // held down
        Released // held up
    };

    struct FrameData
    {
        enum Type
        {
            Mouse,
            Keyboard
        };

        /// @param state should only be just states (just pressed, just released)
        inline FrameData(Type type, State state, sf::Keyboard::Scancode scanCode)
        {
            this->type = type;
            this->state = state;
            this->scanCode = scanCode;
        }

        /// @param state should only be just states (just pressed, just released)
        inline FrameData(Type type, State state, sf::Mouse::Button mouseButton)
        {
            this->type = type;
            this->state = state;
            this->mouseButton = mouseButton;
        }

        FrameData::Type type;
        State state;

        union 
        {
            sf::Keyboard::Scancode scanCode;
            sf::Mouse::Button mouseButton;
        };
    };

    /// @note this uses keyCode to search for the key (first converted to scan code and then searches)
    State getKeyState(sf::Keyboard::Key keyCode) const;
    /// @returns if the given key is down or just pressed
    bool isPressed(sf::Keyboard::Key keycode) const;
    /// @returns if the given key is up or just released
    bool isReleased(sf::Keyboard::Key keycode) const;
    bool isJustPressed(sf::Keyboard::Key keycode) const;
    bool isJustReleased(sf::Keyboard::Key keycode) const;
    /// @note this is more optimal as there are more possible keys so you can have more input settings
    State getKeyState(sf::Keyboard::Scancode scanCode) const;
    /// @returns if the given key/scanCode is down or just pressed
    bool isPressed(sf::Keyboard::Scancode scanCode) const;
    /// @returns if the given key/scanCode is up or just released
    bool isReleased(sf::Keyboard::Scancode scanCode) const;
    bool isJustPressed(sf::Keyboard::Scancode scanCode) const;
    bool isJustReleased(sf::Keyboard::Scancode scanCode) const;
    State getMouseState(sf::Mouse::Button button) const;
    /// @returns if the given mouse button is down or just pressed
    bool isPressed(sf::Mouse::Button button) const;
    /// @returns if the given mouse button is up or just released
    bool isReleased(sf::Mouse::Button button) const;
    bool isJustPressed(sf::Mouse::Button button) const;
    bool isJustReleased(sf::Mouse::Button button) const;
    /// @returns the list of input changes from this frame
    const std::list<FrameData>& getFrameData() const;
    /// @param states a list of states that the Input type must have
    /// @returns an Action::Event that stores all input types (keys, mouse, ect.) with one of the given states 
    Input::Action::Event getAllOfState(const std::list<Input::State>& states) const;

    /// @note throws an assert error if action does not exist (returns false otherwise)
    bool isActionPressed(const std::string& name) const;
    /// @note throws an assert error if action does not exist (returns false otherwise)
    bool isActionReleased(const std::string& name) const;
    /// @note throws an assert error if action does not exist (returns false otherwise)
    bool isActionJustPressed(const std::string& name) const;
    /// @note throws an assert error if action does not exist (returns false otherwise)
    bool isActionJustReleased(const std::string& name) const;

    /// @returns nullptr if no action with the given name exists
    const Action* getAction(const std::string& name) const;
    /// @brief creates or replaces the action with the given actions name
    void setAction(const Action& action);

    static std::string toString(sf::Mouse::Button mouseButton);
    static std::string toString(sf::Keyboard::Key key);
    static std::string toString(sf::Keyboard::Scancode scanCode);
    /// @brief attempts to convert a string into a Action::Event
    /// @returns an Action::Event that stores any inputs that where in the string
    static Input::Action::Event fromString_Action_Event(const std::string& str);
    /// @param actionName the name that the returned action will have
    static Input::Action fromString_Action(const std::string& actionName, const std::string& str);

protected:
    std::set<Action> m_actions;
private:
    std::unordered_map<sf::Mouse::Button, State> m_mouse;
    std::unordered_map<sf::Keyboard::Scancode, State> m_keyboard; // storing keys with Scancode for more possible keys that can be used as input
    std::list<FrameData> m_lastFrame;

    /// @brief used for converting from a string to a button
    static std::unordered_map<std::string, sf::Mouse::Button> m_mouseDict;
    /// @brief used for converting from a string to a scancode
    static std::unordered_map<std::string, sf::Keyboard::Scancode> m_keyboardDict;
};

#endif
