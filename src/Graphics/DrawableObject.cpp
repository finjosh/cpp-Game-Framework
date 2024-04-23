#include "Graphics/DrawableObject.hpp"
#include "Graphics/DrawableManager.hpp"

bool _drawableComp::operator() (const DrawableObject* lhs, const DrawableObject* rhs) const
{
    if (lhs->getLayer() < rhs->getLayer() || lhs->getLayer() == rhs->getLayer() && lhs->getID() < rhs->getID())
        return true;
    else    
        return false;
}

DrawableObject::DrawableObject(const int& layer) : _layer(layer)
{
    _onParentRemoved(&DrawableObject::_removeParent, this);
    _onParentSet(&DrawableObject::_setParent, this);

    DrawableManager::addDrawable(this);
}

DrawableObject::~DrawableObject()
{
    DrawableManager::removeDrawable(this);
}

void DrawableObject::setLayer(const int& layer)
{
    DrawableManager::removeDrawable(this);
    _layer = layer;
    DrawableManager::addDrawable(this);
}

int DrawableObject::getLayer() const
{
    return _layer;
}

void DrawableObject::_setParent()
{
    auto curParent = this->getParent();
    DrawableObject* drawableParent = curParent->cast<DrawableObject>();
    while (drawableParent == nullptr && curParent != nullptr)
    {
        curParent = curParent->getParent();
        drawableParent = curParent->cast<DrawableObject>();
    }

    if (drawableParent != nullptr)
    {
        DrawableManager::removeDrawable(this);
        drawableParent->_drawableChildren.insert({this});
        this->_drawableParent = drawableParent;
    }
}

void DrawableObject::_removeParent()
{
    DrawableObject::setLayer(0);
    DrawableManager::addDrawable(this);
    _drawableParent->_drawableChildren.erase(this);
    _drawableParent = nullptr;
}

void DrawableObject::_draw(sf::RenderWindow& window)
{
    this->Draw(window);
    for (auto child: _drawableChildren)
    {
        child->_draw(window);
    }
    this->LateDraw(window);
}
