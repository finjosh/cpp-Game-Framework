#include <iostream>

#include "SFML/Graphics.hpp"
#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"
#include "box2d/Box2D.h"
#include "BS_thread_pool.hpp"

#include "Utils.hpp"
#include "Utils/iniParser.hpp"

#include "ObjectManager.hpp"
#include "UpdateManager.hpp"
#include "Graphics/WindowHandler.hpp"
#include "Graphics/DrawableManager.hpp"
#include "Graphics/Renderer.hpp"
#include "Physics/WorldHandler.hpp"
#include "Physics/CollisionManager.hpp"
#include "ParticleEmitter.hpp"

//! TESTING
// #include "TestHelper.hpp"
//! -------

using namespace std;

// TODO make animation class

void addThemeCommands();

class Player : public virtual Object, public Collider, public Renderer<sf::RectangleShape>, public UpdateInterface
{
public:
    std::string name = "Random Name";
    // sf::Texture temp;
    Object::Ptr<ParticleEmitter> m_hitParticle;

    inline Player()
    {
        b2PolygonShape b2shape;
        b2shape.SetAsBox(5,5);

        Collider::createFixture(b2shape, 1, 0.25);

        setSize({10,10});
        setOrigin(5,5);
        setFillColor(sf::Color::White);
        // temp.loadFromFile("Assets/test.png");
        // setTexture(&temp);

        m_hitParticle.set(new ParticleEmitter(static_cast<RectangleShape*>(this), {}, 10, 0, 0, 1, 10, 0.5, 360));
    }

    inline virtual void Update(const float& deltaTime) override
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            ApplyForceToCenter({0,-250000*deltaTime});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            ApplyForceToCenter({-250000*deltaTime,0});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            ApplyForceToCenter({0,250000*deltaTime});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        {
            ApplyForceToCenter({250000*deltaTime,0});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
        {
            ApplyTorque(750000*deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
        {
            ApplyTorque(-750000*deltaTime);
        }
        setAwake(true);
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
        Collider::SetType(b2BodyType::b2_staticBody);

        setSize({size.x,size.y});
        setOrigin(size.x/2,size.y/2);
        setFillColor(sf::Color::Red);
    }

    // inline void BeginContact(ContactData data) override
    // {
    //     if (Object::Ptr<Player> player = data.getCollider()->cast<Player>())
    //     {
    //         Command::Prompt::print("Begin Contact with player: " + player->name);
    //         data.getCollider()->destroy();
    //     }
    // }

    // inline void EndContact(ContactData data) override
    // {
    //     if (Object::Ptr<Player> player = data.getCollider()->cast<Player>())
    //     {
    //         Command::Prompt::print("End contact with player: " + player->name);
    //     }
    // }

    createDestroy();
};

class Sensor : public virtual Object, public Renderer<sf::RectangleShape>, public Collider
{
public:
    Sensor()
    {
        b2PolygonShape shape;
        shape.SetAsBox(10,10);
        Collider::createFixtureSensor(shape);

        setSize({20,20});
        setOrigin({10,10});
        setFillColor(sf::Color::Transparent);
        setOutlineColor(sf::Color::White);
    }

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

    WorldHandler::init({0.f,0.f});

    //! Required to initialize VarDisplay and CommandPrompt
    // creates the UI for the VarDisplay
    VarDisplay::init(gui); 
    // creates the UI for the CommandPrompt
    Command::Prompt::init(gui);
    addThemeCommands();
    // create the UI for the TFuncDisplay
    TFuncDisplay::init(gui);
    //! ---------------------------------------------------

    new Wall({window.getSize().x/2/PIXELS_PER_METER,window.getSize().y/PIXELS_PER_METER}, {window.getSize().x/PIXELS_PER_METER,10});
    new Wall({window.getSize().x/2/PIXELS_PER_METER,0}, {window.getSize().x/PIXELS_PER_METER,10});
    new Wall({window.getSize().x/PIXELS_PER_METER, window.getSize().y/2/PIXELS_PER_METER}, {10, window.getSize().y/PIXELS_PER_METER});
    new Wall({0, window.getSize().y/2/PIXELS_PER_METER}, {10, window.getSize().y/PIXELS_PER_METER});
    /// @brief create a body in the world with the default body def parameters
    // (new Player())->setPosition({25,10});
    auto p = new Player();
    p->setPosition({15,10});
    p->setRotation(45);
    p->name = "Something";

    sf::RectangleShape particleShape;
    particleShape.setSize({10,10});
    particleShape.setOrigin({5,5});
    particleShape.setFillColor(sf::Color::Magenta);

    Object::Ptr<ParticleEmitter> emitter(new ParticleEmitter(&particleShape, {10,10}, 10, 0, 0.1, 3, 25, 1, 360));
    // emitter->setPosition({50, window.getSize().y/PIXELS_PER_METER - 10});
    emitter->setLayer(100); // since player default layer is 0
    // emitter->setDrawStage(DrawStage::Particles);
    emitter->setSpawning();
    
    p->addChild(emitter.getObj());
    emitter->setDrawStage(DrawStage::Particles);

    (new Sensor())->setPosition(50,50);

    float secondTimer = 0;
    int fps = 0;
    auto fpsLabel = tgui::Label::create("FPS");
    gui.add(fpsLabel);

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
        secondTimer += deltaTime.asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Key::Escape)
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

        fps++;
        if (secondTimer >= 1)
        {
            fpsLabel->setText("FPS: " + std::to_string(fps));
            secondTimer = 0;
            fps = 0;
        }

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
