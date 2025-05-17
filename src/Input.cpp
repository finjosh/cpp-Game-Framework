#include "Input.hpp"
#include <cassert>
#include <sstream>
#include <algorithm>
#include "Utils/StringHelper.hpp"

//* Action::Event defintions

Input::Action::Event::Event(const std::list<sf::Keyboard::Key>& keyCodes, const std::list<sf::Mouse::Button>& mouseButtons)
{
    for (auto key: keyCodes)
    {
        m_keys.emplace_back(sf::Keyboard::delocalize(key));
    }
    m_mouseButtons = mouseButtons;
}

void Input::Action::Event::setEvent(const std::list<sf::Keyboard::Key>& keyCodes, const std::list<sf::Mouse::Button>& mouseButtons)
{
    m_keys.clear();
    for (auto key: keyCodes)
    {
        m_keys.emplace_back(sf::Keyboard::delocalize(key));
    }
    m_mouseButtons = mouseButtons;
}

void Input::Action::Event::setEvent_sc(const std::list<sf::Keyboard::Scancode>& scanCodes, const std::list<sf::Mouse::Button>& mouseButtons)
{
    m_keys = scanCodes;
    m_mouseButtons = mouseButtons;
}

void Input::Action::Event::addInput(sf::Keyboard::Key keyCode)
{
    m_keys.emplace_back(sf::Keyboard::delocalize(keyCode));
}

void Input::Action::Event::addInput(sf::Keyboard::Scancode scanCode)
{
    m_keys.emplace_back(scanCode);
}

void Input::Action::Event::addInput(sf::Mouse::Button button)
{
    m_mouseButtons.emplace_back(button);
}

const std::list<sf::Keyboard::Scancode>& Input::Action::Event::getKeyCodes() const
{
    return m_keys;
}

const std::list<sf::Mouse::Button>& Input::Action::Event::getMouseButtons() const
{
    return m_mouseButtons;
}

bool Input::Action::Event::isPressed() const
{
    bool pressed = true;

    for (auto button: m_mouseButtons)
    {
        if (Input::get().isPressed(button))
            continue;
        else
        {
            pressed = false;
            continue;
        }
    }

    if (!pressed) // so we dont have to check keys
        return false;

    for (auto key: m_keys)
    {
        if (Input::get().isPressed(key))
            continue;
        else
        {
            pressed = false;
            break;
        }
    }

    return pressed;
}

bool Input::Action::Event::isJustPressed() const
{
    bool justPressed = false;

    for (auto button: m_mouseButtons)
    {
        if (Input::get().isJustPressed(button))
            justPressed = true;
        else if (Input::get().isPressed(button))
            continue;
        else
            return false; // since one button is not pressed this can not be just pressed
    }

    for (auto key: m_keys)
    {
        if (Input::get().isJustPressed(key))
            justPressed = true;
        else if (Input::get().isPressed(key))
            continue;
        else
            return false;
    }   

    return justPressed;
}

bool Input::Action::Event::isReleased() const
{
    return !this->isPressed();
}

bool Input::Action::Event::isJustReleased() const
{
    size_t justReleased = 0;
    size_t pressed = 0;

    for (auto key: m_keys)
    {
        if (Input::get().isPressed(key))
            pressed++;
        else if (Input::get().isJustReleased(key))
            justReleased++;
    }
    
    for (auto button: m_mouseButtons)
    {
        if (Input::get().isPressed(button))
            pressed++;
        else if (Input::get().isJustReleased(button))
            justReleased++;
    }

    if (justReleased > 0 && pressed == m_mouseButtons.size() + m_keys.size() - justReleased)
        return true;
    
    return false;
}

bool Input::Action::Event::isValid() const
{
    return m_keys.size() != 0 || m_mouseButtons.size() != 0;
}

std::string Input::Action::Event::toString(bool sorted) const
{
    std::list<std::string> strs;
    std::string rtn;
    
    for (auto button: m_mouseButtons)
        strs.emplace_back(Input::toString(button));

    if (sorted)
        strs.sort();
    for (auto str: strs)
        rtn += str + " + ";
    strs.clear();

    for (auto key: m_keys)
        strs.emplace_back(Input::toString(key));

    if (strs.size() == 0 && rtn.size() != 0)
        rtn.erase(rtn.size() - 3); // removing the extra plus and space if there are not key inputs
    else
    {
        if (sorted)
        {
            // we want to sort by length then by ascii value so ctrl, alt, ect. are first
            strs.sort([](const std::string& lhs, const std::string& rhs){
                return lhs.size() >= rhs.size() && lhs < rhs;
            });
        }
        for (auto str: strs)
            rtn += str + " + ";
        if (rtn.size() != 0)
            rtn.erase(rtn.size() - 3);
    }

    return rtn;
}

//* Action defintions

Input::Action::Action(const std::string& name, const Input::Action::Event& event) : m_name(name)
{
    if (!event.isValid()) // we dont have any events
        return;
    else
        m_events.emplace_back(event);
}

std::string Input::Action::getName() const
{
    return m_name;
}

size_t Input::Action::addEvent(const Input::Action::Event& event)
{
    m_events.emplace_back(event);

    return m_events.size() - 1; // returning the index of the new event
}

void Input::Action::removeEvent(size_t index)
{
    if (index < m_events.size())
        m_events.erase(m_events.begin() + index);
}

void Input::Action::setEvent(size_t index, const Input::Action::Event& event)
{
    if (index < m_events.size())
        m_events[index] = event;
}

const Input::Action::Event* Input::Action::getEvent(size_t index) const
{
    if (index < m_events.size())
        return &m_events[index];
    else
        return nullptr;
}

size_t Input::Action::getEventCount() const
{
    return m_events.size();
}

bool Input::Action::isPressed() const
{
    for (const Action::Event& event: m_events)
    {
        if (event.isPressed())
            return true;
    }

    return false;
}

bool Input::Action::isJustPressed() const
{
    bool justPressed = false; // at least one input is just pressed

    for (const Action::Event& event: m_events)
    {
        bool temp = event.isJustPressed();
        if (!temp && event.isPressed())
            return false; // because an event is already pressed we dont care that another was just pressed
        justPressed = justPressed || temp;
    }

    return justPressed;
}

bool Input::Action::isReleased() const
{
    return !this->isPressed(); // no need for extra code when we already have this
}

bool Input::Action::isJustReleased() const
{
    bool justReleasedEvent = false; // if at least one event is just released
    for (const Action::Event& event: m_events)
    {
        bool justReleased = event.isJustReleased();
        if (!justReleased && event.isPressed())
            return false; // because at least one event is pressed we dont care that any are just released
        justReleasedEvent = justReleasedEvent || justReleased;
    }

    return justReleasedEvent;
}

bool Input::Action::operator <(const Action& action) const
{
    return m_name < action.m_name;
}

bool Input::Action::operator <=(const Action& action) const
{
    return m_name <= action.m_name;
}

bool Input::Action::operator >(const Action& action) const
{
    return m_name > action.m_name;
}

bool Input::Action::operator >=(const Action& action) const
{
    return m_name >= action.m_name;
}

bool Input::Action::operator ==(const Action& action) const
{
    return m_name == action.m_name;
}

bool Input::Action::operator !=(const Action& action) const
{
    return m_name != action.m_name;
}

bool Input::Action::operator <(const std::string& actionName) const
{
    return m_name < actionName;
}

bool Input::Action::operator <=(const std::string& actionName) const
{
    return m_name <= actionName;
}

bool Input::Action::operator >(const std::string& actionName) const
{
    return m_name > actionName;
}

bool Input::Action::operator >=(const std::string& actionName) const
{
    return m_name >= actionName;
}

bool Input::Action::operator ==(const std::string& actionName) const
{
    return m_name == actionName;
}

bool Input::Action::operator !=(const std::string& actionName) const
{
    return m_name != actionName;
}

std::string Input::Action::toString() const
{
    std::string rtn;
    for (const Action::Event& event: m_events)
        rtn += event.toString() + " / ";
    
    rtn.erase(rtn.size() - 3);
    return rtn;
}

//* input definitions

std::unordered_map<std::string, sf::Mouse::Button> Input::m_mouseDict = {{"Left Mouse", sf::Mouse::Button::Left},
                                                                         {"Right Mouse", sf::Mouse::Button::Right},
                                                                         {"Middle Mouse", sf::Mouse::Button::Middle},
                                                                         {"Mouse Extra 1", sf::Mouse::Button::Extra1},
                                                                         {"Mouse Extra 2", sf::Mouse::Button::Extra2}};
std::unordered_map<std::string, sf::Keyboard::Scancode> Input::m_keyboardDict;

Input::Input()
{
    Input::m_keyboardDict.clear();
    for (int i = -1; i < (int)sf::Keyboard::ScancodeCount; i++)
    {
        m_keyboard[sf::Keyboard::Scancode(i)] = State::Released;
        Input::m_keyboardDict.emplace(Input::toString(sf::Keyboard::Scancode(i)), sf::Keyboard::Scancode(i));
    }
    
    for (int i = 0; i < (int)sf::Mouse::ButtonCount; i++)
        m_mouse[sf::Mouse::Button(i)] = State::Released;
}

Input& Input::get()
{
    static Input singleton;
    return singleton;
}

void Input::HandelEvent(sf::Event event, bool wasHandled)
{
    if (wasHandled) // only handling release events if event was already handled
    {
        // updating states if needed
        if (const sf::Event::KeyReleased* key = event.getIf<sf::Event::KeyReleased>())
        {
            if (this->isPressed(key->scancode))
            {
                m_keyboard[key->scancode] = State::JustReleased;
                m_lastFrame.emplace_back(FrameData::Type::Keyboard, State::JustReleased, key->scancode);
            }
        }
        else if (const sf::Event::MouseButtonReleased* mouseButton = event.getIf<sf::Event::MouseButtonReleased>()) 
        {
            if (this->isPressed(mouseButton->button))
            {
                m_mouse[mouseButton->button] = State::JustReleased;
                m_lastFrame.emplace_back(FrameData::Type::Mouse, State::JustReleased, mouseButton->button);
            }
        }

        return; // we dont want to handle any other events but release since the event was already handled
    }

    // if we lose focus then we should set every input to being released
    if (event.is<sf::Event::FocusLost>())
    {
        for (auto key: m_keyboard)
        {
            if (this->isPressed(key.first))
            {
                m_keyboard[key.first] = State::JustReleased;
                m_lastFrame.emplace_back(FrameData::Type::Keyboard, State::JustReleased, key.first);
            }
        }
    
        for (auto button: m_mouse)
        {
            if (this->isPressed(button.first))
            {
                m_mouse[button.first] = State::JustReleased;
                m_lastFrame.emplace_back(FrameData::Type::Mouse, State::JustReleased, button.first);
            }
        }

        return;
    }

    if (const sf::Event::KeyPressed* key = event.getIf<sf::Event::KeyPressed>())
    {
        m_keyboard[key->scancode] = State::JustPressed;
        m_lastFrame.emplace_back(FrameData::Type::Keyboard, State::JustPressed, key->scancode);
    }
    else if (const sf::Event::KeyReleased* key = event.getIf<sf::Event::KeyReleased>())
    {
        m_keyboard[key->scancode] = State::JustReleased;
        m_lastFrame.emplace_back(FrameData::Type::Keyboard, State::JustReleased, key->scancode);
    }
    else if (const sf::Event::MouseButtonPressed* mouseButton = event.getIf<sf::Event::MouseButtonPressed>())
    {
        m_mouse[mouseButton->button] = State::JustPressed;
        m_lastFrame.emplace_back(FrameData::Type::Mouse, State::JustPressed, mouseButton->button);
    }
    else if (const sf::Event::MouseButtonReleased* mouseButton = event.getIf<sf::Event::MouseButtonReleased>())
    {
        m_mouse[mouseButton->button] = State::JustReleased;
        m_lastFrame.emplace_back(FrameData::Type::Mouse, State::JustReleased, mouseButton->button);
    }
}

void Input::UpdateJustStates()
{
    for (const FrameData& data: m_lastFrame) // updating just down/up to down/up
    {
        State newState = data.state == State::JustPressed ? State::Pressed : State::Released; // since it should only be just states that are added
        if (data.type == FrameData::Type::Mouse)
            m_mouse[data.mouseButton] = newState;
        else if (data.type == FrameData::Type::Keyboard)
            m_keyboard[data.scanCode] = newState;
    }
    m_lastFrame.clear();
}

Input::State Input::getKeyState(sf::Keyboard::Key keyCode) const
{
    return m_keyboard.find(sf::Keyboard::delocalize(keyCode))->second; // we know all keyboard keys exist in this list
}

bool Input::isPressed(sf::Keyboard::Key keycode) const
{
    auto state = m_keyboard.find(sf::Keyboard::delocalize(keycode))->second;
    return state == State::Pressed || state == State::JustPressed;
}

bool Input::isReleased(sf::Keyboard::Key keycode) const
{
    auto state = m_keyboard.find(sf::Keyboard::delocalize(keycode))->second;
    return state == State::Released || state == State::JustReleased;
}

bool Input::isJustPressed(sf::Keyboard::Key keycode) const
{
    return m_keyboard.find(sf::Keyboard::delocalize(keycode))->second == State::JustPressed;
}

bool Input::isJustReleased(sf::Keyboard::Key keycode) const
{
    return m_keyboard.find(sf::Keyboard::delocalize(keycode))->second == State::JustReleased;
}

Input::State Input::getKeyState(sf::Keyboard::Scancode scanCode) const
{
    return m_keyboard.find(scanCode)->second;
}

bool Input::isPressed(sf::Keyboard::Scancode scanCode) const
{
    State state = m_keyboard.find(scanCode)->second;
    return state == State::Pressed || state == State::JustPressed;
}

bool Input::isReleased(sf::Keyboard::Scancode scanCode) const
{
    State state = m_keyboard.find(scanCode)->second;
    return state == State::Released || state == State::JustReleased;
}

bool Input::isJustPressed(sf::Keyboard::Scancode scanCode) const
{
    return m_keyboard.find(scanCode)->second == State::JustPressed;
}

bool Input::isJustReleased(sf::Keyboard::Scancode scanCode) const
{
    return m_keyboard.find(scanCode)->second == State::JustReleased;
}

Input::State Input::getMouseState(sf::Mouse::Button button) const
{
    return m_mouse.find(button)->second; // we know that all buttons exist in the map
}

bool Input::isPressed(sf::Mouse::Button button) const
{
    State state = m_mouse.find(button)->second;
    return state == State::Pressed || state == State::JustPressed;
}

bool Input::isReleased(sf::Mouse::Button button) const
{
    State state = m_mouse.find(button)->second;
    return state == State::Released || state == State::JustReleased;
}

bool Input::isJustPressed(sf::Mouse::Button button) const
{
    return m_mouse.find(button)->second == State::JustPressed;
}

bool Input::isJustReleased(sf::Mouse::Button button) const
{
    return m_mouse.find(button)->second == State::JustReleased;
}

const std::list<Input::FrameData>& Input::getFrameData() const
{
    return m_lastFrame;
}

Input::Action::Event Input::getAllOfState(const std::list<Input::State>& states) const
{
    Input::Action::Event rtn;

    for (auto key: m_keyboard)
    {
        if (std::find(states.begin(), states.end(), key.second) != states.end())
            rtn.addInput(sf::Keyboard::Scancode(key.first));
    }

    for (auto button: m_mouse)
    {
        if (std::find(states.begin(), states.end(), button.second) != states.end())
            rtn.addInput(button.first);
    }

    return rtn;
}

std::string Input::toString(sf::Mouse::Button mouseButton)
{
    std::string rtn;
    switch (mouseButton)
    {
    case sf::Mouse::Button::Left:
        rtn = "Left Mouse";
        break;
    
    case sf::Mouse::Button::Right:
        rtn = "Right Mouse";
        break;

    case sf::Mouse::Button::Middle:
        rtn = "Middle Mouse";
        break;

    case sf::Mouse::Button::Extra1:
        rtn = "Mouse Extra 1";
        break;

    case sf::Mouse::Button::Extra2:
        rtn = "Mouse Extra 2";
        break;

    default:
        break;
    }
    return rtn;
}

std::string Input::toString(sf::Keyboard::Key key)
{
    return Input::toString(sf::Keyboard::delocalize(key));
}

std::string Input::toString(sf::Keyboard::Scancode scanCode)
{
    std::string rtn = sf::Keyboard::getDescription(scanCode);
    if (rtn == "Unknown")
        rtn += " " + sf::Keyboard::getDescription(scanCode).toAnsiString();
    return rtn;
}

bool Input::isActionPressed(const std::string& name) const
{
    auto iter = m_actions.find(name);
    
    if (iter == m_actions.end())
    {
        assert("Action is not set as an input" && false);
        return false;
    }
    
    return iter->isPressed();
}

bool Input::isActionReleased(const std::string& name) const
{
    auto iter = m_actions.find(name);
    
    if (iter == m_actions.end())
    {
        assert("Action is not set as an input" && false);
        return false;
    }
    
    return iter->isReleased();
}

bool Input::isActionJustPressed(const std::string& name) const
{
    auto iter = m_actions.find(name);
    
    if (iter == m_actions.end())
    {
        assert("Action is not set as an input" && false);
        return false;
    }
    
    return iter->isJustPressed();
}

bool Input::isActionJustReleased(const std::string& name) const
{
    auto iter = m_actions.find(name);
    
    if (iter == m_actions.end())
    {
        assert("Action is not set as an input" && false);
        return false;
    }
    
    return iter->isJustReleased();
}

const Input::Action* Input::getAction(const std::string& name) const
{
    auto iter = m_actions.find(name);
    if (iter == m_actions.end())
        return nullptr;
    return &(*iter);
}

void Input::setAction(const Input::Action& action)
{
    auto iter = m_actions.find(action);
    if (iter != m_actions.end())
        m_actions.erase(action);
    m_actions.emplace(action);
}

Input::Action::Event Input::fromString_Action_Event(const std::string& str)
{
    assert(m_keyboardDict.size() != 0 && "Input must have been initalized before using this function (use Input::get())");

    std::stringstream sstr(str);
    std::string token;
    Input::Action::Event event;

    while (std::getline(sstr, token, '+'))
    {
        StringHelper::trim(token);

        auto keyIter = m_keyboardDict.find(token);
        if (keyIter != m_keyboardDict.end())
        {
            event.addInput(keyIter->second);
        }
        else
        {
            auto mouseIter = m_mouseDict.find(token);
            if (mouseIter != m_mouseDict.end())
            {
                event.addInput(mouseIter->second);
            }
        }
    }

    return event;
}

Input::Action Input::fromString_Action(const std::string& actionName, const std::string& str)
{
    assert("Input must have been initalized before using this function (use Input::get())" && m_keyboardDict.size() != 0);

    std::stringstream sstr(str);
    std::string token;
    Input::Action action(actionName);

    while (std::getline(sstr, token, '/'))
    {
        StringHelper::trim(token);

        action.addEvent(Input::fromString_Action_Event(token));
    }

    return action;
}
