#include "Graphics/Canvas.hpp"
#include "Graphics/CanvasManager.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/DrawableManager.hpp"

Canvas::Canvas() : DrawableObject(0, DrawStage::UI)
{
    m_group = tgui::Group::create();
    CanvasManager::m_gui.add(m_group);
    CanvasManager::addCanvas(this);

    // removing the UI component as soon as object is added to queue
    m_onDestroy([this](){ 
        m_group = nullptr;
        CanvasManager::m_gui.remove(m_group);
        if (m_screenSpace)
            CanvasManager::removeCanvas(this);
    });
    m_onTransformUpdated([this](){
        m_group->setPosition(getPosition().x*PIXELS_PER_METER, getPosition().y*PIXELS_PER_METER);
        m_group->setRotation(getRotation()*180/PI);
    });
    m_onEnabled([this](){
        m_group->setVisible(true);
        m_group->setEnabled(true);
    });
    m_onDisabled([this](){
        m_group->setVisible(false);
        m_group->setEnabled(false);
    });
}

Canvas::~Canvas()
{
}

void Canvas::Draw(sf::RenderWindow& window)
{
    if (!isScreenSpace())
        CanvasManager::getGui().getBackendRenderTarget()->drawWidget({}, m_group);
}

void Canvas::setScreenSpace()
{
    m_screenSpace = true;
    CanvasManager::addCanvas(this);
}

void Canvas::setGlobalSpace()
{
    m_screenSpace = false;
    CanvasManager::removeCanvas(this);
}

bool Canvas::isScreenSpace()
{
    return m_screenSpace;
}

tgui::Group::Ptr Canvas::getWidget()
{
    return m_group;
}

tgui::Group::ConstPtr Canvas::getWidget() const
{
    return m_group;
}

void Canvas::setSize(const tgui::Layout2d& size)
{
    m_group->setSize(size);
}

TGUI_NODISCARD tgui::Vector2f Canvas::getSize() const
{
    return m_group->getSize();
}

void Canvas::setOrigin(float x, float y)
{
    setOrigin({x, y});
}

void Canvas::setOrigin(tgui::Vector2f origin)
{
    m_group->setOrigin(origin);
}

TGUI_NODISCARD tgui::Vector2f Canvas::getOrigin() const
{
    return m_group->getOrigin();
}

TGUI_NODISCARD tgui::Vector2f Canvas::getFullSize() const
{
    return m_group->getFullSize();
}

TGUI_NODISCARD tgui::Vector2f Canvas::getInnerSize() const
{
    return m_group->getInnerSize();
}

TGUI_NODISCARD tgui::Vector2f Canvas::getChildWidgetsOffset() const
{
    return m_group->getChildWidgetsOffset(); 
}

TGUI_NODISCARD const std::vector<tgui::Widget::Ptr>& Canvas::getWidgets() const
{
    return m_group->getWidgets();
}

void Canvas::add(const tgui::Widget::Ptr& widgetPtr, const tgui::String& widgetName)
{
    m_group->add(widgetPtr, widgetName);
}

bool Canvas::remove(const tgui::Widget::Ptr& widget)
{
    return m_group->remove(widget);
}

void Canvas::removeAllWidgets()
{
    m_group->removeAllWidgets();
}
