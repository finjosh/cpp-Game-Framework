#include "Graphics/CanvasManager.hpp"
#include "Graphics/WindowHandler.hpp"
#include "Graphics/Camera.hpp"

tgui::Gui CanvasManager::m_gui;
std::set<Canvas*, _drawableComp> CanvasManager::m_canvases;

void CanvasManager::initGUI()
{
    m_gui.setWindow(*WindowHandler::getRenderWindow());
}

void CanvasManager::handleEvent(sf::Event event, b2Vec2 center)
{
    m_gui.setAbsoluteView(tgui::FloatRect{center.x-(float)m_gui.getWindow()->getSize().x/2,center.y-(float)m_gui.getWindow()->getSize().y/2,(float)m_gui.getWindow()->getSize().x,(float)m_gui.getWindow()->getSize().y}); // setting the viewport so mouse position events are updated properly
    m_gui.handleEvent(event);
}

tgui::Gui& CanvasManager::getGui()
{
    return m_gui;
}

void CanvasManager::addCanvas(Canvas* canvas)
{
    m_canvases.insert(canvas);
}

void CanvasManager::removeCanvas(Canvas* canvas)
{
    m_canvases.erase(canvas);
}

void CanvasManager::drawOverlayGUI()
{
    if (!m_gui.getTarget() || (m_gui.getWindow()->getSize().x == 0) || (m_gui.getWindow()->getSize().y == 0) || (m_gui.getView().getWidth() <= 0) || (m_gui.getView().getHeight() <= 0))
        return;
    
    // const sf::View oldView = m_gui.getTarget()->getView();
    // // Change the view
    // sf::View newView(sf::Vector2f((float)m_gui.getTarget()->getSize().x/2, (float)m_gui.getTarget()->getSize().y/2), sf::Vector2f(m_gui.getTarget()->getSize()));
    // newView.setViewport({0,0,1,1});
    // m_gui.getTarget()->setView(newView);

    // m_gui.setAbsoluteViewport(tgui::FloatRect{0,0,(float)m_gui.getWindow()->getSize().x,(float)m_gui.getWindow()->getSize().y});/
    m_gui.setAbsoluteView(tgui::FloatRect{0,0,(float)m_gui.getWindow()->getSize().x,(float)m_gui.getWindow()->getSize().y});
    
    // Draw the canvases
    for (auto canvas: m_canvases)
    {
        if (canvas->isEnabled())
            m_gui.getBackendRenderTarget()->drawWidget({}, canvas->getWidget());
    }

    // Restore the old view
    // m_gui.getTarget()->setView(oldView);
}

void CanvasManager::updateViewForCamera(Camera* camera)
{
    m_gui.setAbsoluteView(tgui::FloatRect{(camera->getPosition().x-camera->getSize().x/2)*PIXELS_PER_METER, (camera->getPosition().y-camera->getSize().y/2)*PIXELS_PER_METER, camera->getSize().x*PIXELS_PER_METER, camera->getSize().y*PIXELS_PER_METER});
}
