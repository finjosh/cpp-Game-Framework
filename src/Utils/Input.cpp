#include "Utils/Input.hpp"

Input::Input()
{
    for (int i = -1; i < sf::Keyboard::Scancode::ScancodeCount; i++)
        m_keyboard[sf::Keyboard::Scancode(i)] = State::Up;
    
    for (int i = 0; i < sf::Mouse::Button::ButtonCount; i++)
        m_mouse[sf::Mouse::Button(i)] = State::Up;
}

Input& Input::get()
{
    static Input singleton;
    return singleton;
}

void Input::HandelEvent(sf::Event event, bool handleEvent)
{
    for (FrameData data: m_lastFrame) // updating just down/up to down/up
    {
        if (data.type == FrameData::MouseDown)
            m_mouse[data.mouseButton] = State::Down;
        else if (data.type == FrameData::MouseUp)
            m_mouse[data.mouseButton] = State::Up;
        else if (data.type == FrameData::KeyboardDown)
            m_keyboard[data.scanCode] = State::Down;
        else if (data.type == FrameData::KeyboardUp)
            m_keyboard[data.scanCode] = State::Up;
    }
    m_lastFrame.clear();

    if (event.type == sf::Event::KeyPressed && handleEvent)
    {
        m_keyboard[event.key.scancode] = State::Pressed;
        m_lastFrame.emplace_back(FrameData::KeyboardDown, event.key.scancode);
    }
    else if (event.type == sf::Event::KeyReleased)
    {
        m_keyboard[event.key.scancode] = State::Released;
        m_lastFrame.emplace_back(FrameData::KeyboardUp, event.key.scancode);
    }
    
    if (event.type == sf::Event::MouseButtonPressed && handleEvent)
    {
        m_mouse[event.mouseButton.button] = State::Pressed;
        m_lastFrame.emplace_back(FrameData::MouseDown, event.mouseButton.button);
    }
    else if (event.type == sf::Event::MouseButtonReleased)
    {
        m_mouse[event.mouseButton.button] = State::Released;
        m_lastFrame.emplace_back(FrameData::MouseUp, event.mouseButton.button);
    }
}

Input::State Input::getKeyState(sf::Keyboard::Key keyCode)
{
    return m_keyboard[sf::Keyboard::delocalize(keyCode)];
}

bool Input::isDown(sf::Keyboard::Key keycode)
{
    auto state = m_keyboard[sf::Keyboard::delocalize(keycode)];
    return state == State::Down || state == State::Pressed;
}

bool Input::isUp(sf::Keyboard::Key keycode)
{
    auto state = m_keyboard[sf::Keyboard::delocalize(keycode)];
    return state == State::Up || state == State::Released;
}

Input::State Input::getKeyState(sf::Keyboard::Scancode scanCode)
{
    return m_keyboard[scanCode];
}

bool Input::isDown(sf::Keyboard::Scancode scanCode)
{
    State state = m_keyboard[scanCode];
    return state == State::Down || state == State::Pressed;
}

bool Input::isUp(sf::Keyboard::Scancode scanCode)
{
    State state = m_keyboard[scanCode];
    return state == State::Up || state == State::Released;
}

Input::State Input::getMouseState(sf::Mouse::Button button)
{
    return m_mouse[button];
}

bool Input::isDown(sf::Mouse::Button button)
{
    State state = m_mouse[button];
    return state == State::Down || state == State::Pressed;
}

bool Input::isUp(sf::Mouse::Button button)
{
    State state = m_mouse[button];
    return state == State::Up || state == State::Released;
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

    case sf::Mouse::Button::XButton1:
        rtn = "Mouse Extra 1";
        break;

    case sf::Mouse::Button::XButton2:
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
        rtn += " " + std::to_string(scanCode);
    return rtn;
}
