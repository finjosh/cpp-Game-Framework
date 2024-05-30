#ifndef DRAWABLE_H
#define DRAWABLE_H

#pragma once

#include "SFML/Graphics/RenderWindow.hpp"

#include "Object.hpp"
#include "Settings.hpp"

#include <set>

class DrawableManager;
class DrawableObject;

struct _drawableComp
{
    bool operator() (const DrawableObject* lhs, const DrawableObject* rhs) const;
};

class DrawableObject : public virtual Object
{
public:
    DrawableObject(const int& layer = 0);
    ~DrawableObject();

    /// @brief changes the order in which this obj will be drawn
    /// @note layer is relative to the parents layer if it is a drawableObject
    /// @param layer smaller is earlier and larger is later
    void setLayer(const int& layer);
    /// @note the layer is relative to the parent layer if it is a drawableObject
    int getLayer() const;
    /// @brief sets the stage at which this object will be drawn
    /// @note stage is the same as the drawable parent unless manually set (if there is any parent that is drawable)
    void setDrawStage(const DrawStage& stage); // TODO implement for children
    /// @note stage is relative to the parent unless manually set
    DrawStage getDrawStage() const;

    /// @brief should be used to draw the obj
    /// @note called before children draw
    inline virtual void Draw(sf::RenderWindow& window) {};
    /// @brief this is called after children draw
    inline virtual void LateDraw(sf::RenderWindow& window) {};

protected:
    /// @brief attempts to set parent given the current object parent
    /// @note only use this if you know what you are doing
    void _setParent();
    /// @brief removes the parent only from the drawable object
    /// @note only use this if you know what you are doing
    void _removeParent();

    /// @brief draws the objects recursively 
    /// @note only use this if you know what you are doing
    void _draw(sf::RenderWindow& window);

    friend DrawableManager;

private:
    int m_layer = 0;
    DrawStage m_stage = DrawStage::Default;

    DrawableObject* m_drawableParent;
    std::set<DrawableObject*, _drawableComp> m_drawableChildren;
};

#endif
