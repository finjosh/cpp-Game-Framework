#include "Graphics/DrawableManager.hpp"

std::set<DrawableObject*, _drawableComp> DrawableManager::m_drawables;

void DrawableManager::draw(sf::RenderTarget* target)
{
    for (auto drawable: m_drawables)    
    {
        if (drawable->isEnabled())
            drawable->m_draw(target, drawable->getTransform()); // since every object in this list is a parent
    }
}

void DrawableManager::addDrawable(DrawableObject* DrawableObject)
{
    m_drawables.insert({DrawableObject});
}

void DrawableManager::removeDrawable(DrawableObject* DrawableObject)
{
    m_drawables.erase(DrawableObject);
}

size_t DrawableManager::getNumberOfObjects()
{
    return m_drawables.size();
}
