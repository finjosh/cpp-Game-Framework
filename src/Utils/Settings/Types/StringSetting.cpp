#include "Utils/Settings/Types/StringSetting.hpp"
#include "Utils/StringHelper.hpp"
#include <algorithm>

StringSetting::StringSetting(const std::string& name, std::string defaultValue, const std::string& description)
{
    m_name = name;
    m_value = defaultValue;
    m_defaultValue = defaultValue;
    m_description = description;
    m_inputValidation = SettingInputValidation::None;
    m_type = "string";
}

StringSetting::StringSetting(const std::string& name, std::string defaultValue, const std::string& description, const std::list<std::string>& options) : StringSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::OptionsList;
    m_validator = options;
}

StringSetting::StringSetting(const std::string& name, std::string defaultValue, const std::string& description, bool(*compare)(std::string)) : StringSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::CompareFunc;
    m_validator = compare;
}

void StringSetting::setValue(std::string value)
{
    if (m_inputValidation == SettingInputValidation::OptionsList)
    {
        auto& options = std::get<std::list<std::string>>(m_validator);

        if (std::find(options.begin(), options.end(), value) != options.end())
        {
            m_value = value;
        }
    }
    else if (m_inputValidation == SettingInputValidation::CompareFunc)
    {
        bool(*compare)(std::string) = std::get<bool(*)(std::string)>(m_validator);
        if (compare == nullptr) // always true
            m_value = value;
        else if (compare(value) || value == m_defaultValue)
            m_value = value;
    }
    else // input validation == none
    {
        m_value = value;
    }

    onValueSet.invoke(m_value);
    onValueSetStr.invoke(getValueStr());
}

std::string StringSetting::getValue() const
{
    return m_value;
}

std::string StringSetting::getDefaultValue() const
{
    return m_defaultValue;
}

std::list<std::string> StringSetting::getOptions() const
{
    if (m_inputValidation != SettingInputValidation::OptionsList)
        return {};
    
    return std::get<std::list<std::string>>(m_validator);
}

std::string StringSetting::getValueStr(uint8_t decimalRounding) const
{
    return m_value; // dont care about rounding because its an std::string
}

std::string StringSetting::getDefaultValueStr(uint8_t decimalRounding) const
{
    return m_defaultValue; // dont care about rounding because its an std::string
}

std::list<std::string> StringSetting::getOptionsStr(uint8_t decimalRounding) const
{
    if (m_inputValidation != SettingInputValidation::OptionsList)
            return {};
    
    std::list<std::string> list;

    for (std::string value: std::get<std::list<std::string>>(m_validator))
        list.emplace_back(value);

    return list;
}

void StringSetting::m_onValueSet(const std::string& valueStr)
{
    setValue(valueStr);
}
