#ifndef CAMERA_HPP
#define CAMERA_HPP

#pragma once

#include <list>
#include "SFML/System/Vector2.hpp"
#include "Utils/EventHelper.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "Graphics/Canvas.hpp"
#include "Object.hpp" // have to inlcude after sfml vector is

class WindowHandler;

/// @note position is always the center of the camera view
class Camera : virtual public Object
{
public:
    using Ptr = Object::Ptr<Camera>;

    Camera(int layer = 0);
    virtual ~Camera();

    /// @brief sets the layer which this camera will be drawn on (relative to other cameras NOT drawable objects)
    /// @note lower is sooner
    /// @warning if this is the main camera then it will be removed and the next lowest layer camera will be made the main camera
    void setLayer(int layer);
    /// @returns this cameras layer
    int getLayer() const;
    /// @brief sets the layer to -1
    /// @note sets this as the main camera in CameraManager
    /// @note main cameras have a fixed screenRect (0,0,1,1)
    void setMainCamera();
    bool isMainCamera() const;
    /// @brief sets the size of the view
    void setViewSize(Vector2 size);
    /// @brief sets the size of the view
    void setViewSize(float x, float y);
    /// @returns the size in terms of meters
    Vector2 getSize() const;
    /// @returns the size in terms of pixels
    Vector2 getPixelSize() const;
    /// @brief resizes the camera from the current size
    void zoom(float factor);
    /// @brief sets if this camera is displaying anything
    void setDisplaying(bool displaying = true);
    /// @returns if this camera is displaying
    bool isDisplaying() const;
    /// @brief sets the rect where this camera will display on the screen
    /// @note each value is from 0-1
    /// @note nothing happens if this is the main camera
    void setScreenRect(const sf::FloatRect& screenRect);
    sf::FloatRect getScreenRect() const;
    /// @returns the data stored in this object as an sfml view object
    sf::View getCameraView() const;
    /// @note this is only for camera NOT object
    void setRotationLocked(bool locked = true);
    /// @note this is only for camera NOT object
    bool isRotationLocked() const;

    /// @brief called when starting to render this camera
    /// @note gives pointer to the window that should be rendered to
    /// @note gives the size of the camera in pixels
    EventHelper::EventDynamic2<sf::RenderTarget*, sf::Vector2f> DrawBackground;
    /// @brief called after rendering everything BUT UI
    /// @note gives pointer to the window that should be rendered to
    /// @note gives the size of the camera in pixels
    EventHelper::EventDynamic2<sf::RenderTarget*, sf::Vector2f> DrawOverlay;

    /// @brief stops the given canvas from being displayed in this camera
    void blacklistCanvas(Canvas* canvas);
    /// @brief removes the given canvas from the blacklist
    void whitelistCanvas(Canvas* canvas);
    /// @returns true if the canvas is in the blacklist
    bool isBlacklisted(Canvas* canvas);

protected:
    void disableBlacklistedCanvases();
    void enableBlacklistedCanvases();

    /// @brief sets the targets view calls the DrawBackground event and resets view to what it was before
    void m_drawBackground(sf::RenderTarget* target);
    /// @brief sets the targets view calls the DrawOverlay event and resets view to what it was before
    void m_drawOverlay(sf::RenderTarget* target);

    friend WindowHandler;

private:
    int m_layer = 0;
    Vector2 m_size;
    bool m_enabled = true;
    bool m_rotationLock = false;
    sf::FloatRect m_screenRect = {0,0,1,1};
    bool m_displaying = true;
    /// @brief canvases to ignore when drawing to this camera
    std::list<Object::Ptr<Canvas>> m_canvases;
};

#endif
