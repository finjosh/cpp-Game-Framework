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
// auto temp = new SettingsUI(gui);
// temp->setShowEffect(tgui::ShowEffectType::Scale, 500);
// temp->createSubSectionLabel("Test Section", "Boolean Settings");
// temp->createSetting("Test Section", new BoolSetting{"Bool", true, "A test for boolean settings"});
// temp->createSubSectionLabel("Test Section", "Integer Settings");
// temp->createSetting("Test Section", new IntSetting{"Int1", 1, "A test for int settings with any input"});
// temp->createSetting("Test Section", new IntSetting{"Int2", 2, "A test for int settings with a conditional input (-7 <= int <= 77)", [](int value){ return -7 <= value && value <= 77; }});
// temp->createSetting("Test Section", new IntSetting{"Int3", 3, "A test for int settings with a range slider", -7, 77, 1});
// temp->createSubSectionLabel("Test Section", "Unsigned Integer Settings");
// temp->createSetting("Test Section", new UIntSetting{"UInt1", 1, "A test for unsigned ints settings with any input"});
// temp->createSetting("Test Section", new UIntSetting{"UInt2", 2, "A test for unsigned ints settings with a conditional input (7 <= int)", [](unsigned int value){ return 7 <= value; }});
// temp->createSetting("Test Section", new UIntSetting{"UInt3", 3, "A test for unsigned ints settings with a range slider", 7, 77, 1});
// temp->createSubSectionLabel("Test Section", "Floating Point Settings");
// temp->createSetting("Test Section", new FloatSetting{"Float1", 1.f, "A test for float settings with any input"});
// temp->createSetting("Test Section", new FloatSetting{"Float2", 2.f, "A test for float settings with a conditional input (0.7 <= float <= 7)", [](float value){ return 0.699999 <= value && value <= 7; }});
// temp->createSetting("Test Section", new FloatSetting{"Float3", 3.f, "A test for float settings with a range slider", 0.7, 77, 0.1});
// temp->createSubSectionLabel("Test Section", "String Settings");
// temp->createSetting("Test Section", new StringSetting{"String1", "one", "A test for string settings with any input"});
// temp->createSetting("Test Section", new StringSetting{"String2", "two", "A test for string settings with a list of input options", {"Cat", "Dog", "Coding", "Math", "Physics"}});
// temp->createSetting("Test Section", new StringSetting{"String3", "three", "A test for string settings with a conditional input (string must start with \"I\")", [](std::string value){ return value.starts_with("I"); }});
// temp->createSubSectionLabel("Test Section", "Input Settings");
// temp->createSetting("Test Section", new InputSetting{"Input1", Input::Action::Event{{sf::Keyboard::Key::Num1}}, "A test for input settings with any input and a default of one key"});
// temp->createSetting("Test Section", new InputSetting{"Input2", Input::Action::Event{{sf::Keyboard::Key::Num2}}, "A test for input settings with only keyboard inputs", [](const Input::Action::Event& event){ return event.getMouseButtons().size() == 0; }});
// temp->createSetting("Test Section", new InputSetting{"Input3", Input::Action::Event{{}, {sf::Mouse::Button::Left}}, "A test for input settings with only mouse inputs", [](const Input::Action::Event& event){ return event.getKeyCodes().size() == 0; }});
// temp->createSetting("Test Section", new InputSetting{"Input4", Input::Action::Event{{sf::Keyboard::Key::Num3}}, "A test for input settings that can only have 0 or 1 input", [](const Input::Action::Event& event){ return event.getKeyCodes().size() + event.getMouseButtons().size() == 1; }});
// temp->createSubSectionLabel("Test Section", "Color Settings");
// temp->createSetting("Test Section", new ColorSetting{"Color1", Color{255,0,255,255}, "A test setting for colors"});
// temp->createSetting("Test Section", new ColorSetting{"Color2", Color{255,0,255,255}, "A test setting for colors where color has to have a r value >= 100", [](Color color){ return color.r >= 100; }});
// temp->createSetting("Test Section", new ColorSetting{"Color3", Color{255,0,255,255}, "A test setting for colors where there is a list of colors", {Color{255,255,255,255}, Color{255,0,255,255}, Color{0,0,255,255}}});
// temp->setVisible();

#endif
