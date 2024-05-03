#include "Graphics/DrawableManager.hpp"

std::set<DrawableObject*, _drawableComp> DrawableManager::_drawables;

void DrawableManager::draw(sf::RenderWindow& window)
{
    for (auto drawable: _drawables)    
    {
        if (drawable->isEnabled())
            drawable->_draw(window);
    }
}

void DrawableManager::addDrawable(DrawableObject* DrawableObject)
{
    _drawables.insert({DrawableObject});
}

void DrawableManager::removeDrawable(DrawableObject* DrawableObject)
{
    _drawables.erase(DrawableObject);
}

size_t DrawableManager::getNumberOfObjects()
{
    return _drawables.size();
}
