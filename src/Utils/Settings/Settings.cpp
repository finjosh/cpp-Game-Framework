#include "Utils/Settings/Settings.hpp"
#include "Utils/iniParser.hpp"
#include <filesystem>
#include <cassert>

Settings::~Settings()
{
    for (auto section: m_settings)
    {
        for (auto setting: section.second)
        {
            delete(setting); // making sure to clean up all memory used by the settings classes
        }
    }
}

bool Settings::tryLoadFromFile(const std::string& fileName, const std::list<std::string>& directories)
{
    std::string path = "";
    for (const std::string& dir: directories)
    {
        if (std::filesystem::exists(dir + fileName))
        {
            path = dir + fileName;
            break;
        }
    }

    if (path == "" && directories.size() != 0)
        return false;
    else
        path = fileName;

    iniParser fileData(path);
    if (!fileData.isOpen())
        return false;
    fileData.setAutoSave(false);
    if (!fileData.loadData())
        return true; // still returning true as the file was found (just skipping any attempt to load since there is no data)

    for (auto section: m_settings)
    {
        for (auto setting: section.second)
        {
            const std::string* value = fileData.getValue(section.first, setting->getName());
            if (value == nullptr)
            {
                continue;
            }
            setting->setValueStr(*value);
        }
    }

    return true;
}

void Settings::save(const std::string& file) const
{
    iniParser fileData;
    if (!fileData.setFile(file))
    {
        fileData.createFile(file);
        fileData.setFile(file);
    }
    fileData.overrideData();
    
    for (auto section: m_settings)
    {
        fileData.addSection(section.first);
        for (auto setting: section.second)
        {
            fileData.setValue(section.first, setting->getName(), setting->getValueStr());
        }
    }

    fileData.save();
}

void Settings::createSection(const std::string& section)
{
    if (m_settings.insert({section, {}}).second)
    {
        onSectionCreated.invoke(section);
    }
}

SettingBase* Settings::createSetting(const std::string& section, SettingBase* setting)
{
    auto settings = m_settings.find(section); // getting/creating the section and getting its set
    if (settings == m_settings.end())
    {
        createSection(section);
        settings = m_settings.find(section);
    }

    auto temp = settings->second.emplace(setting); 
    if (temp.second) // returning if the setting was added or already existed
    {
        onSettingCreated.invoke(section, *temp.first);
        return setting;
    }

    delete(setting);
    return nullptr;
}

const std::set<SettingBase*, _SettingBaseComp>* Settings::getSection(const std::string& section) const
{
    auto sectionIter = m_settings.find(section);
    if (sectionIter == m_settings.cend())
    {
        assert(section == "Section does not exist");
        return nullptr;
    }
    else
        return &(sectionIter->second);
}

std::list<std::string> Settings::getSections() const
{
    std::list<std::string> list;
    for (auto pair: m_settings)
    {
        list.emplace_back(pair.first);
    }
    return list;
}

std::list<std::string> Settings::getSettings(const std::string& section) const
{
    auto sectionIter = m_settings.find(section);
    if (sectionIter == m_settings.end())
        return {};

    std::list<std::string> list;
    for (auto setting: sectionIter->second)
    {
        list.emplace_back(setting->getName());
    }
    return list;
}

#include "Utils/Settings/Types/BoolSetting.hpp" // used for comparing settings
SettingBase* Settings::getSetting(const std::string& section, const std::string& name)
{
    auto settings = m_getSection(section);
    if (settings == nullptr)
    {
        assert(section == "Section does not exist");
        return nullptr;
    }
    else
    {
        BoolSetting temp(name, false, "");
        auto setting = settings->find(&temp);
        if (setting == settings->end())
        {
            assert(section + " " + name == "Setting does not exist");
            return nullptr;
        }
        else
            return (*setting);
    }
}

std::set<SettingBase*, _SettingBaseComp>* Settings::m_getSection(const std::string& section)
{
    auto sectionIter = m_settings.find(section);
    if (sectionIter == m_settings.end())
        return nullptr;
    else
        return &(sectionIter->second);
}
