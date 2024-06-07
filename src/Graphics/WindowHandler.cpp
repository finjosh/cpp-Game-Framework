#include "Graphics/WindowHandler.hpp"
#include "Graphics/DrawableManager.hpp"
#include "Graphics/CameraManager.hpp"
#include "Graphics/CanvasManager.hpp"
#include "Physics/WorldHandler.hpp"
#include "VectorConversions.hpp"
#include "SFML/Window/Mouse.hpp"

sf::RenderWindow* WindowHandler::m_renderWindow = nullptr;

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
    sf::Event event;
    event.type = sf::Event::MouseMoved;
    event.mouseMove.x = sf::Mouse::getPosition(*m_renderWindow).x;
    event.mouseMove.y = sf::Mouse::getPosition(*m_renderWindow).y;
    // CanvasManager::handleEvent(event);

    for (auto camera: CameraManager::m_cameras) //* Note this can not be multithreaded simply (camera uses regular events that could lead to memory races)
    {
        if (camera->DrawBackground.getNumCallbacks() > 0)
        {
            // reseting the view first
            auto view = camera->getCameraView();
            view.setCenter(view.getSize().x/2, view.getSize().y/2);
            m_renderWindow->setView(view);
            camera->DrawBackground.invoke(m_renderWindow, camera->getPixelSize());
        }
        
        m_renderWindow->setView(camera->getCameraView());
        camera->disableBlacklistedCanvases();
        CanvasManager::updateViewForCamera(camera);
        DrawableManager::draw(*m_renderWindow);
        camera->enableBlacklistedCanvases();
        WorldHandler::getWorld().DebugDraw();
        
        if (camera->DrawOverlay.getNumCallbacks() > 0)
        {
            // reseting the view first
            sf::View view = camera->getCameraView();
            view.setCenter(view.getSize().x/2, view.getSize().y/2);
            m_renderWindow->setView(view);
            camera->DrawOverlay.invoke(m_renderWindow, camera->getPixelSize());
        }
    }

    CanvasManager::drawOverlayGUI();
    // draw for tgui
    // auto camera = CameraManager::getMainCamera(); // TODO main camera can be nullptr
    // gui.setAbsoluteViewport(tgui::FloatRect{-camera->getPosition().x*PIXELS_PER_METER + camera->getPixelSize().x/2, -camera->getPosition().y*PIXELS_PER_METER + camera->getPixelSize().y/2, camera->getPixelSize().x, camera->getPixelSize().y});
    // tgui::Event mouseMoveEvent;
    // mouseMoveEvent.type = tgui::Event::Type::MouseMoved;
    // mouseMoveEvent.mouseMove.x = sf::Mouse::getPosition(*m_renderWindow).x;
    // mouseMoveEvent.mouseMove.y = sf::Mouse::getPosition(*m_renderWindow).y;
    // gui.handleEvent(mouseMoveEvent);
    // gui.draw(); // TODO make canvases
    // display for sfml window
    m_renderWindow->display();
    m_renderWindow->clear();
}

b2Vec2 WindowHandler::getMousePos()
{
    auto temp = convertVec2<int>(sf::Mouse::getPosition(*m_renderWindow)) - b2Vec2(m_renderWindow->getSize().x/2, m_renderWindow->getSize().y/2);
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
