#include "Graphics/Camera.hpp"
#include "Graphics/CameraManager.hpp"
#include "Graphics/WindowHandler.hpp"

Camera::Camera(int layer) : m_layer(layer)
{
    CameraManager::addCamera(this);
    m_size = {WindowHandler::getRenderWindow()->getSize().x / PIXELS_PER_METER, WindowHandler::getRenderWindow()->getSize().y / PIXELS_PER_METER};
}

Camera::~Camera()
{
    CameraManager::removeCamera(this);
}

void Camera::setLayer(int layer)
{
    m_layer = layer;
}

int Camera::getLayer() const
{
    return m_layer;
}

void Camera::setMainCamera()
{
    m_layer = std::numeric_limits<int>().min();
    CameraManager::setMainCamera(this);
}

void Camera::setViewSize(b2Vec2 size)
{
    m_size = size;
}

void Camera::setViewSize(float x, float y)
{
    m_size = {x,y};
}

b2Vec2 Camera::getSize() const
{
    return m_size;
}

void Camera::zoom(float factor)
{
    m_size *= factor;
}

void Camera::setDisplaying(bool displaying)
{
    m_enabled = displaying;
}

bool Camera::isDisplaying() const
{
    return m_enabled;
}

void Camera::setScreenRect(const sf::FloatRect& screenRect)
{
    m_screenRect = screenRect;
}

sf::FloatRect Camera::getScreenRect() const
{
    return m_screenRect;
}

sf::View Camera::getCameraView() const
{
    sf::View temp(sf::Vector2f(Object::getPosition().x*PIXELS_PER_METER, Object::getPosition().y*PIXELS_PER_METER), {m_size.x*PIXELS_PER_METER, m_size.y*PIXELS_PER_METER});
    if (!m_rotationLock)
        temp.setRotation(Object::getRotation() * 180 / PI);
    temp.setViewport(m_screenRect);
    return temp;
}

void Camera::setRotationLocked(bool locked)
{
    m_rotationLock = locked;
}

bool Camera::isRotationLocked()
{
    return m_rotationLock;
}
