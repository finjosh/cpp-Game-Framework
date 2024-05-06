#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#pragma once

#include <numeric>

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"

#include "Utils/iniParser.hpp"
#include "Utils/Stopwatch.hpp"
#include "Utils/funcHelper.hpp"

class TestHelper
{
public:
    /// @brief sets the current name for collected data
    static void setName(const std::string& name);
    /// @returns the current name
    static std::string getName();
    /// @brief resets all data back to default
    static void reset();
    /// @brief sets up the test to be run
    /// @param name the name of this test
    /// @param xName the name of the iterated values
    /// @param test the function that we are testing time for
    /// @param iterateTest how we iterate to the next part of the test (should iterate by 1 in the X)
    /// @param resetTest the function that will reset the test (only required if doing multiple repetitions)
    /// @param iterations the number of iterations the test has
    /// @param repetitions the number of times we want to run the test (used to find average)
    /// @param timeFormat how the time will be stored (it will always be recorded in nanoseconds but converted to the given format)
    static void initTest(const std::string& name, const std::string& xName,
                         const funcHelper::func<>& test, const funcHelper::func<>& iterateTest, const size_t& iterations = 1, const funcHelper::func<>& resetTest = {[](){}},
                         const size_t& repetitions = 1, const timer::TimeFormat& timeFormat = timer::TimeFormat::MILLISECONDS);
    /// @note This runs the test and opens a window that shows the progress of the test
    /// @note this also saves the data collected to a 'name of test'Test.ini file 
    /// @note if the file already exists a new one will be created with a different name
    /// @warning if the window is closed early then nothing will be
    static void runTest();
    static void setXName(const std::string& name);
    static std::string getXName();

protected:

private:
    inline TestHelper() = default;

    static funcHelper::func<> m_test;
    static funcHelper::func<> m_iterateTest;
    static funcHelper::func<> m_resetTest;
    static size_t m_iterations;

    static std::string m_name;
    static std::string m_xName;
    /// @brief the total number of tests to run
    static size_t m_repetitions;
    static size_t m_currentTest;
    static timer::TimeFormat m_timeFormat;
    static std::vector<float> m_yData;
};

#endif
