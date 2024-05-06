#include <iostream>

#include "SFML/Graphics.hpp"

#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"

#include "Utils.hpp"
#include "Utils/iniParser.hpp"

using namespace std;
using namespace sf;

void addThemeCommands();

/*
*Data storage
-------------
*X values should be stored in the section "General" and have the key name "X"
*X and Y axis names are stored in the "General" section and have the key name "XLabel" and "YLabel" respectively
-------------
*Any other section will be graphed individually 
*The name of the second is what will be shown in the legend
*The data should have a key name of "Values"
*Color of the data is stored under "Color" and must be a list of size 4
*Graph Type is stored under "Graph Type" options: {"Scatter", "Histogram", "Line", "Bar"}
*/

/// @brief if not able to convert to a color returns sf::Color::White
sf::Color toColor(const std::string& str)
{
    std::vector<int> data = StringHelper::toVector<int>(str, &StringHelper::toInt);
    if (data.size() != 4)
        return sf::Color::White;
    return {(uint8_t)data[0], (uint8_t)data[1], (uint8_t)data[2], (uint8_t)data[3]};
}

// @returns string formatted as a list of size 4
std::string toString(const sf::Color& color)
{
    return "[" + std::to_string(color.r) + "," +
                        std::to_string(color.g) + "," +
                        std::to_string(color.b) + "," +
                        std::to_string(color.a) + "]";
}

/// @brief assumes that the data has all the required fields
void makeGraph(Graph& graph, const iniParser& data, const float& scatterThickness = 5, const float& lineThickness = 5, const float& barThickness = 5)
{
    graph.clearDataSets();
    graph.setXLable(data.getValue("General", "XLabel"));
    graph.setYLable(data.getValue("General", "YLabel"));
    GraphData graphData;
    graphData.setXValues(StringHelper::toVector<float>(data.getValue("General", "X")));
    
    for (auto i : data.getLoadedData())
    {
        if (i.first == "General")
            continue;
        graphData.setYValues(StringHelper::toVector<float>(data.getValue(i.first, "Values")));
        graphData.setColor(toColor(data.getValue(i.first, "Color")));
        graphData.setGraphType(Graph::strToType(data.getValue(i.first, "Graph Type")));
        graphData.setLabel(i.first);

        switch (graphData.getGraphType())
        {
        case GraphType::Line:
            graphData.setThickness(lineThickness);
            break;
        
        case GraphType::Scatter:
            graphData.setThickness(scatterThickness);
            break;

        case GraphType::Bar:
            graphData.setThickness(barThickness);
            break;

        default:
            break;
        }

        graph.addDataSet(graphData);
    }
    graph.Update();
}

int main()
{
    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Data Visualization", sf::Style::Fullscreen);
    window.setFramerateLimit(144);

    tgui::Gui gui{window};
    gui.setRelativeView({0, 0, 1920/(float)window.getSize().x, 1080/(float)window.getSize().y});
    tgui::Theme::setDefault("Assets/themes/Dark.txt");
    Command::color::setDefaultColor({255,255,255,255});
    // -----------------------

    //! Required to initialize VarDisplay and CommandPrompt
    // creates the UI for the VarDisplay
    VarDisplay::init(gui); 
    // creates the UI for the CommandPrompt
    Command::Prompt::init(gui);
    addThemeCommands();
    // create the UI for the TFuncDisplay
    TFuncDisplay::init(gui);
    //! ---------------------------------------------------

    tgui::String dataFolder = "Data";
    std::vector<tgui::String> dataFiles;
    for (const auto& entry: filesystem::directory_iterator(dataFolder.toStdString()))
        dataFiles.emplace_back(entry.path());
    size_t currentFile = 0;

    sf::Font font;
    font.loadFromFile("JetBrainsMono-Regular.ttf");
    GraphData data({0}, {0}, sf::Color::White, "Times", GraphType::Line);
    data.setThickness(1);
    Graph graph;
    graph.setMargin(100);
    graph.setCharSize(20);
    graph.setDecimalPrecision(2);
    graph.setResolution({1920,1000});
    graph.setSize({1920,1000});
    graph.setFont(font);
    iniParser temp;
    temp.setFilePath("data.ini");
    temp.LoadData();
    makeGraph(graph, temp, 2, 2);

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        EventHelper::Event::ThreadSafe::update();
        window.clear();
        // updating the delta time var
        sf::Time deltaTime = deltaClock.restart();
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
        //! Updates all the vars being displayed
        VarDisplay::Update();
        //! ------------------------------=-----
        //! Updates all Terminating Functions
        TerminatingFunction::UpdateFunctions(deltaTime.asSeconds());
        //* Updates for the terminating functions display
        TFuncDisplay::update(); // updates the terminating functions display
        //! ------------------------------

        window.draw(graph);

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
