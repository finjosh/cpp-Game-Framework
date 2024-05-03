#include <iostream>

#include "SFML/Graphics.hpp"

#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"

#include "box2d/Box2D.h"

#include "Utils.hpp"
#include "Graphics/WindowHandler.hpp"
#include "Physics/WorldHandler.hpp"
#include "Utils/iniParser.hpp"

#include "UpdateManager.hpp"
#include "Graphics/DrawableManager.hpp"
#include "ObjectManager.hpp"
#include "Physics/CollisionManager.hpp"

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

    auto fpsLabel = tgui::Label::create("FPS");
    gui.add(fpsLabel);
    auto objectsLabel = tgui::Label::create("Objects");
    objectsLabel->setPosition({0,25});
    gui.add(objectsLabel);

    std::vector<double> totalTime;
    totalTime.resize(30000);
    std::vector<size_t> numObjects;
    numObjects.resize(30000);
    timer::Stopwatch timer;
    int tests = 10;
    int counter = 0;
    auto testLabel = tgui::Label::create("Test: " + std::to_string(counter+1) + "/" + std::to_string(tests));
    testLabel->setPosition({0,50});
    gui.add(testLabel);

    int fps = 0;
    float secondTimer = 0;

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
        if (secondTimer >= 1)
        {
            fpsLabel->setText("FPS: " + std::to_string(fps));
            fps = 0;
            secondTimer = 0;
        }
        fps++;
        objectsLabel->setText("Objects: " + std::to_string(ObjectManager::getNumberOfObjects()));
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
        timer.start();
        UpdateManager::Update(deltaTime.asSeconds());
        auto temp = timer.lap<timer::nanoseconds>();
        totalTime[UpdateManager::getNumberOfObjects()] += temp/10000000.0;
        numObjects[UpdateManager::getNumberOfObjects()] = UpdateManager::getNumberOfObjects();
        if (fixedUpdate >= 0.2)
        {
            fixedUpdate = 0;
            temp = timer.lap<timer::nanoseconds>();
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

        if (UpdateManager::getNumberOfObjects() >= 30000)
        {
            counter++;
            if (counter >= tests)
                break;

            testLabel->setText("Test: " + std::to_string(counter+1) + "/" + std::to_string(tests));

            ObjectManager::destroyAllObjects();
        }

        new EmptyUpdateObject();

        // ---------------

        DrawableManager::draw(window);

        // draw for tgui
        gui.draw();
        // display for sfml window
        window.display();
    }

    iniParser saveData;
    saveData.setFilePath("data.ini");
    if (!saveData.isOpen())
    {
        saveData.createFile("data.ini");
        saveData.setFilePath("data.ini");
    }
    saveData.overrideData();
    saveData.addValue("General", "Sizes", StringHelper::fromVector<size_t>(numObjects));
    for (size_t i = 0; i < totalTime.size(); i++)
    {
        totalTime[i] /= tests;
    }
    saveData.addValue("Normal Update", "Times", StringHelper::fromVector<double>(totalTime));
    saveData.SaveData();

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
