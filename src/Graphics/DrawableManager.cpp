#include "Graphics/DrawableManager.hpp"

std::set<DrawableObject*, _drawableComp> DrawableManager::m_drawables;

void DrawableManager::draw(sf::RenderWindow& window)
{
    for (auto drawable: m_drawables)    
    {
        if (drawable->isEnabled())
            drawable->m_draw(window);
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
