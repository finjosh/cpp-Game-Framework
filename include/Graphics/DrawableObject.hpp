#ifndef DRAWABLE_OBJECT_HPP
#define DRAWABLE_OBJECT_HPP

#pragma once

#include "SFML/Graphics/RenderWindow.hpp"

#include "Object.hpp"
#include "Settings.hpp"

#include <set>

class DrawableManager;
class CanvasManager;
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
    /// @note it is suggested that you use the this transform that is given to reduce repeated calculations to get global position
    /// @note you need to draw in global position
    inline virtual void Draw(sf::RenderTarget* target, const Transform& thisTransform) {};
    /// @brief this is called after children draw
    /// @note it is suggested that you use the this transform that is given to reduce repeated calculations to get global position
    /// @note you need to draw in global position
    inline virtual void LateDraw(sf::RenderTarget* target, const Transform& thisTransform) {};

protected:
    /// @brief attempts to set parent given the current object parent
    /// @note only use this if you know what you are doing
    void m_setParent();
    /// @brief removes the parent only from the drawable object
    /// @note only use this if you know what you are doing
    void m_removeParent();

    /// @brief draws the objects recursively 
    /// @note only use this if you know what you are doing
    void m_draw(sf::RenderTarget* target, const Transform& stateTransform);

    friend DrawableManager;
    friend CanvasManager;

private:
    int m_layer = 0;
    DrawStage m_stage = DrawStage::Default;

    DrawableObject* m_drawableParent = nullptr;
    std::set<DrawableObject*, _drawableComp> m_drawableChildren;
};

#endif
