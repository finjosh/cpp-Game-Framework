#include "Engine.hpp"

#include "Utils/CommandPrompt.hpp"
#include "Utils/Debug/VarDisplay.hpp"
#include "Utils/Debug/TFuncDisplay.hpp"

#include "Graphics/WindowHandler.hpp"
#include "Graphics/CanvasManager.hpp"

#include "Physics/WorldHandler.hpp"
#include "Physics/DebugDraw.hpp"
#include "Physics/CollisionManager.hpp"

#include "ObjectManager.hpp"
#include "UpdateManager.hpp"
#include "Input.hpp"

Engine& Engine::get()
{
    static Engine instance;
    return instance;
}

void Engine::init(sf::VideoMode mode, const sf::String &title, uint32_t style, sf::State state, const sf::ContextSettings &settings)
{
    WindowHandler::initRenderWindowSettings(sf::VideoMode::getDesktopMode(), "Game Framework", sf::Style::Default, sf::State::Fullscreen);
    WindowHandler::createRenderWindow();
    CanvasManager::initGUI();

    tryLoadTheme({"Dark.txt", "Black.txt"}, {"", "Assets/", "themes/", "Themes/", "assets/", "Assets/Themes/", "Assets/themes/", "assets/themes/", "assets/Themes/"});
    // -----------------------

    WorldHandler::get().init({0.f,0.f}); // TODO implement ray cast system
    DebugDraw::get().initCommands();
    // DebugDraw::get().drawAll(true);
    Input::get(); // initializing the input dictionary for string conversions

    Canvas* main_gui = new Canvas();

    //! Required to initialize VarDisplay and CommandPrompt
    // creates the UI for the VarDisplay
    VarDisplay::init(main_gui->getGroup()); 
    // creates the UI for the CommandPrompt
    Command::Prompt::init(main_gui->getGroup());
    // create the UI for the TFuncDisplay
    TFuncDisplay::init(main_gui->getGroup());
    //! ---------------------------------------------------
}

void Engine::preLoop()
{
    m_deltaClock.start();
    m_fixedUpdate = 0;
    UpdateManager::Start();
}

void Engine::preEventHandling()
{
    EventHelper::Event::ThreadSafe::update();
    // updating the delta time var
    sf::Time deltaTime = m_deltaClock.restart();
    m_deltaTime = deltaTime.asSeconds();
    m_fixedUpdate += m_deltaTime;

    Input::get().UpdateJustStates();
}

bool Engine::handleEvent(const sf::Event& event, bool handled)
{
    if (event.is<sf::Event::Closed>())
        WindowHandler::getRenderWindow()->close();

    bool wasHandled = false;
    if (!handled)
    {
        if (Command::Prompt::UpdateEvent(event))
            wasHandled = true;
        else if (CanvasManager::handleEvent(event))
            wasHandled = true;
        else
            LiveVar::UpdateLiveVars(event);
    }

    Input::get().HandelEvent(event, wasHandled);
    return wasHandled;
}

void Engine::preUserCode()
{
    UpdateManager::Update(m_deltaTime);
    if (m_fixedUpdate >= 0.2)
    {
        UpdateManager::FixedUpdate();
        m_fixedUpdate = 0;
    }
    //! Updates all the vars being displayed
    VarDisplay::Update();
    //! ------------------------------=-----
    //! Updates all Terminating Functions
    TerminatingFunction::UpdateFunctions(m_deltaTime);
    //* Updates for the terminating functions display
    TFuncDisplay::Update(); // updates the terminating functions display
    //! ------------------------------

    //! Do physics before this for consistent physics (in object update)
    WorldHandler::get().updateWorld(m_deltaTime); // updates the world physics
    CollisionManager::get()->Update(); // updates the collision callbacks
    //! Draw after this
    UpdateManager::LateUpdate(m_deltaTime);
}

void Engine::postUserCode()
{
    ObjectManager::ClearDestroyQueue();
    WindowHandler::Display();
}

void Engine::close()
{
    ObjectManager::destroyAllObjects();
    CanvasManager::closeGUI();
    WindowHandler::getRenderWindow()->close();
}

float Engine::getDeltaTime() const
{
    return m_deltaTime;
}

void Engine::tryLoadTheme(std::list<std::string> themes, std::list<std::string> directories)
{
    for (auto theme: themes)
    {
        for (auto directory: directories)
        {
            if (std::filesystem::exists(directory + theme))
            {
                tgui::Theme::setDefault(directory + theme);
                return;
            }
        }
    }
}
