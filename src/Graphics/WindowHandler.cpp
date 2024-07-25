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
    TGUI_ASSERT(m_renderWindow, "Must set render window before trying to draw");

    // Emitting an mouse move event to the gui so that it updates any canvas position changes
    Vector2 newMousePos = WindowHandler::getMousePos();
    if (m_lastMousePos != newMousePos)
    {
        m_lastMousePos = newMousePos;
        sf::Event event;
        event.type = sf::Event::MouseMoved;
        Vector2 mousePos = sf::Mouse::getPosition(*m_renderWindow); // pos in pixel cords
        event.mouseMove.x = mousePos.x;
        event.mouseMove.y = mousePos.y;
        CanvasManager::handleEvent(event);
    }

    if (CameraManager::m_cameras.size() == 0)
    {   
        DrawableManager::draw(m_renderWindow);
        WorldHandler::getWorld().DebugDraw();
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
        DrawableManager::draw(m_renderWindow);
        camera->enableBlacklistedCanvases();
        WorldHandler::getWorld().DebugDraw();

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
    return Vector2{sf::Mouse::getPosition(*m_renderWindow)}/PIXELS_PER_METER;
}

Vector2 WindowHandler::getScreenSize()
{
    return Vector2{m_renderWindow->getSize()} / PIXELS_PER_METER;
}
