#include <iostream>

#include "SFML/Graphics.hpp"
#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"
// #include "BS_thread_pool.hpp"

#include "Utils/CommandPrompt.hpp"
#include "Utils/Debug/LiveVar.hpp"
#include "Utils/Debug/VarDisplay.hpp"
#include "Utils/Debug/TFuncDisplay.hpp"
#include "Utils/iniParser.hpp"

#include "ObjectManager.hpp"
#include "UpdateManager.hpp"
#include "Particles/ParticleEmitter.hpp"

#include "Graphics/Renderer.hpp"
#include "Graphics/WindowHandler.hpp"
#include "Graphics/DrawableManager.hpp"
#include "Graphics/CameraManager.hpp"
#include "Graphics/CanvasManager.hpp"

#include "Physics/WorldHandler.hpp"
#include "Physics/CollisionManager.hpp"
#include "Physics/DebugDraw.hpp"

#include "Input.hpp"

#include "Utils/Settings/SettingsUI.hpp"
#include "Utils/Settings/AllSettingTypes.hpp"

using namespace std;

// TODO make animation class
// TODO implement fixture and joint isValid functions that connect the the collider with the body that relates to them
// TODO update managers to be singletons and make the getter function thread safe

/// @param themes in order of most wanted
/// @param directories directories to check in ("" for current)
void tryLoadTheme(std::list<std::string> themes, std::list<std::string> directories);

class Wall : public virtual Object, public Collider, public Renderer<sf::RectangleShape>
{
public:
    inline Wall(const Vector2& pos, const Vector2& size)
    {
        setPosition(pos);

        Fixture::Shape::Polygon shape;
        shape.makeBox(size.x, size.y);

        FixtureDef fixtureDef;
        fixtureDef.setFriction(1);

        Collider::createFixture(shape, fixtureDef);
        Collider::setType(b2BodyType::b2_staticBody);

        setSize({size.x,size.y});
        setOrigin({size.x/2,size.y/2});
        setFillColor(sf::Color::Red);
    }
};

class Player : public virtual Object, public Collider, public Renderer<sf::RectangleShape>, public UpdateInterface
{
public:
    std::string name = "Random Name";
    ParticleEmitter::Ptr m_hitParticle = nullptr;
    static sf::RectangleShape m_particle;
    static Camera::Ptr m_camera; // default is nullptr
    static std::set<Object*> m_players;

    inline Player()
    {
        m_players.emplace(this);

        Fixture::Shape::Polygon shape;
        shape.makeBox(5,5);

        FixtureDef fixtureDef;
        fixtureDef.enablePreSolveEvents(true);
        fixtureDef.setFriction(1);

        Collider::createFixture(shape, fixtureDef);

        setSize({5,5});
        setOrigin({2.5,2.5});
        setFillColor(sf::Color::White);

        m_particle.setFillColor(sf::Color::Green);
        m_particle.setSize({5,5});
        m_particle.setOrigin({2.5,2.5});
        m_hitParticle.set(new ParticleEmitter(&m_particle, {0,0}, 10, 0, 0, 1, 10, 0.5, 360));

        if (!m_camera)
        {
            m_camera = new Camera();
            m_camera->setMainCamera();
            m_camera->setRotationLocked(true);
        }
    }

    inline ~Player()
    {
        m_players.erase(this);
    }

    inline virtual void Update(float deltaTime) override
    {
        if (Input::get().isPressed(sf::Keyboard::Key::W))
        {
            applyForceToCenter({0,-120000*deltaTime});
        }
        if (Input::get().isPressed(sf::Keyboard::Key::A))
        {
            applyForceToCenter({-120000*deltaTime,0});
        }
        if (Input::get().isPressed(sf::Keyboard::Key::S))
        {
            applyForceToCenter({0,120000*deltaTime});
        }
        if (Input::get().isPressed(sf::Keyboard::Key::D))
        {
            applyForceToCenter({120000*deltaTime,0});
        }
        if (Input::get().isPressed(sf::Keyboard::Key::E))
        {
            applyTorque(500000*deltaTime);
        }
        if (Input::get().isPressed(sf::Keyboard::Key::Q))
        {
            applyTorque(-500000*deltaTime);
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
        if (data.getPointCount() > 0 && getLinearVelocity().lengthSquared() > 250 && data.getCollider()->cast<Wall>())
        {
            m_hitParticle->setPosition(data.getContactPoint(0));
            m_hitParticle->emit();
        }
    }
};

Camera::Ptr Player::m_camera = nullptr;
sf::RectangleShape Player::m_particle;
std::set<Object*> Player::m_players;

class Sensor : public virtual Object, public Renderer<sf::RectangleShape>, public Collider
{
public:
    Sensor(funcHelper::func<void> onEnter, const Object::Ptr<>& object = Object::Ptr<>(nullptr))
    {
        Fixture::Shape::Polygon shape;
        shape.makeBox(20,20);

        Collider::createFixtureSensor(shape);
        Collider::setType(b2_staticBody);

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
    funcHelper::func<> m_onEnter;
    Object::Ptr<> m_object;
};

class OneWay : public virtual Object, public Renderer<sf::RectangleShape>, public Collider, public UpdateInterface
{
public:
    inline OneWay(const Vector2& pos, const Vector2& size)
    {
        Fixture::Shape::Polygon shape;
        shape.makeBox(size.x, size.y);
        
        FixtureDef fixtureDef;
        fixtureDef.setFilter({0x0001, 0x0000, 0});

        Collider::createFixture(shape);

        setSize({size.x,size.y});
        setOrigin({size.x/2,size.y/2});
        setFillColor(sf::Color::Transparent);
        setOutlineThickness(0.5);
        setOutlineColor(sf::Color::Blue);

        auto opening = new Renderer<sf::RectangleShape>();
        opening->setParent(this);
        opening->setSize({size.x, size.y/8});
        opening->setFillColor(sf::Color::Green);
        opening->setOrigin({size.x/2, -size.y/2});

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

    bool PreSolve(PreSolveData data) override
    {
        if (data.getCollider()->cast<Wall>() != nullptr)
            return true;
        if (getLocalVector(data.getNormal()).y < -0.5f) // if the object is colliding from the bottom
        {
            return false;
            m_freeFlow.emplace(data.getCollider());
        }
        else if (m_freeFlow.find(data.getCollider()) != m_freeFlow.end())
        {
            return false;
        }
        return true;
    }

    void EndContact(ContactData data) override
    {
        m_freeFlow.erase(data.getCollider());
    }

private:
    std::set<const Collider*> m_freeFlow;
};

int main()
{
    WindowHandler::initRenderWindow(sf::VideoMode::getDesktopMode(), "Game Framework");
    CanvasManager::initGUI();

    tryLoadTheme({"Dark.txt", "Black.txt"}, {"", "Assets/", "themes/", "Themes/", "assets/", "Assets/Themes/", "Assets/themes/", "assets/themes/", "assets/Themes/"});
    // -----------------------

    WorldHandler::get()->init({0.f,0.f}); // TODO implement ray cast system
    DebugDraw::get().initCommands();
    // DebugDraw::get().drawAll(true);
    Input::get(); // initializing the input dictionary for string conversions

    Canvas* gui = new Canvas();

    //! Required to initialize VarDisplay and CommandPrompt
    // creates the UI for the VarDisplay
    VarDisplay::init(gui->getGroup()); 
    // creates the UI for the CommandPrompt
    Command::Prompt::init(gui->getGroup());
    // create the UI for the TFuncDisplay
    TFuncDisplay::init(gui->getGroup());
    //! ---------------------------------------------------

    //* init code

    new Wall({96,108}, {192,10});
    new Wall({96,0}, {192,10});
    new Wall({192, 54}, {10, 108});
    new Wall({0, 54}, {10, 108});

    for (int i = 0; i < 100; i++)
    {
        auto p = new Player();
        p->setPosition({15,10});
    }
    auto p = new Player();
    p->setPosition({15,10});
    new Sensor([](){}, p);

    sf::RectangleShape particleShape;
    particleShape.setSize({10,10});
    particleShape.setOrigin({5,5});
    particleShape.setFillColor(sf::Color::Magenta);

    new OneWay({40,25}, {40,10});

    float secondTimer = 0;
    int fps = 0;
    auto fpsLabel = tgui::Label::create("FPS");
    // gui->add(fpsLabel);
    // -------------

    sf::Clock deltaClock;
    float fixedUpdate = 0;
    UpdateManager::Start();
    while (WindowHandler::getRenderWindow()->isOpen())
    {
        EventHelper::Event::ThreadSafe::update();
        // updating the delta time var
        sf::Time deltaTime = deltaClock.restart();
        fixedUpdate += deltaTime.asSeconds();
        secondTimer += deltaTime.asSeconds();

        Input::get().UpdateJustStates();
        while (const std::optional<sf::Event> event = WindowHandler::getRenderWindow()->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                WindowHandler::getRenderWindow()->close();
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) 
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                    WindowHandler::getRenderWindow()->close();

            bool wasHandled = false;
            if (Command::Prompt::UpdateEvent(event.value()))
                wasHandled = true;
            else if (CanvasManager::handleEvent(event.value()))
                wasHandled = true;
            else
                LiveVar::UpdateLiveVars(event.value());

            Input::get().HandelEvent(event.value(), wasHandled);
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
        WorldHandler::get()->updateWorld(deltaTime.asSeconds()); // updates the world physics
        CollisionManager::get()->Update(); // updates the collision callbacks
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
    WindowHandler::getRenderWindow()->close();

    return 0;
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
