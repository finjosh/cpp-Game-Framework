#include "Graphics/CameraManager.hpp"

bool _cameraComp::operator()(const Camera* lhs, const Camera* rhs) const
{
    return lhs->getLayer() <= rhs->getLayer() && lhs->getID() < rhs->getID();
}

std::set<Camera*, _cameraComp> CameraManager::m_cameras;
Camera* CameraManager::m_mainCamera = nullptr;

void CameraManager::addCamera(Camera* camera)
{
    m_cameras.emplace(camera);
    if (!m_mainCamera)
        m_mainCamera = camera;
}

void CameraManager::removeCamera(Camera* camera)
{
    m_cameras.erase(camera);
    if (m_mainCamera == camera)
    {
        if (m_cameras.size() == 0)
            m_mainCamera = nullptr;
        else
            m_cameras.begin().operator*()->setMainCamera();
    }
}

Camera* CameraManager::getMainCamera()
{
    return m_mainCamera;
}

void CameraManager::setMainCamera(Camera* camera)
{
    m_mainCamera = camera;
}
