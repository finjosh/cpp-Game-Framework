#include "Graphics/DrawableManager.hpp"

std::set<DrawableObject*, _drawableComp> DrawableManager::m_drawables;

void DrawableManager::draw(sf::RenderTarget* target)
{
    // TODO test multithreading
    // NOTE - you have to draw to different render targets in different threads as a texture and then draw the textures to the window
    for (auto drawable: m_drawables)    
    {
        if (drawable->isEnabled())
            drawable->m_draw(target, {}); // since every object in this list is a parent
    }
}

void DrawableManager::addDrawable(DrawableObject* DrawableObject)
{
    m_drawables.emplace(DrawableObject);
}

void DrawableManager::removeDrawable(DrawableObject* DrawableObject)
{
    m_drawables.erase(DrawableObject);
}

size_t DrawableManager::getNumberOfObjects()
{
    return m_drawables.size();
}
