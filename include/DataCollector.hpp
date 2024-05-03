#ifndef DATA_COLLECTOR_H
#define DATA_COLLECTOR_H

#pragma once

#include "Utils/iniParser.hpp"
#include "Utils/Stopwatch.hpp"

// TODO finish this later
class DataCollector
{
public:
    /// @brief sets the current name for collected data
    static void setDataName(const std::string& name);
    /// @returns the current name
    static std::string getDataName();
    /// @brief removes all data stored in the current name
    static void removeData();


protected:

private:
    inline DataCollector() = default;

};

#endif
