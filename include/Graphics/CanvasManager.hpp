#ifndef CANVAS_MANAGER_H
#define CANVAS_MANAGER_H

#pragma once

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "Canvas.hpp"

class Camera;

class CanvasManager
{
public:
    /// @note call this AFTER the window handler has it's render window set
    static void initGUI();
    /// @note call this for each event
    /// @param center the global position for the center of the screen
    static void handleEvent(sf::Event event, b2Vec2 center);
    static tgui::Gui& getGui();
    static void drawOverlayGUI();
    static void updateViewForCamera(Camera* camera);

protected:
    /// @brief adds the canvas to the list of canvases that will draw to the screen space  
    static void addCanvas(Canvas* canvas);
    /// @brief removes the canvas from the list of canvases that will draw to the screen space
    static void removeCanvas(Canvas* canvas);

    friend Canvas;

private:
    inline CanvasManager() = default;

    static tgui::Gui m_gui;
    /// @brief list of canvases that are drawn to the main screen
    static std::set<Canvas*, _drawableComp> m_canvases;
};

#endif
