#ifndef GRAPHS_H
#define GRAPHS_H

#pragma once

#include "Utils/Graph.hpp"
#include "TGUI/AllWidgets.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"
#include "Utils/Debug/CommandHandler.hpp"
#include "Utils/TGUICommon.hpp"
#include "Utils/Debug/LiveVar.hpp"
#include "Utils/Debug/VarDisplay.hpp"

class Graphs
{
public:
    static void init(tgui::Gui& gui);

protected:

private:
    static tguiCommon::ChildWindow _windowData;
    static Graph _graph;
};

#endif
