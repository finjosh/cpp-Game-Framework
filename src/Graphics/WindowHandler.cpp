#include "Graphics/WindowHandler.hpp"
#include "Graphics/DrawableManager.hpp"
#include "Graphics/CameraManager.hpp"
#include "Graphics/CanvasManager.hpp"
#include "Physics/WorldHandler.hpp"
#include "Physics/DebugDraw.hpp"
#include "SFML/Window/Mouse.hpp"

sf::RenderWindow* WindowHandler::m_renderWindow = nullptr;
Vector2 WindowHandler::m_lastMousePos = {0,0};
EventHelper::EventDynamic<sf::RenderWindow*> WindowHandler::onRenderWindowChanged;
sf::VideoMode WindowHandler::m_videoMode;
sf::ContextSettings WindowHandler::m_contextSettings;
std::uint32_t WindowHandler::m_style;
std::string WindowHandler::m_title;
sf::State WindowHandler::m_state;
unsigned int WindowHandler::m_FPSLimit = 0;

sf::RenderWindow* WindowHandler::getRenderWindow()
{
    return m_renderWindow;
}

void WindowHandler::initRenderWindowSettings(sf::VideoMode mode, const sf::String &title, std::uint32_t style, sf::State state, const sf::ContextSettings &settings)
{
    m_videoMode = mode;
    m_contextSettings = settings;
    m_style = style;
    m_state = state;
    m_title = title;
}

void WindowHandler::createRenderWindow()
{
    sf::RenderWindow* temp = m_renderWindow;

    m_renderWindow = new sf::RenderWindow(m_videoMode, m_title, m_style, m_state, m_contextSettings);
    m_renderWindow->setFramerateLimit(m_FPSLimit);
    onRenderWindowChanged.invoke(m_renderWindow);
    if (temp != nullptr) // The previous window has to be deleted after the new one is created or else it will crash
        delete(temp);
}

void WindowHandler::Display()
{
    assert("Must set render window before trying to draw" && m_renderWindow);

    // Emitting an mouse move event to the gui so that it updates any canvas position changes
    Vector2 newMousePos = WindowHandler::getMousePos();
    if (m_lastMousePos != newMousePos)
    {
        m_lastMousePos = newMousePos;
        sf::Event::MouseMoved temp;
        temp.position = sf::Mouse::getPosition(*m_renderWindow);
        sf::Event event(temp);
        CanvasManager::handleEvent(event);
    }

    if (CameraManager::m_cameras.size() == 0)
    {   
        DrawableManager::draw(m_renderWindow, m_contextSettings);
        DebugDraw::get().draw(WorldHandler::get(), m_renderWindow);
    }
    for (auto camera: CameraManager::m_cameras)
    {
        if (!camera->isDisplaying())
            return;

        m_renderWindow->setView(camera->getCameraView()); // updates drawable objects view
        /// TODO make a global and screen space GUI so that global UI does not need to change its view port all the time (try to find a way around this for global as its not efficient)
        // CanvasManager::updateViewForCamera(camera); // updates the UI view // TODO uncomment this is a quick fix for frame drops when camera view is different from normal view (global gui does not work with this commented)
        
        camera->m_drawBackground((sf::RenderTarget*)m_renderWindow);
        
        camera->disableBlacklistedCanvases();
        DrawableManager::draw(m_renderWindow, m_contextSettings);
        camera->enableBlacklistedCanvases();
        DebugDraw::get().draw(WorldHandler::get(), m_renderWindow);
        
        camera->m_drawOverlay((sf::RenderTarget*)m_renderWindow);
    }

    CanvasManager::drawOverlayGUI();
    m_renderWindow->display();
    m_renderWindow->clear();
    if (auto mainCamera = CameraManager::getMainCamera())
        m_renderWindow->setView(mainCamera->getCameraView());
}

Vector2 WindowHandler::getMousePos()
{
    assert("Must set render window before trying to get mouse position" && m_renderWindow);

    Vector2 temp(sf::Mouse::getPosition(*m_renderWindow));
    temp.x *= m_renderWindow->getView().getSize().x / m_renderWindow->getSize().x;
    temp.y *= m_renderWindow->getView().getSize().y / m_renderWindow->getSize().y;
    temp -= Vector2(m_renderWindow->getView().getSize().x/2, m_renderWindow->getView().getSize().y/2);
    temp *= 1/PIXELS_PER_METER;
    if (auto camera = CameraManager::getMainCamera())
    {
        if (camera->isRotationLocked())
            return temp + camera->getPosition();
        else
            return camera->getGlobalPoint(temp);
    }
    return temp;
}

Vector2 WindowHandler::getMouseScreenPos()
{
    assert("Must set render window before trying to get screen size" && m_renderWindow);
    return Vector2{sf::Mouse::getPosition(*m_renderWindow)}/PIXELS_PER_METER;
}

Vector2 WindowHandler::getScreenSize()
{
    assert("Must set render window before trying to get screen size" && m_renderWindow);
    return Vector2{m_renderWindow->getSize()} / PIXELS_PER_METER;
}

void WindowHandler::setContextSettings(sf::ContextSettings contextSettings)
{
    initRenderWindowSettings(m_videoMode, m_title, m_style, m_state, contextSettings);
}

sf::ContextSettings WindowHandler::getContextSettings()
{
    return m_contextSettings;
}

void WindowHandler::setVideMode(sf::VideoMode mode)
{
    initRenderWindowSettings(mode, m_title, m_style, m_state, m_contextSettings);
}

void WindowHandler::setState(sf::State state)
{
    initRenderWindowSettings(m_videoMode, m_title, m_style, state, m_contextSettings);
}

sf::State WindowHandler::getState()
{
    return m_state;
}

sf::VideoMode WindowHandler::getVideMode()
{
    return m_videoMode;
}

void WindowHandler::setStyle(std::uint32_t style)
{
    initRenderWindowSettings(m_videoMode, m_title, style, m_state, m_contextSettings);
}

std::uint32_t WindowHandler::getStyle()
{
    return m_style;
}

void WindowHandler::setTitle(const std::string& title)
{
    m_title = title;
    if (m_renderWindow != nullptr)
    {
        m_renderWindow->setTitle(title);
    }
    #ifdef DEBUG
    else
        std::cout << __FUNCTION__ << " - Unable to set title of window immediately since no window exists" << std::endl;
    #endif
}

std::string WindowHandler::getTitle()
{
    return m_title;
}

void WindowHandler::setFPSLimit(unsigned int value)
{
    m_FPSLimit = value;
    if (m_renderWindow != nullptr)
    {
        m_renderWindow->setFramerateLimit(value);
    }
    #ifdef DEBUG
    else
        std::cout << __FUNCTION__ << " - Unable to set FPS limit of window immediately since no window exists" << std::endl;
    #endif
}

unsigned int WindowHandler::getFPSLimit()
{
    return m_FPSLimit;
}
