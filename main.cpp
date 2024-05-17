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
#include "TestHelper.hpp"
//! -------

using namespace std;
using namespace sf;

void addThemeCommands();

class Wall : public virtual Object, public Collider, public DrawableObject, public sf::RectangleShape
{
public:
    inline Wall(const b2Vec2& pos, const b2Vec2& size)
    {
        b2PolygonShape b2shape;
        b2shape.SetAsBox(size.x/2, size.y/2);

        Collider::initCollider(pos.x,pos.y);
        Collider::createFixture(b2shape, 1, 0.25);
        Collider::getBody()->SetType(b2BodyType::b2_staticBody);

        RectangleShape::setSize({size.x*PIXELS_PER_METER,size.y*PIXELS_PER_METER});
        RectangleShape::setOrigin(size.x/2*PIXELS_PER_METER,size.y/2*PIXELS_PER_METER);
        RectangleShape::setFillColor(sf::Color::Red);
    }

    inline virtual void Draw(sf::RenderWindow& window) override
    {
        RectangleShape::setPosition(Object::getPosition().x*PIXELS_PER_METER, Object::getPosition().y*PIXELS_PER_METER);
        RectangleShape::setRotation(Object::getRotation()*180/PI);
        window.draw(*this);
    }

    createDestroy();
};

class Player : public virtual Object, public Collider, public DrawableObject, public UpdateInterface, public sf::RectangleShape
{
public:
    inline Player()
    {
        b2PolygonShape b2shape;
        b2shape.SetAsBox(5,5);

        Collider::initCollider(0,0);
        Collider::createFixture(b2shape, 1, 0.25);
        Collider::getBody()->SetType(b2BodyType::b2_dynamicBody);

        RectangleShape::setSize({10*PIXELS_PER_METER,10*PIXELS_PER_METER});
        RectangleShape::setOrigin(5*PIXELS_PER_METER,5*PIXELS_PER_METER);
        RectangleShape::setFillColor(sf::Color::White);
    }

    inline virtual void Update(const float& deltaTime) override
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            Collider::move({0,-10*deltaTime});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            Collider::move({-10*deltaTime,0});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            Collider::move({0,10*deltaTime});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        {
            Collider::move({10*deltaTime,0});
        }
    }

    inline virtual void OnColliding(CollisionData data) override
    {
        Command::Prompt::print("Colliding");
    }

    inline virtual void Draw(sf::RenderWindow& window) override
    {
        RectangleShape::setPosition(Object::getPosition().x*PIXELS_PER_METER, Object::getPosition().y*PIXELS_PER_METER);
        RectangleShape::setRotation(Object::getRotation()*180/PI);
        window.draw(*this);
    }

    createDestroy();
};

class EmptyUpdateObject : public virtual Object, public UpdateInterface 
{
    createDestroy();
};

int main()
{
    // BS::thread_pool pool;

    // TestHelper::initTest("Normal Enable Check (all enabled)", "# of Objects", {[&pool](){ UpdateManager::Update(0,pool); }}, {[](){ new EmptyUpdateObject(); }}, 100001, 0);
    // // , {[](){ ObjectManager::destroyAllObjects(); }}, 10
    // TestHelper::runTest(TestHelper::FileExists::DoNothing); 
    
    // TODO the ability to zoom into a specific section of the data (from x0 to x1 OR from y0 to y1)
    // TestHelper::graphData();

    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Game Framework", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
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

    new Wall({0,0}, {100,10});
    new Player();

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
            Command::command{"default", "(Currently does not work, coming soon) Sets the theme back to default", {[](){ 
                tgui::Theme::setDefault(""); //! This does not work due to a tgui issue
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({0,0,0,255}); // black
            }}},
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
