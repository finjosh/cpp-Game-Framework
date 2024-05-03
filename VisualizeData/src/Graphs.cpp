#include "Graphs.hpp"
#include "Utils/iniParser.hpp"

tguiCommon::ChildWindow Graphs::_windowData;
Graph Graphs::_graph;

void Graphs::init(tgui::Gui& gui)
{
    sf::Font font;
    font.loadFromFile("JetBrainsMono-Regular.ttf");
    GraphData data({0}, {0}, sf::Color::White, "Random Data", GraphType::Line);
    data.setThickness(5);
    _graph.setXLable("# objects");
    _graph.setYLable("Update time");
    _graph.setMargin(100);
    _graph.setCharSize(20);
    _graph.setDecimalPrecision(1);
    _graph.setResolution({1920,1000});
    _graph.setFont(font);

    auto parent = tgui::ChildWindow::create("Graphs", tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Maximize);
    parent->setResizable();
    parent->setSize({"80%","100%"});
    auto panel = tgui::ScrollablePanel::create();
    parent->add(panel);
    auto list = tgui::HorizontalWrap::create();
    panel->add(list);

    iniParser temp;
    temp.setFilePath("data.ini");
    temp.LoadData();
    data.setXValues(StringHelper::toVector<float>(temp.getValue("General", "Sizes")));
    data.setYValues(StringHelper::toVector<float>(temp.getValue("Normal Update", "Times")));
    _graph.addDataSet(data);
    _graph.Update();

    tgui::Texture texture;
    texture.loadFromPixelData(_graph.getTexture().getSize(), _graph.getTexture().copyToImage().getPixelsPtr());
    list->add(tgui::Picture::create(texture));
    list->getWidgets().back()->setSize({1920,1000});

    list->getRenderer()->setSpaceBetweenWidgets(10);
    list->setSize({"100% - 15", list->getWidgets().back()->getPosition().y + list->getWidgets().back()->getFullSize().y + 15});
    list->onSizeChange([list](){list->setSize({"100% - 15", list->getWidgets().back()->getPosition().y + list->getWidgets().back()->getFullSize().y + 15});});

    parent->onClosing(tguiCommon::ChildWindow::closeWindow, parent);
    parent->onMaximize(tguiCommon::ChildWindow::maximizeWindow, &_windowData);

    tguiCommon::ChildWindow::createOpenCloseCommand("GraphExamples", parent);

    gui.add(parent);
}
