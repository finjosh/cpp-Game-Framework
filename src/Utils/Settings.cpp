#include "Utils/Settings.hpp"
#include "Utils/iniParser.hpp"
#include <filesystem>

Settings::~Settings()
{
    for (auto section: m_settings)
    {
        for (auto setting: section.second)
        {
            setting->Delete(); // making sure to clean up all memory used by the settings classes
        }
    }
}

bool Settings::loadFromFile(const std::string& fileName, const std::list<std::string>& directories)
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
            setting->setValueStr(fileData.getValue(section.first, setting->getName()));
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
            fileData.addValue(section.first, setting->getName(), setting->getValueStr());
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

const std::set<SettingBase*, _SettingBaseComp>* Settings::getSection(const std::string& section) const
{
    auto sectionIter = m_settings.find(section);
    if (sectionIter == m_settings.cend())
        return nullptr;
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

SettingBase* Settings::getSetting(const std::string& section, const std::string& name)
{
    auto settings = m_getSection(section);
    if (settings == nullptr)
        return nullptr;
    else
    {
        Setting<bool> temp(name, false, "");
        auto setting = settings->find(&temp);
        if (setting == settings->end())
            return nullptr;
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
