#include "Graphics/WindowHandler.hpp"
#include "Graphics/DrawableManager.hpp"
#include "Graphics/CameraManager.hpp"
#include "Graphics/CanvasManager.hpp"
#include "Physics/WorldHandler.hpp"
#include "SFML/Window/Mouse.hpp"

sf::RenderWindow* WindowHandler::m_renderWindow = nullptr;
Vector2 WindowHandler::m_lastMousePos = {0,0};

sf::RenderWindow* WindowHandler::getRenderWindow()
{
    return m_renderWindow;
}

void WindowHandler::setRenderWindow(sf::RenderWindow* renderWindow)
{
    m_renderWindow = renderWindow;
}

void WindowHandler::Display()
{
    if (!m_renderWindow)
        return;

    // Emitting an mouse move event to the gui so that it updates any canvas position changes
    Vector2 newMousePos = WindowHandler::getMousePos();
    if (m_lastMousePos != newMousePos)
    {
        m_lastMousePos = newMousePos;
        sf::Event event;
        event.type = sf::Event::MouseMoved;
        event.mouseMove.x = sf::Mouse::getPosition(*m_renderWindow).x;
        event.mouseMove.y = sf::Mouse::getPosition(*m_renderWindow).y;
        CanvasManager::handleEvent(event);
    }

    if (CameraManager::m_cameras.size() == 0)
    {   
        DrawableManager::draw(m_renderWindow);
        WorldHandler::getWorld().DebugDraw();
    }
    for (auto camera: CameraManager::m_cameras) //* Note this can not be multithreaded simply (camera uses regular events that could lead to memory races)
    {
        if (!camera->isDisplaying())
            return;

        m_renderWindow->setView(camera->getCameraView()); // updates drawable objects view
        CanvasManager::updateViewForCamera(camera); // updates the UI view
        
        camera->m_drawBackground((sf::RenderTarget*)m_renderWindow);
        
        camera->disableBlacklistedCanvases();
        DrawableManager::draw(m_renderWindow);
        camera->enableBlacklistedCanvases();
        WorldHandler::getWorld().DebugDraw();

        camera->m_drawOverlay((sf::RenderTarget*)m_renderWindow);
    }

    CanvasManager::drawOverlayGUI();
    m_renderWindow->display();
    m_renderWindow->clear();
}

Vector2 WindowHandler::getMousePos()
{
    Vector2 temp(sf::Mouse::getPosition(*m_renderWindow));
    temp -= Vector2(m_renderWindow->getSize().x/2, m_renderWindow->getSize().y/2);
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
    return Vector2{sf::Mouse::getPosition(*m_renderWindow)}/PIXELS_PER_METER;
}

Vector2 WindowHandler::getScreenSize()
{
    return Vector2{m_renderWindow->getSize()} / PIXELS_PER_METER;
}
