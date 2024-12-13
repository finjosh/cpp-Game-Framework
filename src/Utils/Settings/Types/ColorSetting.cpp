#include "Utils/Settings/Types/ColorSetting.hpp"
#include <algorithm>

ColorSetting::ColorSetting(const std::string& name, Color defaultValue, const std::string& description)
{
    m_name = name;
    m_value = defaultValue;
    m_defaultValue = defaultValue;
    m_description = description;
    m_inputValidation = SettingInputValidation::None;
    m_type = "color";
}

ColorSetting::ColorSetting(const std::string& name, Color defaultValue, const std::string& description, const std::list<Color>& options) : ColorSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::OptionsList;
    m_validator = options;
}

ColorSetting::ColorSetting(const std::string& name, Color defaultValue, const std::string& description, bool(*compare)(Color)) : ColorSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::CompareFunc;
    m_validator = compare;
}

void ColorSetting::setValue(Color value)
{
    if (m_inputValidation == SettingInputValidation::OptionsList)
    {
        auto& options = std::get<std::list<Color>>(m_validator);

        if (std::find(options.begin(), options.end(), value) != options.end())
        {
            m_value = value;
        }
    }
    else if (m_inputValidation == SettingInputValidation::CompareFunc)
    {
        bool(*compare)(Color) = std::get<bool(*)(Color)>(m_validator);
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

Color ColorSetting::getValue() const
{
    return m_value;
}

Color ColorSetting::getDefaultValue() const
{
    return m_defaultValue;
}

std::list<Color> ColorSetting::getOptions() const
{
    if (m_inputValidation != SettingInputValidation::OptionsList)
        return {};
    
    return std::get<std::list<Color>>(m_validator);
}

std::string ColorSetting::getValueStr(uint8_t decimalRounding) const
{
    return m_value.toString();
}

std::string ColorSetting::getDefaultValueStr(uint8_t decimalRounding) const
{
    return m_defaultValue.toString();
}

std::list<std::string> ColorSetting::getOptionsStr(uint8_t decimalRounding) const
{
    if (m_inputValidation != SettingInputValidation::OptionsList)
        return {};
    
    std::list<std::string> list;

    for (Color value: std::get<std::list<Color>>(m_validator))
        list.emplace_back(value.toString());

    return list;
}

void ColorSetting::m_onValueSet(const std::string& valueStr)
{
    setValue(Color::fromString(valueStr, m_defaultValue));
}
