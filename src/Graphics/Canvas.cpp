#include "Graphics/Canvas.hpp"
#include "Graphics/CanvasManager.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/DrawableManager.hpp"

Canvas::Canvas() : DrawableObject(0, DrawStage::UI)
{
    DrawableManager::removeDrawable(this);

    m_group = tgui::Group::create();
    CanvasManager::m_gui->add(m_group);
    CanvasManager::addCanvas(this);

    // removing the UI component as soon as object is added to queue
    m_onDestroy([this](){ 
        this->m_group = nullptr;
        CanvasManager::m_gui->remove(m_group);
        if (m_screenSpace)
            CanvasManager::removeCanvas(this);
    });
    m_onTransformUpdated([this](){
        if (isScreenSpace())
            return;

        if (!isRotationLocked())
            this->m_group->setRotation(getGlobalRotation().getAngle()*180/PI);

        Vector2 globalPos = Object::getGlobalPosition()*PIXELS_PER_METER;
        this->m_group->setPosition(globalPos.x, globalPos.y);
    });
    m_onEnabled([this](){
        this->m_group->setEnabled(true); //! MUST call set enabled first (something in tgui backend stops it from registering events otherwise)
        this->m_group->setVisible(true);
    });
    m_onDisabled([this](){
        this->m_group->setEnabled(false); //! MUST call set enabled first (something in tgui backend stops it from registering events otherwise)
        this->m_group->setVisible(false);
    });
}

Canvas::~Canvas()
{
}

void Canvas::Draw(sf::RenderTarget* target, const Transform& parentTransform)
{
    tgui::RenderStates widgetStates;
    if (!isScreenSpace()) // only draw in the set position and rotation IF not in screen space
    {
        widgetStates.transform.translate(m_group->getPosition() - tgui::Vector2f{m_group->getOrigin().x * m_group->getSize().x, m_group->getOrigin().y * m_group->getSize().y});
        if (m_group->getRotation() != 0)
        {
            const tgui::Vector2f rotOrigin{m_group->getRotationOrigin().x * m_group->getSize().x, m_group->getRotationOrigin().y * m_group->getSize().y};
            widgetStates.transform.rotate(m_group->getRotation(), rotOrigin);
        }
        if ((m_group->getScale().x != 1) || (m_group->getScale().y != 1))
        {
            const tgui::Vector2f scaleOrigin{m_group->getScaleOrigin().x * m_group->getSize().x, m_group->getScaleOrigin().y * m_group->getSize().y};
            widgetStates.transform.scale(m_group->getScale(), scaleOrigin);
        }
    }
    CanvasManager::getGui()->getBackendRenderTarget()->drawWidget(widgetStates, m_group);
}

void Canvas::setScreenSpace()
{
    m_screenSpace = true;
    CanvasManager::addCanvas(this);
    DrawableManager::removeDrawable(this);
}

void Canvas::setGlobalSpace()
{
    m_screenSpace = false;
    CanvasManager::removeCanvas(this);
    DrawableManager::addDrawable(this);
}

bool Canvas::isScreenSpace() const
{
    return m_screenSpace;
}

void Canvas::setRotationLocked(bool locked)
{
    m_rotationLock = locked;
}

bool Canvas::isRotationLocked() const
{
    return m_rotationLock;
}

tgui::Group::Ptr Canvas::getGroup()
{
    return m_group;
}

tgui::Group::ConstPtr Canvas::getGroup() const
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
