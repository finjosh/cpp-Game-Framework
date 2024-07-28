#include "Graphics/DrawableManager.hpp"
// #include "SFML/Graphics/RenderTexture.hpp"
// #include "ThreadPool.hpp"
// #include "SFML/Graphics/RectangleShape.hpp"

std::set<DrawableObject*, _drawableComp> DrawableManager::m_drawables;

// std::set<DrawableObject*, _drawableComp>::iterator getIterator(std::set<DrawableObject*, _drawableComp>::iterator start, uint64_t n)
// {
//     for (uint64_t i = 0; i < n; i++)
//     {
//         start++;
//     }
//     return start;
// }

void DrawableManager::draw(sf::RenderTarget* target, sf::ContextSettings contextSettings)
{
    // std::list<std::future<void>> threads;
    // std::list<sf::RenderTexture> renderTargets;
    // TODO test multithreading
    // NOTE - you have to draw to different render targets in different threads as a texture and then draw the textures to the window
    for (auto drawable: m_drawables)    
    {
        if (drawable->isEnabled())
            drawable->m_draw(target, {}); // since every object in this list is a parent
    }
    // sf::RenderTexture renderTexture;
    // renderTexture.create(target->getSize().x, target->getSize().y, contextSettings);

    // uint64_t partitionSize = m_drawables.size()/ThreadPool::get().get_thread_count();
    // uint64_t extra = m_drawables.size()%ThreadPool::get().get_thread_count();
    // uint64_t temp = 0;
    // std::set<DrawableObject*, _drawableComp>::iterator current = m_drawables.begin();
    // std::set<DrawableObject*, _drawableComp>::iterator next; // also the current end
    // for (int i = 0; i < ThreadPool::get().get_thread_count(); i++)
    // {
    //     temp = (extra > 0 ? 1 : 0);
    //     next = getIterator(current, partitionSize + temp);
    //     renderTargets.emplace_back();
    //     renderTargets.back().create(target->getSize().x, target->getSize().y, contextSettings);
    //     renderTargets.back().setActive(false);
    //     threads.emplace_back(ThreadPool::get().submit_task([current, next, &renderTargets]{ 
    //         m_draw(current, next, &renderTargets.back());
    //     }));
    //     current = next;
    //     extra -= temp;
    // }

    // for (auto& thread: threads)
    // {
    //     thread.wait();
    // }

    // threads.clear();

    // for (auto& renderTexture: renderTargets)
    // {
    //     renderTexture.setActive(true);
    //     renderTexture.display();
    //     sf::RectangleShape temp;
    //     temp.setTexture(&renderTexture.getTexture(), true);
    //     target->draw(temp);
    // }
}

// void DrawableManager::m_draw(std::set<DrawableObject*, _drawableComp>::iterator start, std::set<DrawableObject*, _drawableComp>::iterator end, sf::RenderTarget* renderTarget)
// {
//     renderTarget->setActive(true);
//     while (start != end)
//     {
//         if ((*start)->isEnabled())
//             (*start)->m_draw(renderTarget, {}); // since every object in this list is a parent
//         start++;
//     }
//     renderTarget->setActive(false);
// }

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
