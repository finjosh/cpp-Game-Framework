#ifndef CANVAS_MANAGER_HPP
#define CANVAS_MANAGER_HPP

#pragma once

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "Canvas.hpp"

class Camera;

/// @note used to access protected variables
class Gui : public tgui::Gui
{
public:
    Gui(sf::RenderWindow& window);
    Gui(sf::RenderTarget& target);

    /// @returns the current visible tooltip
    tgui::Widget::Ptr getToolTip();

    /// @brief sets the viewport without changing the container size
    void setRelativeViewport_notContainer(const tgui::FloatRect& viewport);
    /// @brief sets the view without changing the container size
    void setAbsoluteView_notContainer(const tgui::FloatRect& view);

protected:

private:

};

class CanvasManager
{
public:
    /// @note call this AFTER the window handler has it's render window set
    static void initGUI();
    /// @brief frees up memory used by the gui
    static void closeGUI();
    /// @note call this for each event
    /// @note this changes the tgui AbsoluteView and RelativeViewport
    static bool handleEvent(sf::Event event);
    /// @warning do NOT remove any widgets from this only use canvases for adding widgets 
    static Gui* getGui();
    /// @brief this also updates the gui time
    static void drawOverlayGUI();
    /// @brief updates the gui view for the given camera
    static void updateViewForCamera(Camera* camera);

protected:
    /// @brief adds the canvas to the list of canvases that will draw to the screen space  
    static void addCanvas(Canvas* canvas);
    /// @brief removes the canvas from the list of canvases that will draw to the screen space
    static void removeCanvas(Canvas* canvas);

    friend Canvas;

private:
    inline CanvasManager() = default;

    static Gui* m_gui;
    /// @brief list of canvases that are drawn to the main screen
    static std::set<Canvas*, _drawableComp> m_canvases;
};

#endif
