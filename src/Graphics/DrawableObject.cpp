#include "Graphics/DrawableObject.hpp"
#include "Graphics/DrawableManager.hpp"

bool _drawableComp::operator() (const DrawableObject* lhs, const DrawableObject* rhs) const
{
    if (lhs->getDrawStage() < rhs->getDrawStage())
        return true;
    else if (lhs->getDrawStage() > rhs->getDrawStage())
        return false;
    else if (lhs->getLayer() <= rhs->getLayer() && lhs->getID() < rhs->getID())
        return true;
    else 
        return false;

//     if (lhs->getDrawStage() < rhs->getDrawStage() || (lhs->getDrawStage() == rhs->getDrawStage() && lhs->getLayer() <= rhs->getLayer()) && lhs->getID() < rhs->getID())
//         return true;
//     else    
//         return false;
}

DrawableObject::DrawableObject(int layer, DrawStage stage) : m_layer(layer), m_stage(stage)
{
    m_onParentRemoved(&DrawableObject::m_removeParent, this);
    m_onParentSet(&DrawableObject::m_setParent, this);

    DrawableManager::addDrawable(this);
}

DrawableObject::~DrawableObject()
{
    DrawableManager::removeDrawable(this);
}

void DrawableObject::setLayer(int layer)
{
    DrawableManager::removeDrawable(this);
    m_layer = layer;
    DrawableManager::addDrawable(this);
}

int DrawableObject::getLayer() const
{
    return m_layer;
}

void DrawableObject::setDrawStage(DrawStage stage)
{
    DrawableManager::removeDrawable(this);
    m_stage = stage;
    if (m_drawableParent)
        m_removeParent(); // also adds back to the drawable manager
    else
        DrawableManager::addDrawable(this);
}

DrawStage DrawableObject::getDrawStage() const
{
    if (m_drawableParent)
        return m_drawableParent->getDrawStage();
    else
        return m_stage;
}

void DrawableObject::m_setParent()
{
    auto curParent = this->getParent();

    if (curParent == nullptr)
    {
        m_removeParent();
        return;
    }

    DrawableObject* drawableParent = curParent->cast<DrawableObject>();

    while (drawableParent == nullptr && curParent != nullptr)
    {
        curParent = curParent->getParent();
        drawableParent = curParent->cast<DrawableObject>();
    }

    if (drawableParent != nullptr)
    {
        DrawableManager::removeDrawable(this);
        drawableParent->m_drawableChildren.insert({this});
        m_drawableParent = drawableParent;
    }
    else
    {
        m_removeParent();
    }
}

void DrawableObject::m_removeParent()
{
    if (m_drawableParent != nullptr)
    {
        m_drawableParent->m_drawableChildren.erase(this);
        m_drawableParent = nullptr;
    }
    DrawableManager::addDrawable(this);
}

void DrawableObject::m_draw(sf::RenderTarget* target, const Transform& stateTransform) 
{
    this->Draw(target, stateTransform);
    for (auto child: m_drawableChildren)
    {
        Transform childTransform(stateTransform);
        childTransform.position += Vector2::rotateAround(child->getInterpolatedPosition(), {0,0}, stateTransform.rotation);
        childTransform.rotation += child->getInterpolatedRotation();
        child->m_draw(target, childTransform);
    }
    this->LateDraw(target, stateTransform);
}
