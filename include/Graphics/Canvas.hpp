#ifndef CANVAS_HPP
#define CANVAS_HPP

#pragma once

#include "TGUI/Widgets/Group.hpp"
#include "Graphics/DrawableObject.hpp"
#include "Object.hpp"

// TODO make this work when the camera is rotated
/// @bug when the camera is rotated UI is not drawn properly (UI will be rotated around the origin to align with the window) and event dont work properly
/// @bug tooltips dont work when the camera is moving
/// @note layer and drawStage work with drawableObjects if drawing in global space
/// @note if in screen space layer and drawStage are between canvases only
/// @note defaults to drawing in screen space (screen overlay)
/// @note when a canvas is in screen space the position, rotation, and origin does not affect the canvas
/// @note when canvas is added to destroy queue the tgui object is set enabled(false) and visible(false) and full removed when fully destroyed
/// @warning all functions defined in the Canvas class return values in pixels NOT meters
class Canvas : public virtual Object, public DrawableObject
{
public:
    using Ptr = Object::Ptr<Canvas>;

    Canvas();
    virtual ~Canvas();

    /// @brief draws in the screen space as an overlay
    void setScreenSpace();
    /// @brief draws with other drawable objects
    void setGlobalSpace();
    bool isScreenSpace() const;

    void setVisible(bool visible = true);
    bool isVisible() const;

    // void setRotationLocked(bool locked = true);
    // bool isRotationLocked() const;

    /// @note position, rotation, transform, origin, and is enabled are controlled by this object DONT edit them
    /// @returns the group widget that stores all widgets in this canvas 
    tgui::Group::Ptr getGroup();
    /// @note position, rotation, transform, origin, and is enabled are controlled by this object DONT edit them
    /// @returns the group widget that stores all widgets in this canvas
    tgui::Group::ConstPtr getGroup() const;

    /// @brief Changes the size of the group
    /// @param size  The new size of the group
    void setSize(const tgui::Layout2d& size);
    /// @brief Returns the size of the widget
    /// @return Size of the widget in pixels
    TGUI_NODISCARD Vector2 getSize() const;
    /// @brief Sets the origin point on which the position, scale and rotation is based
    /// @param x  Relative horizontal position of the origin point
    /// @param y  Relative vertical position of the origin point
    /// @note Valid x and y values range from 0 to 1, with 0 representing the left/top of the widget and 1 being right/bottom
    /// @warning This function should not be used if the AutoLayout is set to something other than Manual
    void setOrigin(float x, float y);
    /// @brief Sets the origin point on which the position, scale and rotation is based
    /// @param origin  Relative position of the origin point
    /// @note Valid x and y values range from 0 to 1, with 0 representing the left/top of the widget and 1 being right/bottom
    /// @warning This function should not be used if the AutoLayout is set to something other than Manual
    void setOrigin(Vector2 origin);
    /// @brief Returns the relative origin point on which the position, scale and rotation is based
    /// @return Relative origin point of the widget (0 for left/top, 1 for right/bottom)
    TGUI_NODISCARD Vector2 getOrigin() const;
    /// @brief Returns the entire size that the widget is using
    /// This function will return a value equal or greater than what getSize returns.
    /// If the widget would e.g. have borders around it then this function will return the size, including these borders.
    /// @return Full size of the widget
    TGUI_NODISCARD Vector2 getFullSize() const;
    /// @brief Returns the space available for widgets inside the container
    /// @return Size without padding
    TGUI_NODISCARD Vector2 getInnerSize() const;
    /// @brief Returns the distance between the position of the container and a widget that would be drawn inside
    ///        this container on relative position (0,0)
    /// @return Offset of the widgets in the container
    TGUI_NODISCARD Vector2 getChildWidgetsOffset() const;
    /// @brief Returns a list of all the widgets in this container
    /// @return Vector of all widget pointers
    TGUI_NODISCARD const std::vector<tgui::Widget::Ptr>& getWidgets() const;
    /// @brief Sorts a list of all the widgets in this container
    /// @param function Comparison function object (i.e. an object that satisfies the requirements of Compare) which
    ///                 returns true if the first argument is less than (i.e. is ordered before) the second.
    template<typename Function>
    void sortWidgets(Function&& function);
    /// @brief Adds a widget to the container
    ///
    /// @param widgetPtr   Pointer to the widget you would like to add
    /// @param widgetName  You can give the widget a unique name to retrieve it from the container later
    /// @warning Widgets should be named as if they are C++ variables, i.e. names must not include any whitespace, or most
    ///          symbols (e.g.: +, -, *, /, ., &), and should not start with a number. If you do not follow these rules,
    ///          layout expressions may give unexpected results. Alphanumeric characters and underscores are safe to use,
    ///          and widgets are permitted to have no name.
    void add(const tgui::Widget::Ptr& widgetPtr, const tgui::String& widgetName = "");
    /// @brief Returns a pointer to a widget that was added earlier
    /// @param widgetName The name that was given to the widget when it was added to the container
    /// @note The container will first search for widgets that are direct children of it, but when none of the child widgets match the given name, a recursive search will be performed.
    /// @return Pointer to the earlier added widget
    /// @warning This function will return nullptr when an unknown widget name was passed
    TGUI_NODISCARD tgui::Widget::Ptr get(const tgui::String& widgetName) const;
    /// @brief Returns a pointer to a widget that was added earlier
    /// @param widgetName The name that was given to the widget when it was added to the container
    /// @return Pointer to the earlier added widget.
    /// @note The pointer will already be casted to the desired type
    /// The container will first search for widgets that are direct children of it, but when none of the child widgets match
    /// the given name, a recursive search will be performed.
    /// @warning This function will return nullptr when an unknown widget name was passed
    template <class WidgetType>
    TGUI_NODISCARD typename WidgetType::Ptr get(const tgui::String& widgetName) const
    {
        return std::dynamic_pointer_cast<WidgetType>(get(widgetName));
    }
    /// @brief Removes a single widget that was added to the container
    /// @param widget  Pointer to the widget to remove
    /// @return True when widget is removed, false when widget was not found
    bool remove(const tgui::Widget::Ptr& widget);
    /// @brief Removes all widgets that were added to the container
    void removeAllWidgets();

    void Draw(sf::RenderTarget* target, const Transform& parentTransform) override;

protected:

private:
    tgui::Group::Ptr m_group = nullptr;
    bool m_screenSpace = true;
    bool m_rotationLock = false;
};

#endif
