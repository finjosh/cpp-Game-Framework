#include "Graphics/Camera.hpp"
#include "Graphics/CameraManager.hpp"
#include "Graphics/WindowHandler.hpp"
#include "SFML/Graphics/View.hpp"

Camera::Camera(int layer) : m_layer(layer)
{
    CameraManager::addCamera(this);
    m_size = Vector2{WindowHandler::getRenderWindow()->getSize().x / PIXELS_PER_METER, WindowHandler::getRenderWindow()->getSize().y / PIXELS_PER_METER};
}

Camera::~Camera()
{
    CameraManager::removeCamera(this);
}

void Camera::setLayer(int layer)
{
    CameraManager::removeCamera(this);
    m_layer = layer;
    CameraManager::addCamera(this);
}

int Camera::getLayer() const
{
    return m_layer;
}

void Camera::setMainCamera()
{
    m_layer = -1;
    m_screenRect = {{0,0},{1,1}};
    m_size = Vector2{WindowHandler::getRenderWindow()->getSize().x / PIXELS_PER_METER, WindowHandler::getRenderWindow()->getSize().y / PIXELS_PER_METER};
    CameraManager::setMainCamera(this);
}

bool Camera::isMainCamera() const
{
    return (this == CameraManager::getMainCamera());
}

void Camera::setViewSize(Vector2 size)
{
    m_size = size;
}

void Camera::setViewSize(float x, float y)
{
    m_size = Vector2{x,y};
}

Vector2 Camera::getSize() const
{
    return m_size;
}

void Camera::zoom(float factor)
{
    m_size *= factor;
}

void Camera::setDisplaying(bool displaying)
{
    m_displaying = displaying;
}

bool Camera::isDisplaying() const
{
    return (isEnabled() && m_displaying);
}

void Camera::setScreenRect(const sf::FloatRect& screenRect)
{
    if (isMainCamera())
        return;
    m_screenRect = screenRect;
}

sf::FloatRect Camera::getScreenRect() const
{
    return m_screenRect;
}

sf::View Camera::getCameraView() const
{
    sf::View temp(sf::Vector2f(Object::getPosition().x*PIXELS_PER_METER, Object::getPosition().y*PIXELS_PER_METER), {m_size.x*PIXELS_PER_METER, m_size.y*PIXELS_PER_METER});
    temp.setViewport(m_screenRect);
    if (!m_rotationLock)
        temp.setRotation((sf::Angle)Object::getRotation());
    return temp;
}

void Camera::setRotationLocked(bool locked)
{
    m_rotationLock = locked;
}

bool Camera::isRotationLocked() const
{
    return m_rotationLock;
}

Vector2 Camera::getPixelSize() const
{
    return Vector2{m_size.x*PIXELS_PER_METER, m_size.y*PIXELS_PER_METER};
}

void Camera::disableBlacklistedCanvases()
{
    for (auto& canvas: m_canvases)
    {
        canvas->setEnabled(false);
    }
}

void Camera::enableBlacklistedCanvases()
{
    for (auto& canvas: m_canvases)
    {
        canvas->setEnabled(true);
    }
}

void Camera::blacklistCanvas(Canvas* canvas)
{
    if (canvas)
    {
        m_canvases.emplace_back(canvas);
    }
}

void Camera::whitelistCanvas(Canvas* canvas)
{
    if (canvas)
    {
        m_canvases.remove(canvas);
    }
}

bool Camera::isBlacklisted(Canvas* canvas)
{
    return (std::find(m_canvases.begin(), m_canvases.end(), canvas) != m_canvases.end());
}

void Camera::m_drawBackground(sf::RenderTarget* target)
{
    if (DrawBackground.getNumCallbacks() > 0)
    {
        const sf::View oldView = target->getView();
        const sf::Vector2f size = (sf::Vector2f)getPixelSize();
        sf::View view({size.x/2, size.y/2}, size);
        view.setViewport(m_screenRect);
        target->setView(view);
        DrawBackground.invoke(target, size);
        target->setView(oldView);
    }
}

void Camera::m_drawOverlay(sf::RenderTarget* target)
{
    if (DrawBackground.getNumCallbacks() > 0)
    {
        const sf::View oldView = target->getView();
        const sf::Vector2f size = (sf::Vector2f)getPixelSize();
        sf::View view({size.x/2, size.y/2}, size);
        view.setViewport(m_screenRect);
        target->setView(view);
        DrawBackground.invoke(target, size);
        target->setView(oldView);
    }
}
