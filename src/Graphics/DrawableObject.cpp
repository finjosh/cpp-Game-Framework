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
    if (m_drawableParent)
    {
        auto parent = m_drawableParent;
        unsigned int nonDraw = m_nonDrawableParents;
        m_removeParent();
        m_layer = layer;
        m_drawableParent = parent;
        m_nonDrawableParents = nonDraw;
        m_drawableParent->m_drawableChildren.emplace(this);
    }
    else
    {
        DrawableManager::removeDrawable(this);
        m_layer = layer;
        DrawableManager::addDrawable(this);
    }
}

int DrawableObject::getLayer() const
{
    return m_layer;
}

void DrawableObject::setDrawStage(DrawStage stage)
{
    if (m_drawableParent)
    {
        auto parent = m_drawableParent;
        unsigned int nonDraw = m_nonDrawableParents;
        m_removeParent();
        m_stage = stage;
        m_drawableParent = parent;
        m_nonDrawableParents = nonDraw;
        m_drawableParent->m_drawableChildren.emplace(this);
    }
    else
    {
        DrawableManager::removeDrawable(this);
        m_stage = stage;
        DrawableManager::addDrawable(this);
    }
}

DrawStage DrawableObject::getDrawStage() const
{
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
    unsigned int nonDrawables = 0;

    while (drawableParent == nullptr && curParent != nullptr)
    {
        nonDrawables++;
        curParent = curParent->getParentRaw();
        drawableParent = curParent->cast<DrawableObject>();
    }

    if (drawableParent != nullptr)
    {
        DrawableManager::removeDrawable(this);
        drawableParent->m_drawableChildren.emplace(this);
        m_drawableParent = drawableParent;
    }
    else
    {
        m_removeParent();
    }
    m_nonDrawableParents = nonDrawables;
}

void DrawableObject::m_removeParent()
{
    if (m_drawableParent != nullptr)
    {
        m_drawableParent->m_drawableChildren.erase(this);
        m_drawableParent = nullptr;
    }
    m_nonDrawableParents = 0;
    DrawableManager::addDrawable(this);
}

// TODO fix this as it does not handle the following case
// Drawable Parent -> non drawable child -> drawable child
void DrawableObject::m_draw(sf::RenderTarget* target, Transform stateTransform) 
{
    if (m_nonDrawableParents > 0)
        stateTransform += m_getNInterpolatedTransforms(getParentRaw(), m_nonDrawableParents);
    stateTransform += getInterpolatedTransform();
    this->Draw(target, stateTransform);
    for (auto child: m_drawableChildren)
    {
        child->m_draw(target, stateTransform);
    }
    this->LateDraw(target, stateTransform);
}

Transform DrawableObject::m_getNInterpolatedTransforms(Object* parent, unsigned int n)
{
    if (n == 0)
        return Transform{};
    return m_getNInterpolatedTransforms(parent->getParentRaw(), n-1) + parent->getInterpolatedTransform();
}
