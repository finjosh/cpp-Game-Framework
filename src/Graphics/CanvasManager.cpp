#include "Graphics/CanvasManager.hpp"
#include "Graphics/WindowHandler.hpp"
#include "Graphics/CameraManager.hpp"
#include <cassert>
#include <TGUI/ToolTip.hpp>

// TODO update for the following
// - need to stop changing the size of the root tgui container
//   - only change root container size when the main camera size changes
//   - when rendering need to handle window size manually with scaling
// possibly dont use the root container at all and build own backend
//   - might be difficult when dealing with stuff like tooltips and combo box list boxes
// Use two gui objects one for global and one for local gui
//   - this could still slow down if there are lots of global gui as the container size will still change a lot
//      - change with handling events, drawing (also per camera), ect.

Gui::Gui(sf::RenderWindow& window) : tgui::Gui(window) {}
Gui::Gui(sf::RenderTarget& target) : tgui::Gui(target) {}
tgui::Widget::Ptr Gui::getToolTip()
{
    return m_visibleToolTip;
}

void Gui::setRelativeViewport_notContainer(const tgui::FloatRect& viewport)
{
    m_viewport = {tgui::RelativeValue(viewport.left), tgui::RelativeValue(viewport.top), tgui::RelativeValue(viewport.width), tgui::RelativeValue(viewport.height)};
}

void Gui::setAbsoluteView_notContainer(const tgui::FloatRect& view)
{
    m_view = {view.left, view.top, view.width, view.height};
}

Gui* CanvasManager::m_gui;
std::set<Canvas*, _drawableComp> CanvasManager::m_canvases;

void CanvasManager::initGUI()
{
    if (WindowHandler::getRenderWindow() != nullptr)
        m_gui = new Gui(*WindowHandler::getRenderWindow());
    
    WindowHandler::onRenderWindowChanged([](sf::RenderWindow* window){
        assert("Render window must not be nullptr" && window != nullptr);

        if (m_gui)
            m_gui->setWindow(*window);
        else
            m_gui = new Gui(*window);
    });
}

void CanvasManager::closeGUI()
{
    if (m_gui)
    {
        m_gui->removeAllWidgets();
        delete(m_gui);
        m_gui = nullptr;
    }
}

bool CanvasManager::handleEvent(sf::Event event)
{
    Vector2 screenPosition; // left, top in pixels
    Vector2 screenSize = m_gui->getTarget()->getSize();
    if (auto camera = CameraManager::getMainCamera())
    {
        screenPosition = camera->getPosition();
        screenPosition *= PIXELS_PER_METER;
        screenPosition -= screenSize/2;
    }
    else
    {
        screenPosition = Vector2{0,0};
    }

    // first update all screen space canvases to have the proper position for events to be handled
    for (auto canvas: m_canvases)
    {
        const tgui::Vector2f origin = {canvas->getOrigin().x * canvas->getSize().x, canvas->getOrigin().y * canvas->getSize().y};
        canvas->getGroup()->setPosition(screenPosition.x + origin.x, screenPosition.y + origin.y);
        // canvas->getGroup()->moveToFront();
    }

    // setting the viewport so mouse position events are updated properly
    m_gui->setAbsoluteView(tgui::FloatRect{screenPosition.x,screenPosition.y,screenSize.x,screenSize.y}); // setting the viewport so mouse position events are updated properly
    m_gui->setRelativeViewport(tgui::FloatRect{0,0,1,1});
    return m_gui->handleEvent(event);
}

Gui* CanvasManager::getGui()
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
    assert(WindowHandler::getRenderWindow() != nullptr);

    m_gui->updateTime();

    if (!m_gui->getTarget() || (m_gui->getWindow()->getSize().x == 0) || (m_gui->getWindow()->getSize().y == 0) || (m_gui->getView().getWidth() <= 0) || (m_gui->getView().getHeight() <= 0))
        return;

    // m_gui->setAbsoluteView(tgui::FloatRect{0,0,(float)m_gui->getWindow()->getSize().x,(float)m_gui->getWindow()->getSize().y});
    // m_gui->setRelativeViewport(tgui::FloatRect{0,0,1,1});
    
    // Draw the canvases
    for (auto canvas: m_canvases)
    {
        if (canvas->isEnabled() /* && canvas->isScreenSpace() */)
            canvas->m_draw(m_gui->getTarget(), {}); // should be the default transform as screen space canvases dont care about pos or rotation
    }

    // TODO fix when camera moves after combo box list is showing
    for (auto widget: CanvasManager::getGui()->getWidgets()) // TODO do this more optimally?
    {
        if (widget->getWidgetName().starts_with("#TGUI_INTERNAL$ComboBoxListBox#"))
        {
            tgui::RenderStates widgetStates;
            Vector2 offset;
            if (auto camera = CameraManager::getMainCamera())
            {
                offset = camera->getPosition() - WindowHandler::getScreenSize()/2;
                offset *= PIXELS_PER_METER;
            }

            if (widget->getPosition().y - offset.y + widget->getFullSize().y > m_gui->getWindow()->getSize().y)
            {
                widget->setHeight(m_gui->getWindow()->getSize().y - widget->getPosition().y + offset.y);
            }

            widgetStates.transform.translate(widget->getPosition() - tgui::Vector2f{widget->getOrigin().x * widget->getSize().x, widget->getOrigin().y * widget->getSize().y});
            if (widget->getRotation() != 0)
            {
                const tgui::Vector2f rotOrigin{widget->getRotationOrigin().x * widget->getSize().x, widget->getRotationOrigin().y * widget->getSize().y};
                widgetStates.transform.rotate(widget->getRotation(), rotOrigin);
            }
            if ((widget->getScale().x != 1) || (widget->getScale().y != 1))
            {
                const tgui::Vector2f scaleOrigin{widget->getScaleOrigin().x * widget->getSize().x, widget->getScaleOrigin().y * widget->getSize().y};
                widgetStates.transform.scale(widget->getScale(), scaleOrigin);
            }
            CanvasManager::getGui()->getBackendRenderTarget()->drawWidget(widgetStates, widget);
            
            break;
        }
    }

    if (m_gui->getToolTip() != nullptr)
    {
        auto toolTip = m_gui->getToolTip();
        tgui::RenderStates widgetStates;
        widgetStates.transform.translate((tgui::Vector2f)WindowHandler::getMousePos()*PIXELS_PER_METER - tgui::Vector2f{toolTip->getOrigin().x * toolTip->getSize().x, toolTip->getOrigin().y * toolTip->getSize().y});
        if (toolTip->getRotation() != 0)
        {
            const tgui::Vector2f rotOrigin{toolTip->getRotationOrigin().x * toolTip->getSize().x, toolTip->getRotationOrigin().y * toolTip->getSize().y};
            widgetStates.transform.rotate(toolTip->getRotation(), rotOrigin);
        }
        if ((toolTip->getScale().x != 1) || (toolTip->getScale().y != 1))
        {
            const tgui::Vector2f scaleOrigin{toolTip->getScaleOrigin().x * toolTip->getSize().x, toolTip->getScaleOrigin().y * toolTip->getSize().y};
            widgetStates.transform.scale(toolTip->getScale(), scaleOrigin);
        }
        CanvasManager::getGui()->getBackendRenderTarget()->drawWidget(widgetStates, toolTip);
    }
}

void CanvasManager::updateViewForCamera(Camera* camera)
{
    m_gui->setAbsoluteView(tgui::FloatRect{(camera->getPosition().x-camera->getSize().x/2)*PIXELS_PER_METER, (camera->getPosition().y-camera->getSize().y/2)*PIXELS_PER_METER, camera->getSize().x*PIXELS_PER_METER, camera->getSize().y*PIXELS_PER_METER});
    // m_gui->setAbsoluteView_notContainer(tgui::FloatRect{(camera->getPosition().x-camera->getSize().x/2)*PIXELS_PER_METER, (camera->getPosition().y-camera->getSize().y/2)*PIXELS_PER_METER, camera->getSize().x*PIXELS_PER_METER, camera->getSize().y*PIXELS_PER_METER});
    auto temp = camera->getScreenRect();
    m_gui->setRelativeViewport(tgui::FloatRect{temp.position.x - temp.size.x/2, temp.position.y - temp.size.y/2, temp.size.x, temp.size.y});
    // m_gui->setRelativeViewport_notContainer({temp.left, temp.top, temp.width, temp.height});
}
