#ifndef CAMERA_H
#define CAMERA_H

#pragma once

#include "SFML/Graphics/Transformable.hpp"
#include "SFML/Graphics/View.hpp"
#include "Object.hpp"
#include "Settings.hpp"
#include "Utils/EventHelper.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

// TODO make a UI camera for ui that does not move with the screen and is in world cords

/// @warning if derived from do NOT forget to use "createDestroy()"
/// @note this can be used as a stand alone object
/// @note position is always the center of the camera view
class Camera : virtual public Object
{
public:
    Camera(int layer = 0);
    ~Camera();

    /// @brief sets the layer which this camera will be drawn on (relative to other cameras NOT drawable objects)
    /// @note lower is sooner
    /// @warning if this is the main camera then it will be removed and the next lowest layer camera will be made the main camera
    void setLayer(int layer);
    /// @returns this cameras layer
    int getLayer() const;
    /// @brief sets the layer to -1
    /// @note sets this as the main camera in CameraManager
    void setMainCamera();
    bool isMainCamera();
    /// @brief sets the size of the view
    void setViewSize(b2Vec2 size);
    /// @brief sets the size of the view
    void setViewSize(float x, float y);
    /// @returns the size in terms of meters
    b2Vec2 getSize() const;
    /// @returns the size in terms of pixels
    sf::Vector2f getPixelSize() const;
    /// @brief resizes the camera from the current size
    void zoom(float factor);
    /// @brief sets if this camera is displaying anything
    void setDisplaying(bool displaying = true);
    /// @returns if this camera is displaying
    bool isDisplaying() const;
    /// @brief sets the rect where this camera will display on the screen
    /// @note each value is from 0-1
    void setScreenRect(const sf::FloatRect& screenRect);
    sf::FloatRect getScreenRect() const;
    /// @returns the data stored in this object as an sfml view object
    sf::View getCameraView() const;
    /// @note this is only for camera NOT object
    void setRotationLocked(bool locked = true);
    /// @note this is only for camera NOT object
    bool isRotationLocked();

    /// @brief called when starting to render this camera
    /// @note gives pointer to the window that should be rendered to
    /// @note gives the size of the camera in pixels
    EventHelper::EventDynamic2<sf::RenderWindow*, sf::Vector2f> DrawBackground;
    /// @brief called after rendering everything BUT UI
    /// @note gives pointer to the window that should be rendered to
    /// @note gives the size of the camera in pixels
    EventHelper::EventDynamic2<sf::RenderWindow*, sf::Vector2f> DrawOverlay;

protected:

private:
    int m_layer = 0;
    b2Vec2 m_size;
    bool m_enabled = true;
    bool m_rotationLock = false;
    sf::FloatRect m_screenRect = {0,0,1,1};
    createDestroy();
};

#endif
