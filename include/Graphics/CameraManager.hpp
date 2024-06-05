#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#pragma once

#include <set>
#include "Camera.hpp"

struct _cameraComp
{
    bool operator() (const Camera* lhs, const Camera* rhs) const;
};

class WindowHandler;

class CameraManager
{
public:
    /// @note main camera defaults to the first camera added
    /// @returns pointer to the main camera if there is one else nullptr
    static Camera* getMainCamera();
    static void setMainCamera(Camera* camera);

protected:
    static void addCamera(Camera* camera);
    static void removeCamera(Camera* camera);

    friend Camera;
    friend WindowHandler;

private:
    inline CameraManager() = default;

    static std::set<Camera*, _cameraComp> m_cameras;
    static Camera* m_mainCamera;
};

#endif
