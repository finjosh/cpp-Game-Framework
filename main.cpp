#include <iostream>

#include "SFML/Graphics.hpp"
#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"
#include "box2d/Box2D.h"
// #include "BS_thread_pool.hpp"

#include "Utils.hpp"
#include "Utils/iniParser.hpp"

#include "ObjectManager.hpp"
#include "UpdateManager.hpp"
#include "ParticleEmitter.hpp"
#include "VectorConversions.hpp"

#include "Graphics/Renderer.hpp"
#include "Graphics/WindowHandler.hpp"
#include "Graphics/DrawableManager.hpp"
#include "Graphics/CameraManager.hpp"
#include "Graphics/CanvasManager.hpp"

#include "Physics/WorldHandler.hpp"
#include "Physics/CollisionManager.hpp"
#include "Physics/DebugDraw.hpp"

using namespace std;

// TODO make animation class

void addThemeCommands();
/// @param themes in order of most wanted
/// @param directories directories to check in ("" for current)
void tryLoadTheme(std::list<std::string> themes, std::list<std::string> directories);
std::string vecToStr(b2Vec2 vec);

class Wall : public virtual Object, public Collider, public Renderer<sf::RectangleShape>
{
public:
    inline Wall(const b2Vec2& pos, const b2Vec2& size)
    {
        setPosition(pos);

        b2PolygonShape b2shape;
        b2shape.SetAsBox(size.x/2, size.y/2);

        Collider::createFixture(b2shape, 1);
        Collider::setType(b2BodyType::b2_staticBody);

        setSize({size.x,size.y});
        setOrigin(size.x/2,size.y/2);
        setFillColor(sf::Color::Red);
    }

    createDestroy();
};

class Player : public virtual Object, public Collider, public Renderer<sf::RectangleShape>, public UpdateInterface, public Camera
{
public:
    std::string name = "Random Name";
    // sf::Texture temp;
    Object::Ptr<ParticleEmitter> m_hitParticle;
    sf::RectangleShape m_particle;

    inline Player()
    {
        b2PolygonShape b2shape;
        b2shape.SetAsBox(2.5,2.5);

        Collider::createFixture(b2shape, 1, 0.1);

        setSize({5,5});
        setOrigin(2.5,2.5);
        setFillColor(sf::Color::White);
        // temp.loadFromFile("Assets/test.png");
        // setTexture(&temp);

        m_particle.setFillColor(sf::Color::Green);
        m_particle.setSize({5,5});
        m_particle.setOrigin({2.5,2.5});
        m_hitParticle.set(new ParticleEmitter(&m_particle, {0,0}, 10, 0, 0, 1, 10, 0.5, 360));

        setMainCamera();
        Camera::setRotationLocked(true);
    }

    inline virtual void Update(float deltaTime) override
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            applyForceToCenter({0,-120000*deltaTime});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            applyForceToCenter({-120000*deltaTime,0});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            applyForceToCenter({0,120000*deltaTime});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        {
            applyForceToCenter({120000*deltaTime,0});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
        {
            applyTorque(500000*deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
        {
            applyTorque(-500000*deltaTime);
        }
    }

    void BeginContact(ContactData data) override
    {
        auto info = data.getInfo();
        if (info.getPointCount() > 0 && getLinearVelocity().LengthSquared() > 250 && data.getCollider()->cast<Wall>())
        {
            m_hitParticle->setPosition(info.getContactPoints()[0]);
            m_hitParticle->emit();
        }
    }

    createDestroy();
};

class Sensor : public virtual Object, public Renderer<sf::RectangleShape>, public Collider
{
public:
    Sensor(funcHelper::func<void> onEnter, const Object::Ptr<>& object = Object::Ptr<>(nullptr))
    {
        b2PolygonShape shape;
        shape.SetAsBox(10,10);
        Collider::createFixtureSensor(shape);
        Collider::setType(b2BodyType::b2_staticBody);

        setSize({20,20});
        setOrigin({10,10});
        setFillColor(sf::Color::Transparent);
        setOutlineColor(sf::Color::White);
        setOutlineThickness(1);

        m_onEnter = onEnter;
        m_object = object;
    }

    void BeginContact(ContactData data) override
    {
        if (!m_object || data.getCollider() == m_object.get())
        {
            m_onEnter.invoke();
        }
    }


private:
    createDestroy();
    funcHelper::func<> m_onEnter;
    Object::Ptr<> m_object;
};

class OneWay : public virtual Object, public Renderer<sf::RectangleShape>, public Collider, public UpdateInterface
{
public:
    inline OneWay(const b2Vec2& pos, const b2Vec2& size)
    {
        b2PolygonShape b2shape;
        b2shape.SetAsBox(size.x/2, size.y/2);

        Collider::createFixture(b2shape, 1);
        // Collider::setType(b2BodyType::b2_staticBody);

        setSize({size.x,size.y});
        setOrigin(size.x/2,size.y/2);
        setFillColor(sf::Color::Transparent);
        setOutlineThickness(0.5);
        setOutlineColor(sf::Color::Blue);

        auto opening = new Renderer<sf::RectangleShape>();
        opening->setParent(this);
        opening->setSize({size.x, size.y/8});
        opening->setFillColor(sf::Color::Green);
        opening->setOrigin(size.x/2, -size.y/2);

        setPosition(pos);
    }

    void Update(float delta) override
    {
        // b2Vec2 mousePos = WindowHandler::getMousePos();

        // if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        // {
        //     mousePos -= getPosition();
        //     mousePos.Normalize();
        //     mousePos *= 7500000.f*delta;
        //     applyForceToCenter(mousePos);
        // }
    }

    void PreSolve(PreSolveData data) override
    {
        if (data.getCollider()->cast<Wall>() != nullptr)
            return;
        if (getLocalVector(data.getInfo().getNormal()).y < -0.5f) // if the object is colliding from the bottom
        {
            data.setEnabled(false);
            m_freeFlow.emplace(data.getCollider());
        }
        else if (m_freeFlow.find(data.getCollider()) != m_freeFlow.end())
        {
            data.setEnabled(false);
        }
    }

    void EndContact(ContactData data) override
    {
        m_freeFlow.erase(data.getCollider());
    }

private:
    createDestroy();
    std::set<const Collider*> m_freeFlow;
};

// TODO implement contact filter callback
int main()
{
    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Game Framework", sf::Style::Fullscreen);
    // window.setFramerateLimit(60);
    WindowHandler::setRenderWindow(&window);
    CanvasManager::initGUI();

    // tgui::Gui gui{window};
    // gui.setRelativeView({0, 0, 1920/(float)window.getSize().x, 1080/(float)window.getSize().y});
    tryLoadTheme({"Dark.txt", "Black.txt"}, {"", "Assets/", "themes/", "Themes/", "assets/", "Assets/Themes/", "Assets/themes/", "assets/themes/", "assets/Themes/"});
    // -----------------------

    WorldHandler::init({0.f,0.f});
    DebugDraw debugDraw(&window);
    debugDraw.initCommands();
    WorldHandler::getWorld().SetDebugDraw(&debugDraw);

    //! Required to initialize VarDisplay and CommandPrompt
    // creates the UI for the VarDisplay
    VarDisplay::init(CanvasManager::getGui()); 
    // creates the UI for the CommandPrompt
    Command::Prompt::init(CanvasManager::getGui());
    addThemeCommands();
    // create the UI for the TFuncDisplay
    TFuncDisplay::init(CanvasManager::getGui());
    //! ---------------------------------------------------

    //* init code
    new Wall({96,108}, {192,10});
    new Wall({96,0}, {192,10});
    new Wall({192, 54}, {10, 108});
    new Wall({0, 54}, {10, 108});
    // for (int i = 0; i < 100; i++)
    //     (new Player())->setPosition({50,50});
    auto p = new Player();
    p->setPosition({15,10});
    p->setRotation(45);
    p->name = "Something";

    sf::RectangleShape particleShape;
    particleShape.setSize({10,10});
    particleShape.setOrigin({5,5});
    particleShape.setFillColor(sf::Color::Magenta);

    Object::Ptr<ParticleEmitter> emitter(new ParticleEmitter(&particleShape, {50,50}, 10, 0, 0.1, 3, 25, 1, 360));
    // emitter->setPosition({50, window.getSize().y/PIXELS_PER_METER - 10});
    emitter->setLayer(100); // since player default layer is 0
    // emitter->setDrawStage(DrawStage::Particles);
    emitter->setSpawning();
    emitter->setDrawStage(DrawStage::Particles);
    
    (new Sensor({[&emitter](){ emitter->setSpawning(!emitter->isSpawning()); }}, nullptr))->setPosition(50,50);

    new OneWay({40,25}, {40,10});

    auto camera = new Camera(10);
    camera->setScreenRect({0,0,0.25,0.25});
    camera->setPosition(window.getSize().x/PIXELS_PER_METER/2, window.getSize().y/PIXELS_PER_METER/2);
    camera->DrawBackground.connect([](sf::RenderWindow* win, sf::Vector2f size){
        sf::RectangleShape temp(size);
        temp.setFillColor(sf::Color(100,100,100,75));
        win->draw(temp);
    });

    auto gui = new Canvas();
    auto gui2 = new Canvas();
    gui2->setGlobalSpace();
    gui2->setPosition(50,50);
    gui2->add(tgui::ChildWindow::create("Test window"));
    // gui->add(tgui::ChildWindow::create("Test window"));

    float secondTimer = 0;
    int fps = 0;
    auto fpsLabel = tgui::Label::create("FPS");
    gui->getWidget()->add(fpsLabel);
    // -------------

    sf::Clock deltaClock;
    float fixedUpdate = 0;
    UpdateManager::Start();
    while (window.isOpen())
    {
        EventHelper::Event::ThreadSafe::update();
        // updating the delta time var
        sf::Time deltaTime = deltaClock.restart();
        fixedUpdate += deltaTime.asSeconds();
        secondTimer += deltaTime.asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Key::Escape)
                window.close();

            // gui.handleEvent(event);
            if (auto camera = CameraManager::getMainCamera())
                CanvasManager::handleEvent(event, {camera->getPosition().x*PIXELS_PER_METER, camera->getPosition().y*PIXELS_PER_METER});
            else
                CanvasManager::handleEvent(event, {(float)WindowHandler::getRenderWindow()->getSize().x/2, (float)WindowHandler::getRenderWindow()->getSize().y/2});

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

        ObjectManager::ClearDestroyQueue();
        WindowHandler::Display();
    }

    ObjectManager::destroyAllObjects();

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
                Command::Prompt::UpdateDefaultColor();
            }}}, 
            Command::command{"black", "Sets the them to the black theme", {[](){ 
                tgui::Theme::getDefault()->load("Assets/themes/Black.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::Prompt::UpdateDefaultColor();
            }}},
            Command::command{"grey", "Sets the them to the transparent grey theme", {[](){ 
                tgui::Theme::getDefault()->load("Assets/themes/TransparentGrey.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::Prompt::UpdateDefaultColor();
            }}}
        }
    });
}

void tryLoadTheme(std::list<std::string> themes, std::list<std::string> directories)
{
    for (auto theme: themes)
    {
        for (auto directory: directories)
        {
            if (std::filesystem::exists(directory + theme))
            {
                tgui::Theme::setDefault(directory + theme);
                Command::Prompt::UpdateDefaultColor();
                return;
            }
        }
    }
}

std::string vecToStr(b2Vec2 vec)
{
    return std::to_string(vec.x) + ", " + std::to_string(vec.x);
}
