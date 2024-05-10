#include "TestHelper.hpp"
#include "TGUI/DefaultFont.hpp"

std::string TestHelper::m_name = "Unknown";
funcHelper::func<> TestHelper::m_test = {[](){}};
funcHelper::func<> TestHelper::m_iterateTest = {[](){}};
funcHelper::func<> TestHelper::m_resetTest = {[](){}};
size_t TestHelper::m_iterations = 1;
size_t TestHelper::m_startingValue = 0;
std::string TestHelper::m_xName = "X";
size_t TestHelper::m_repetitions = 1;
size_t TestHelper::m_currentTest = 1;
timer::TimeFormat TestHelper::m_timeFormat = timer::TimeFormat::MILLISECONDS;
std::vector<float> TestHelper::m_yData;

void TestHelper::setName(const std::string& name)
{
    m_name = name;
}

std::string TestHelper::getName()
{
    return m_name;
}

void TestHelper::reset()
{
    m_name = "Unknown";
    m_xName = "X";
    m_repetitions = 1;
    m_currentTest = 0;
    m_test.setFunction([](){});
    m_iterateTest.setFunction([](){});
    m_resetTest.setFunction([](){});
    m_iterations = 1;
    m_startingValue = 0;
    m_yData.clear();
    m_timeFormat = timer::TimeFormat::MILLISECONDS;
}

void TestHelper::initTest(const std::string& name, const std::string& xName,
                          const funcHelper::func<>& test, const funcHelper::func<>& iterateTest, const size_t& iterations, const size_t& startingValue,
                          const funcHelper::func<>& resetTest, const size_t& repetitions, const timer::TimeFormat& timeFormat)
{
    reset();
    m_name = name;
    m_xName = xName;
    m_test = test;
    m_iterateTest = iterateTest;
    m_resetTest = resetTest;
    m_iterations = iterations;
    m_startingValue = startingValue;
    m_repetitions = repetitions;
    m_timeFormat = timeFormat;
    m_yData.resize(iterations);
}

std::string TestHelper::runTest(const TestHelper::FileExists& fileExists, const std::string& suffix, std::string folderPath)
{
    if (!std::filesystem::is_directory(folderPath))
    {
        folderPath = "";
    }

    iniParser data;
    // if the y data size there is no test to do
    data.setFilePath(folderPath + m_name + "Test" + suffix + ".ini");
    if (m_yData.size() == 0 || (data.isOpen() && fileExists == FileExists::DoNothing))
    {
        return "";
    }

    auto desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(desktopMode.width / 3, desktopMode.height / 3, desktopMode.bitsPerPixel), "Test: " + m_name, sf::Style::Resize);
    tgui::Gui gui{window};

    std::list<std::string> themePaths = {"Assets/themes/Dark.txt", "themes/Dark.txt",
                                         "Assets/themes/Black.txt", "themes/Black.txt"};

    for (auto path: themePaths)
    {
        if (std::filesystem::exists(path))
        {
            tgui::Theme::setDefault(path);
            break;
        }
    }

    auto panel = tgui::HorizontalWrap::create();
    panel->setAutoLayoutUpdateEnabled(true);
    auto testProgress = tgui::ProgressBar::create();
    auto progress = tgui::ProgressBar::create();
    auto IPS = tgui::Label::create(); // iterations per second
    panel->add(testProgress);
    panel->add(progress);
    panel->add(IPS);
    IPS->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    IPS->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    IPS->setTextSize(window.getSize().x/20);
    gui.add(panel);

    testProgress->setSize({"100%", "33%"});
    testProgress->setText("Test: " + std::to_string(m_currentTest) + "/" + std::to_string(m_repetitions));
    testProgress->setTextSize(window.getSize().x/20);
    testProgress->setMaximum(m_repetitions);
    progress->setSize({"100%", "33%"});
    progress->setText("Test Progress: 0%");
    progress->setTextSize(window.getSize().x/20);
    progress->setMaximum(m_iterations);
    IPS->setSize({"100%", "33%"});
    IPS->setText("Iterations Per Second (IPS)");
    panel->setSize({"100%", "100%"});
    panel->updateChildrenWithAutoLayout();

    timer::Stopwatch timer;
    size_t iter = 0;
    double conversionFactor = 1.0;
    std::string timeFormatStr = "ns";
    switch (m_timeFormat)
    {
    case timer::SECONDS:
        conversionFactor = 1000000000.0;
        timeFormatStr = "s";
        break;
    
    case timer::MILLISECONDS:
        conversionFactor = 1000000.0;
        timeFormatStr = "ms";
        break;

    case timer::MICROSECONDS:
        conversionFactor = 1000.0;
        timeFormatStr = "mus";
        break;

    default:
        break;
    }

    for (size_t i = 0; i < m_startingValue; i++)
    {
        m_iterateTest.invoke();
    }

    sf::Clock deltaClock;
    float second = 0;
    size_t ips = 0;
    while (window.isOpen())
    {
        second += deltaClock.restart().asSeconds();
        ips++;
        if (second >= 1)
        {
            IPS->setText("IPS: " + std::to_string(ips));
            ips = 0;
            second = 0;
        }
        window.clear();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                return "";

            if (event.type == sf::Event::Resized)
            {
                panel->setSize(window.getSize().x, window.getSize().y);
                panel->updateChildrenWithAutoLayout();
                testProgress->setTextSize(window.getSize().x/20);
                progress->setTextSize(window.getSize().x/20);
                IPS->setTextSize(window.getSize().x/20);
            }

            gui.handleEvent(event);
        }

        if (iter >= m_iterations)
        {
            m_currentTest++;
            if (m_currentTest > m_repetitions)
                break;
            iter = 0;
            m_resetTest.invoke();
            testProgress->setText("Test: " + std::to_string(m_currentTest) + "/" + std::to_string(m_repetitions));
            testProgress->setValue(m_currentTest);

            for (size_t i = 0; i < m_startingValue; i++)
            {
                m_iterateTest.invoke();
            }
        }
        
        uint64_t nanoSec;
        timer.start();
        m_test.invoke();
        nanoSec = timer.lap<timer::nanoseconds>();
        m_yData[iter] = (float)(nanoSec/conversionFactor); // using y data as total for now
        iter++;
        m_iterateTest.invoke();

        progress->setText("Test Progress: " + StringHelper::FloatToStringRound((float)(iter)/(m_iterations)*100, 1) + "%");
        progress->setValue(iter);

        // draw for tgui
        gui.draw();
        // display for sfml window
        window.display();
    }
    window.close();

    // finding the average of the data collected
    if (m_repetitions != 1)
        for (size_t i: m_yData)
        {
            m_yData[i] /= m_repetitions;
        }

    size_t temp = 0;
    data.setFilePath(folderPath + m_name + "Test" + suffix + ".ini");
    while (data.isOpen() && fileExists != FileExists::Replace)
    {
        temp++;
        data.setFilePath(folderPath + m_name + "Test (" + std::to_string(temp) + ")" + suffix + ".ini");
    }

    if (temp == 0)
    {
        data.createFile(folderPath + m_name + "Test" + suffix + ".ini");
        data.setFilePath(folderPath + m_name + "Test" + suffix + ".ini");
    }
    else
    {
        data.createFile(folderPath + m_name + "Test (" + std::to_string(temp) + ")" + suffix + ".ini");
        data.setFilePath(folderPath + m_name + "Test (" + std::to_string(temp) + ")" + suffix + ".ini");
    }
    data.overrideData();
    data.addValue("General", "XLabel", m_xName);
    data.addValue("General", "YLabel", "Time (" + timeFormatStr + ")");
    data.addValue(m_name, "Values", StringHelper::fromVector<float>(m_yData));
    std::vector<size_t> xValues;
    xValues.resize(m_iterations);
    std::iota(xValues.begin(), xValues.end(), m_startingValue);
    data.addValue("General", "X", StringHelper::fromVector<size_t>(xValues, &std::to_string));
    data.SaveData();
    return data.getFilePath(); // path is just the file name in this case
}

void TestHelper::setXName(const std::string& name)
{
    m_xName = name;
}

std::string TestHelper::getXName()
{
    return m_xName;
}

void TestHelper::graphData(const std::string& folder, const std::string& suffix)
{
    if (folder != "" && !std::filesystem::is_directory(folder))
        return;

    auto screenMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(screenMode, "Test: " + m_name, sf::Style::Fullscreen);
    tgui::Gui gui{window};

    std::list<std::string> files;
    for (const auto& entry: std::filesystem::directory_iterator(folder == "" ? std::filesystem::current_path() : folder))
    {
        if (entry.path().extension() == ".ini" && entry.path().filename().generic_string().ends_with(suffix))
            files.push_back(entry.path().filename().generic_string());
    }

    auto Next = tgui::Button::create("Next");
    auto Last = tgui::Button::create("Last");
    auto label = tgui::Label::create("No files available");
    auto childWindow = tgui::ChildWindow::create("Navigation", tgui::ChildWindow::TitleButton::None);
    childWindow->setResizable(false);
    gui.add(childWindow);
    childWindow->setSize({"20%","10%"});
    childWindow->setPosition({"30%", "5%"});
    childWindow->add(Next);
    childWindow->add(Last);
    Next->setSize("20%", "100%");
    Next->setPosition("80%",0);
    Last->setSize("20%", "100%");
    Last->setPosition(0,0);
    childWindow->add(label);
    label->setSize({"60%", "100%"});
    label->setMaximumTextWidth(label->getSize().x);
    label->setPosition({"20%", 0});
    label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);

    std::list<std::string>::iterator currentFile = files.begin();

    Graph graph;
    sf::Font font;
    font.loadFromMemory(static_cast<const unsigned char*>(defaultFontBytes), sizeof(defaultFontBytes));
    graph.setFont(font);
    graph.setResolution({window.getSize().x, window.getSize().y});
    graph.setSize({(float)window.getSize().x, (float)window.getSize().y});
    graph.setDecimalPrecision(3);

    if (currentFile != --files.end())
    {
        label->setText(*currentFile);
        iniParser temp;
        temp.setFilePath(*currentFile);
        if (temp.LoadData())
            makeGraph(graph, temp);
    }

    Next->setEnabled(currentFile != --files.end());
    Next->onClick([Last, Next, &currentFile, label, &files, &graph](){
        currentFile++;
        label->setText(*currentFile);
        Last->setEnabled(currentFile != files.begin());
        Next->setEnabled(currentFile != --files.end());
        iniParser temp;
        temp.setFilePath(*currentFile);
        if (temp.LoadData())
            makeGraph(graph, temp);
    });
    Last->setEnabled(false);
    Last->onClick([Last, Next, &currentFile, label, &files, &graph](){
        currentFile--;
        label->setText(*currentFile);
        Last->setEnabled(currentFile != files.begin());
        Next->setEnabled(currentFile != --files.end());
        iniParser temp;
        temp.setFilePath(*currentFile);
        if (temp.LoadData())
            makeGraph(graph, temp);
    });

    while (window.isOpen())
    {
        window.clear();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
                return;

            gui.handleEvent(event);
        }

        window.draw(graph);

        // draw for tgui
        gui.draw();
        // display for sfml window
        window.display();
    }
    window.close();
}

void TestHelper::makeGraph(Graph& graph, const iniParser& data, const float& thickness)
{
    if (data.getValue("General", "X") == "\0")
        return;

    graph.clearDataSets();
    graph.setXLable(data.getValue("General", "XLabel"));
    graph.setYLable(data.getValue("General", "YLabel"));
    GraphData graphData;
    graphData.setXValues(StringHelper::toVector<float>(data.getValue("General", "X")));
    
    for (auto i: data.getLoadedData())
    {
        if (i.first == "General")
            continue;
        graphData.setYValues(StringHelper::toVector<float>(data.getValue(i.first, "Values")));
        graphData.setLabel(i.first);

        graphData.setThickness(thickness);

        graph.addDataSet(graphData);
        break;
    }
    graph.Update();
}
