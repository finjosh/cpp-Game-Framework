#ifndef GAME_SETTINGS_HPP
#define GAME_SETTINGS_HPP

#pragma once

#include <string>
#include <set>
#include <map>
#include <list>

#include "Utils/EventHelper.hpp"
#include "Utils/Setting.hpp"

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
    bool loadFromFile(const std::string& fileName = "Settings.ini", const std::list<std::string>& directories = {});
    /// @brief saves the settings to file 
    /// @param file the path and name of the file
    /// @note if the path does not exists it will first be created
    void save(const std::string& file = "Settings.ini") const;
    /// @brief only creates section if no section with the given name already exists
    void createSection(const std::string& section);
    /// @note Setting is ONLY compared by name NOT type
    /// @note if the setting already exists does NOT replace
    /// @note creates section if it does not exist
    /// @returns false if setting with the same name and section already exists
    template <typename T>
    inline bool createSetting(const std::string& section, const Setting<T>& setting)
    {
        auto settings = m_settings.find(section); // getting/creating the section and getting its set
        if (settings == m_settings.end())
        {
            createSection(section);
            settings = m_settings.find(section);
        }

        auto temp = settings->second.emplace((SettingBase*)(new Setting<T>(setting))); 
        if (temp.second) // returning if the setting was added or already existed
        {
            onSettingCreated.invoke(section, *temp.first);
            return true;
        }
        return false;
    }
    /// @brief tries to find the given section and returns the set of all settings in the set
    /// @returns nullptr if the section is not found
    const std::set<SettingBase*, _SettingBaseComp>* getSection(const std::string& section) const;
    /// @returns a list of all the section names
    std::list<std::string> getSections() const;
    /// @param section the section the setting is in
    /// @param name the name of the setting
    /// @returns nullptr if not found 
    SettingBase* getSetting(const std::string& section, const std::string& name);
    /// @param section the section the setting is in
    /// @param name the name of the setting
    /// @tparam the setting type that is wanted
    /// @note this tries to case the found section base into a setting of the given type
    /// @returns nullptr if not found or not convertible
    template <typename T>
    inline Setting<T>* getSetting(const std::string& section, const std::string& name)
    {
        auto base = getSetting(section, name);
        if (base)
        {
            try
            {
                return dynamic_cast<Setting<T>*>(base);
            }
            catch(const std::exception& e)
            {
                return nullptr;
            }
        }
        
        return nullptr;
    }

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

#endif
