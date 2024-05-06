#include "TestHelper.hpp"

std::string TestHelper::m_name = "Unknown";
funcHelper::func<> TestHelper::m_test = {[](){}};
funcHelper::func<> TestHelper::m_iterateTest = {[](){}};
funcHelper::func<> TestHelper::m_resetTest = {[](){}};
size_t TestHelper::m_iterations = 1;
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
    m_yData.clear();
}

void TestHelper::initTest(const std::string& name, const std::string& xName,
                          const funcHelper::func<>& test, const funcHelper::func<>& iterateTest, const size_t& iterations, const funcHelper::func<>& resetTest,
                          const size_t& repetitions, const timer::TimeFormat& timeFormat)
{
    m_name = name;
    m_xName = xName;
    m_test = test;
    m_iterateTest = iterateTest;
    m_resetTest = resetTest;
    m_iterations = iterations;
    m_repetitions = repetitions;
    m_timeFormat = timeFormat;
    m_yData.resize(iterations);
}

void TestHelper::runTest()
{
    // if the y data size there is no test to do
    if (m_yData.size() == 0)
        return;

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
                return;

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
        }
        
        timer.start();
        m_test.invoke();
        auto nanoSec = timer.lap<timer::nanoseconds>();
        m_yData[iter] = (float)(nanoSec/conversionFactor); // using y data as total for now
        iter++;
        m_iterateTest.invoke();

        progress->setText("Test Progress: " + StringHelper::FloatToStringRound((float)iter/m_iterations*100, 1) + "%");
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

    iniParser data;
    size_t temp = 0;
    data.setFilePath(m_name + "Test.ini");
    while (data.isOpen())
    {
        temp++;
        data.setFilePath(m_name + "Test (" + std::to_string(temp) + ").ini");
    }

    if (temp == 0)
    {
        data.createFile(m_name + "Test.ini");
        data.setFilePath(m_name + "Test.ini");
    }
    else
    {
        data.createFile(m_name + "Test (" + std::to_string(temp) + ").ini");
        data.setFilePath(m_name + "Test (" + std::to_string(temp) + ").ini");
    }
    data.overrideData();
    data.addValue("General", "XLabel", m_xName);
    data.addValue("General", "YLabel", "Time (" + timeFormatStr + ")");
    data.addValue(m_name, "Values", StringHelper::fromVector<float>(m_yData));
    std::vector<size_t> xValues;
    xValues.resize(m_iterations);
    std::iota(xValues.begin(), xValues.end(), 0);
    data.addValue("General", "X", StringHelper::fromVector<size_t>(xValues, &std::to_string));
    data.SaveData();
}

void TestHelper::setXName(const std::string& name)
{
    m_xName = name;
}

std::string TestHelper::getXName()
{
    return m_xName;
}
