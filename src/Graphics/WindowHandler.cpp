#include "Graphics/WindowHandler.hpp"
#include "Graphics/DrawableManager.hpp"
#include "Graphics/CameraManager.hpp"
#include "Physics/WorldHandler.hpp"
#include "VectorConversions.hpp"

sf::RenderWindow* WindowHandler::m_renderWindow = nullptr;

sf::RenderWindow* WindowHandler::getRenderWindow()
{
    return m_renderWindow;
}

void WindowHandler::setRenderWindow(sf::RenderWindow* renderWindow)
{
    m_renderWindow = renderWindow;
}

void WindowHandler::Display(tgui::Gui& gui)
{
    for (auto camera: CameraManager::m_cameras) // Note this can not be multithreaded easily (camera uses regular events that could lead to memory races)
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
        DrawableManager::draw(*m_renderWindow);
        WorldHandler::getWorld().DebugDraw();
        
        if (camera->DrawOverlay.getNumCallbacks() > 0)
        {
            // reseting the view first
            auto view = camera->getCameraView();
            view.setCenter(view.getSize().x/2, view.getSize().y/2);
            m_renderWindow->setView(view);
            camera->DrawOverlay.invoke(m_renderWindow, camera->getPixelSize());
        }
    }

    if (auto camera = CameraManager::getMainCamera()) // TODO do this after drawing UI and add canvases
        m_renderWindow->setView(camera->getCameraView());

    // draw for tgui
    gui.draw(); // TODO make canvases
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
