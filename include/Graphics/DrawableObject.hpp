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
    using Ptr = Object::Ptr<DrawableObject>;

    DrawableObject(int layer = 0, DrawStage stage = DrawStage::Default);
    ~DrawableObject();

    /// @brief changes the order in which this obj will be drawn
    /// @note layer is relative to the parents layer if it is a drawableObject
    /// @param layer smaller is earlier and larger is later
    virtual void setLayer(int layer);
    /// @note the layer is relative to the parent layer if it is a drawableObject
    virtual int getLayer() const;
    /// @brief sets the stage at which this object will be drawn
    /// @note stage is the same as the drawable parent unless manually set (if there is any parent that is drawable)
    virtual void setDrawStage(DrawStage stage);
    /// @note stage is relative to the parent unless manually set
    virtual DrawStage getDrawStage() const;

    /// @brief should be used to draw the obj
    /// @note called before children draw
    inline virtual void Draw(sf::RenderTarget* target) {}; // Take in parent transform so that it is easier to interpolate
    /// @brief this is called after children draw
    inline virtual void LateDraw(sf::RenderTarget* target) {};

protected:
    /// @brief attempts to set parent given the current object parent
    /// @note only use this if you know what you are doing
    void m_setParent();
    /// @brief removes the parent only from the drawable object
    /// @note only use this if you know what you are doing
    void m_removeParent();

    /// @brief draws the objects recursively 
    /// @note only use this if you know what you are doing
    void m_draw(sf::RenderTarget* target);

    friend DrawableManager;

private:
    int m_layer = 0;
    DrawStage m_stage = DrawStage::Default;

    DrawableObject* m_drawableParent = nullptr;
    std::set<DrawableObject*, _drawableComp> m_drawableChildren;
};

#endif
