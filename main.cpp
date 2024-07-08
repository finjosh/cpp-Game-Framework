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

#include "Graphics/Renderer.hpp"
#include "Graphics/WindowHandler.hpp"
#include "Graphics/DrawableManager.hpp"
#include "Graphics/CameraManager.hpp"
#include "Graphics/CanvasManager.hpp"

#include "Physics/WorldHandler.hpp"
#include "Physics/CollisionManager.hpp"
#include "Physics/DebugDraw.hpp"

#include "Networking/NetworkTypes.hpp"
#include "Networking/SocketUI.hpp"
#include "Networking/NetworkObject.hpp"
#include "Networking/NetworkObjectManager.hpp"

using namespace std;

// TODO make animation class
// TODO implement fixture and joint isValid functions that connect the the collider with the body that relates to them

void addThemeCommands();
/// @param themes in order of most wanted
/// @param directories directories to check in ("" for current)
void tryLoadTheme(std::list<std::string> themes, std::list<std::string> directories);

class Wall : public virtual Object, public Collider, public Renderer<sf::RectangleShape>
{
public:
    inline Wall(const Vector2& pos, const Vector2& size)
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

class Player : public virtual Object, public Collider, public Renderer<sf::RectangleShape>, public UpdateInterface
{
public:
    std::string name = "Random Name";
    // sf::Texture temp;
    Object::Ptr<ParticleEmitter> m_hitParticle;
    static sf::RectangleShape m_particle;
    static Camera::Ptr m_camera; // default is nullptr
    static std::set<Object*> m_players;

    inline Player()
    {
        m_players.emplace(this);

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

        if (!m_camera)
        {
            m_camera = new Camera();
            // m_camera->setParent(this);
            m_camera->setMainCamera();
            // m_camera->setRotationLocked(true);
        }
    }

    inline ~Player()
    {
        m_players.erase(this);
    }

    inline virtual void Update(float deltaTime) override
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            // applyForceToCenter({0,-120000*deltaTime});
            move({0,-12*deltaTime});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            // applyForceToCenter({-120000*deltaTime,0});
            move({-12*deltaTime,0});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            // applyForceToCenter({0,120000*deltaTime});
            move({0,12*deltaTime});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        {
            // applyForceToCenter({120000*deltaTime,0});
            move({12*deltaTime,0});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
        {
            // applyTorque(500000*deltaTime);
            rotate(PI * deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
        {
            // applyTorque(-500000*deltaTime);
            rotate(-PI * deltaTime);
        }
    }

    inline void LateUpdate(float delta) override
    {
        Vector2 pos(0,0);

        for (auto player: m_players)
        {
            pos += player->getPosition();
        }
        pos /= m_players.size();

        m_camera->setPosition(Vector2::lerp(m_camera->getPosition(), pos, 0.97*delta)); // since late update is called after physics update
        m_camera->setRotation(getRotation());
    }

    void BeginContact(ContactData data) override
    {
        auto info = data.getInfo();
        if (info.getPointCount() > 0 && getLinearVelocity().lengthSquared() > 250 && data.getCollider()->cast<Wall>())
        {
            m_hitParticle->setPosition(info.getContactPoint(0));
            m_hitParticle->emit();
        }
    }

    createDestroy();
};

Camera::Ptr Player::m_camera = nullptr;
sf::RectangleShape Player::m_particle;
std::set<Object*> Player::m_players;

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
    inline OneWay(const Vector2& pos, const Vector2& size)
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

class NetObj : public virtual Object, public Renderer<sf::CircleShape>, public Collider, public NetworkObject
{
public:
    inline NetObj() : NetworkObject(1)
    {
        setRadius(5);
        setOrigin(5,5);
    }

protected:
    sf::Packet OnServerSendData() override
    {
        sf::Packet temp;
        temp << this->getPosition().x << this->getPosition().y << this->getRotation().getAngle();
        return temp;
    }

    void OnClientReceivedData(sf::Packet& packet) override
    {
        float x, y, angle;
        packet >> x >> y >> angle;
        this->setPosition(x,y);
        this->setRotation(angle);
    }

    sf::Packet OnClientSendData() override
    {
        return sf::Packet(); // empty packet
    }

    void OnServerReceivedData(sf::Packet& packet) override
    {
        // dont care about this data
    }

private:
    createDestroy();
};

int main()
{
    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Game Framework"); // , sf::Style::Fullscreen
    // window.setFramerateLimit(60);
    WindowHandler::setRenderWindow(&window);
    CanvasManager::initGUI();

    tryLoadTheme({"Dark.txt", "Black.txt"}, {"", "Assets/", "themes/", "Themes/", "assets/", "Assets/Themes/", "Assets/themes/", "assets/themes/", "assets/Themes/"});
    // -----------------------

    WorldHandler::init({0.f,0.f});
    DebugDraw debugDraw(&window);
    debugDraw.initCommands();
    WorldHandler::getWorld().SetDebugDraw(&debugDraw); // TODO implement ray cast system

    Canvas* gui = new Canvas();

    //! Required to initialize VarDisplay and CommandPrompt
    // creates the UI for the VarDisplay
    VarDisplay::init(gui->getGroup()); 
    // creates the UI for the CommandPrompt
    Command::Prompt::init(gui->getGroup());
    addThemeCommands();
    // create the UI for the TFuncDisplay
    TFuncDisplay::init(gui->getGroup());
    //! ---------------------------------------------------

    //* init code

    // NetworkType::initType(1, {[](){ return static_cast<NetworkObject*>(new NetObj()); }});
    // NetworkType::initType(2, {[](){ return static_cast<NetworkObject*>(new Player()); }});

    // using namespace udp;
    // SocketUI socketUI(gui->getGroup(), 50001);
    // Command::Handler::addCommand(Command::command{"net", "Commands for the network UI", {Command::helpCommand, "net"}, {}, {
    //     {"SetConnectionWindowVisible", "sets visiblity of connection window", {[&socketUI](Command::Data* data){ socketUI.setConnectionVisible(StringHelper::toBool(data->getToken())); }}, {"True", "False"}},
    //     {"SetInfoWindowVisible", "sets visiblity of info window", {[&socketUI](Command::Data* data){ socketUI.setInfoVisible(StringHelper::toBool(data->getToken())); }}, {"True", "False"}},
    // }});
    // socketUI.setConnectionVisible();
    // socketUI.setInfoVisible();
    // socketUI.getInfoWindow()->setPosition({socketUI.getConnectionWindow()->getSize().x, 0});
    // NetworkObjectManager::init(&socketUI.getServer(), &socketUI.getClient());

    new Wall({96,108}, {192,10});
    new Wall({96,0}, {192,10});
    new Wall({192, 54}, {10, 108});
    new Wall({0, 54}, {10, 108});
    // for (int i = 0; i < 100; i++)
    //     (new Player())->setPosition({50,50});
    auto p = new Player();
    p->setPosition({15,10});
    // p->setRotation(45);
    // p->name = "Something";

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

    // NetworkObject* temp = nullptr;
    // NetworkObjectManager::getServer()->onConnectionOpen([&temp](){
    //     temp = new Player(true);
    //     temp->createNetworkObject();
    // });

    auto canvas = new Canvas();
    canvas->setGlobalSpace();
    canvas->setPosition({50,50});
    canvas->add(tgui::Panel::create());
    canvas->add(tgui::ChildWindow::create());

    float secondTimer = 0;
    int fps = 0;
    auto fpsLabel = tgui::Label::create("FPS");
    gui->add(fpsLabel);
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
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape))
                window.close();

            if (Command::Prompt::UpdateEvent(event))
                continue;
            if (CanvasManager::handleEvent(event))
                continue;

            //! Required for LiveVar and CommandPrompt to work as intended
            LiveVar::UpdateLiveVars(event);
            //! ----------------------------------------------------------
        }

        UpdateManager::Update(deltaTime.asSeconds());
        if (fixedUpdate >= 0.2)
        {
            UpdateManager::FixedUpdate();
            fixedUpdate = 0;
        }
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
        UpdateManager::LateUpdate(deltaTime.asSeconds());

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
    CanvasManager::closeGUI();
    window.close();

    // NetworkObjectManager::getClient()->closeConnection(); // TODO do this in a function
    // NetworkObjectManager::getServer()->closeConnection();

    return 0;
}

void addThemeCommands()
{
    Command::Handler::addCommand(Command::command{"setTheme", "Function used to set the theme of the UI (The previous outputs in the command prompt will not get updated color)", 
        {Command::print, "Trying calling one of the sub commands"}, {},
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
