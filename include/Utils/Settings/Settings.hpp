#ifndef GAME_SETTINGS_HPP
#define GAME_SETTINGS_HPP

#pragma once

#include <string>
#include <set>
#include <map>
#include <list>

#include "Utils/EventHelper.hpp"
#include "Utils/Settings/SettingBase.hpp"

// TODO add a way for the compare function to give an error message to this class which can be displayed to the user later
/// @note settings are only compared by name
class Settings
{
public:
    Settings() = default;
    ~Settings();
    /// @brief tries to load settings from the given file
    /// @warning you must first create all settings and then load from file or else nothing will be loaded 
    /// @note the first file that matches the name will be used for loading settings
    /// @param fileName the name of the settings files
    /// @param directories the directories to search for the file (must end in a "/")
    /// @returns false if no file was found true if file was found and attempted to load from
    bool tryLoadFromFile(const std::string& fileName = "Settings.ini", const std::list<std::string>& directories = {});
    /// @brief saves the settings to file 
    /// @param file the path and name of the file
    /// @note if the path does not exists it will first be created
    void save(const std::string& file = "Settings.ini") const;
    /// @brief only creates section if no section with the given name already exists
    void createSection(const std::string& section);
    /// @param setting should has control given over to this (will be deleted when this object is deleted)
    /// @note Setting is ONLY compared by name NOT type
    /// @note if the setting already exists does NOT replace
    /// @note creates section if it does not exist
    /// @returns false if setting with the same name and section already exists
    bool createSetting(const std::string& section, SettingBase* setting);
    /// @brief tries to find the given section and returns the set of all settings in the set
    /// @returns nullptr if the section is not found
    const std::set<SettingBase*, _SettingBaseComp>* getSection(const std::string& section) const;
    /// @returns a list of all the section names
    std::list<std::string> getSections() const;
    /// @param section the section the setting is in
    /// @param name the name of the setting
    /// @returns nullptr if not found 
    SettingBase* getSetting(const std::string& section, const std::string& name);

    /// @note Optional parameter: the new sections name
    EventHelper::EventDynamic<std::string> onSectionCreated;
    /// @note Optional parameter: the new settings section name
    /// @note Optional parameter: the new settings base objects
    EventHelper::EventDynamic2<std::string, SettingBase*> onSettingCreated;

protected:
    std::set<SettingBase*, _SettingBaseComp>* m_getSection(const std::string& section);

private:
    /// @brief first is the section
    /// @brief second is the set of all settings in the section
    std::map<std::string, std::set<SettingBase*, _SettingBaseComp>> m_settings;
};

// //* test settings
// m_settings->createSubSectionLabel("Test Section", "Boolean Settings");
// m_settings->createSetting<bool>("Test Section", {"Bool", true, "A test for boolean settings"});
// m_settings->createSubSectionLabel("Test Section", "Integer Settings");
// m_settings->createSetting<int>("Test Section", {"Int1", 1, "A test for int settings with any input"});
// m_settings->createSetting<int>("Test Section", {"Int2", 2, "A test for int settings with a conditional input (-7 <= int <= 77)", [](int value){ return -7 <= value && value <= 77; }});
// m_settings->createSetting<int>("Test Section", {"Int3", 3, "A test for int settings with a range slider", -7, 77, 1});
// m_settings->createSubSectionLabel("Test Section", "Unsigned Integer Settings");
// m_settings->createSetting<unsigned int>("Test Section", {"UInt1", 1, "A test for unsigned ints settings with any input"});
// m_settings->createSetting<unsigned int>("Test Section", {"UInt2", 2, "A test for unsigned ints settings with a conditional input (7 <= int)", [](unsigned int value){ return 7 <= value; }});
// m_settings->createSetting<unsigned int>("Test Section", {"UInt3", 3, "A test for unsigned ints settings with a range slider", 7, 77, 1});
// m_settings->createSubSectionLabel("Test Section", "Floating Point Settings");
// m_settings->createSetting<float>("Test Section", {"Float1", 1.f, "A test for float settings with any input"});
// m_settings->createSetting<float>("Test Section", {"Float2", 2.f, "A test for float settings with a conditional input (0.7 <= float <= 7)", [](float value){ return 0.699999 <= value && value <= 7; }});
// m_settings->createSetting<float>("Test Section", {"Float3", 3.f, "A test for float settings with a range slider", 0.7, 77, 0.1});
// m_settings->createSubSectionLabel("Test Section", "String Settings");
// m_settings->createSetting<std::string>("Test Section", {"String1", "one", "A test for string settings with any input"});
// m_settings->createSetting<std::string>("Test Section", {"String2", "two", "A test for string settings with a list of input options", {"Cat", "Dog", "Coding", "Math", "Physics"}});
// m_settings->createSetting<std::string>("Test Section", {"String3", "three", "A test for string settings with a conditional input (string must start with \"I\")", [](std::string value){ return value.starts_with("I"); }});

#endif
