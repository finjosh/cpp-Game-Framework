#include <iostream>

#include "SFML/Graphics.hpp"
#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"
#include "box2d/Box2D.h"
#include "BS_thread_pool.hpp"

#include "Utils.hpp"
#include "Graphics/WindowHandler.hpp"
#include "Physics/WorldHandler.hpp"
#include "Utils/iniParser.hpp"

#include "UpdateManager.hpp"
#include "Graphics/DrawableManager.hpp"
#include "ObjectManager.hpp"
#include "Physics/CollisionManager.hpp"

//! TESTING
// #include "TestHelper.hpp"
//! -------

#include "Graphics/Renderer.hpp"

using namespace std;

void addThemeCommands();

class Player : public virtual Object, public Collider, public Renderer<sf::RectangleShape>, public UpdateInterface
{
public:
    std::string name = "Random Name";

    inline Player()
    {
        b2PolygonShape b2shape;
        b2shape.SetAsBox(5,5);

        Collider::createFixture(b2shape, 1, 0.25);

        setSize({10*PIXELS_PER_METER,10*PIXELS_PER_METER});
        setOrigin(5*PIXELS_PER_METER,5*PIXELS_PER_METER);
        setFillColor(sf::Color::White);
    }

    inline virtual void Update(const float& deltaTime) override
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            move({0,-25*deltaTime});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            move({-25*deltaTime,0});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            move({0,25*deltaTime});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        {
            move({25*deltaTime,0});
        }
        Collider::setAwake(true);
    }

    inline virtual void OnColliding(CollisionData data) override
    {
        Command::Prompt::print("Colliding: " + std::to_string(Object::getPosition().x) + ", " + std::to_string(Object::getPosition().y));
    }

    createDestroy();
};

class Wall : public virtual Object, public Collider, public Renderer<sf::RectangleShape>
{
public:
    inline Wall(const b2Vec2& pos, const b2Vec2& size)
    {
        setPosition(pos);

        b2PolygonShape b2shape;
        b2shape.SetAsBox(size.x/2, size.y/2);

        Collider::createFixture(b2shape, 1, 0.25);
        Collider::getBody()->SetType(b2BodyType::b2_staticBody);

        setSize({size.x*PIXELS_PER_METER,size.y*PIXELS_PER_METER});
        setOrigin(size.x/2*PIXELS_PER_METER,size.y/2*PIXELS_PER_METER);
        setFillColor(sf::Color::Red);
    }

    // inline void BeginContact(CollisionData data) override
    // {
    //     if (Object::Ptr<Player> player = data.getCollider()->cast<Player>())
    //     {
    //         Command::Prompt::print("Begin Contact with player: " + player->name);
    //         data.getCollider()->destroy();
    //     }
    // }

    // inline void EndContact(CollisionData data) override
    // {
    //     if (Object::Ptr<Player> player = data.getCollider()->cast<Player>())
    //     {
    //         Command::Prompt::print("End contact with player: " + player->name);
    //     }
    // }

    createDestroy();
};

class EmptyUpdateObject : public virtual Object, public UpdateInterface 
{
    createDestroy();
};

int main()
{
    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Game Framework", sf::Style::Fullscreen);
    // window.setFramerateLimit(60);
    WindowHandler::setRenderWindow(&window);

    tgui::Gui gui{window};
    gui.setRelativeView({0, 0, 1920/(float)window.getSize().x, 1080/(float)window.getSize().y});
    tgui::Theme::setDefault("Assets/themes/Dark.txt");
    Command::color::setDefaultColor({255,255,255,255});
    // -----------------------

    WorldHandler::getWorld().SetGravity({0.f,0.f});
    WorldHandler::getWorld().SetContactListener(new CollisionManager); // TODO put this stuff into the constructor

    //! Required to initialize VarDisplay and CommandPrompt
    // creates the UI for the VarDisplay
    VarDisplay::init(gui); 
    // creates the UI for the CommandPrompt
    Command::Prompt::init(gui);
    addThemeCommands();
    // create the UI for the TFuncDisplay
    TFuncDisplay::init(gui);
    //! ---------------------------------------------------

    new Wall({25,25}, {100,10});
    /// @brief create a body in the world with the default body def parameters
    new Player();
    auto p = new Player();
    p->setPosition({15,0});
    p->setRotation(45);
    p->name = "Something";

    sf::Clock deltaClock;
    float fixedUpdate = 0;
    UpdateManager::Start();
    while (window.isOpen())
    {
        EventHelper::Event::ThreadSafe::update();
        window.clear();
        // updating the delta time var
        sf::Time deltaTime = deltaClock.restart();
        fixedUpdate += deltaTime.asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            gui.handleEvent(event);

            //! Required for LiveVar and CommandPrompt to work as intended
            LiveVar::UpdateLiveVars(event);
            Command::Prompt::UpdateEvent(event);
            //! ----------------------------------------------------------
        }

        UpdateManager::Update(deltaTime.asSeconds());
        if (fixedUpdate >= 0.2)
        {
            UpdateManager::FixedUpdate();
            fixedUpdate = 0;
        }
        UpdateManager::LateUpdate(deltaTime.asSeconds());
        //! Updates all the vars being displayed
        VarDisplay::Update();
        //! ------------------------------=-----
        //! Updates all Terminating Functions
        TerminatingFunction::UpdateFunctions(deltaTime.asSeconds());
        //* Updates for the terminating functions display
        TFuncDisplay::update(); // updates the terminating functions display
        //! ------------------------------

        //! Do physics before this for consistent physics (in object update)
        WorldHandler::updateWorld(deltaTime.asSeconds()); // updates the world physics
        CollisionManager::Update(); // updates the collision callbacks
        //! Draw after this

        //* Write code here

        // ---------------

        DrawableManager::draw(window);

        ObjectManager::ClearDestroyQueue();

        // draw for tgui
        gui.draw();
        // display for sfml window
        window.display();
    }

    //! Required so that VarDisplay and CommandPrompt release all data
    VarDisplay::close();
    Command::Prompt::close();
    TFuncDisplay::close();
    //! --------------------------------------------------------------

    window.close();

    return 0;
}

void addThemeCommands()
{
    Command::Handler::addCommand(Command::command{"setTheme", "Function used to set the theme of the UI (The previous outputs in the command prompt will not get updated color)", 
        {Command::print, "Trying calling one of the sub commands"},
        std::list<Command::command>{
            // Command::command{"default", "(Currently does not work, coming soon) Sets the theme back to default", {[](){ 
            //     tgui::Theme::setDefault(""); //! This does not work due to a tgui issue
            //     // Note that command color does not update with theme so you have to set the default color
            //     Command::color::setDefaultColor({0,0,0,255}); // black
            // }}},
            // Dark theme is a custom theme made by me 
            // It can be found here: https://github.com/finjosh/TGUI-DarkTheme
            Command::command{"dark", "Sets the them to the dark theme", {[](){ 
                tgui::Theme::getDefault()->load("Assets/themes/Dark.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({255,255,255,255}); // white
            }}}, 
            Command::command{"black", "Sets the them to the black theme", {[](){ 
                tgui::Theme::getDefault()->load("Assets/themes/Black.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({255,255,255,255}); // white
            }}},
            Command::command{"grey", "Sets the them to the transparent grey theme", {[](){ 
                tgui::Theme::getDefault()->load("Assets/themes/TransparentGrey.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({0,0,0,255}); // black
            }}}
        }
    });
}
