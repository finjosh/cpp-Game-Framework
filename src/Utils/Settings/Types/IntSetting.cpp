#include "Utils/Settings/Types/IntSetting.hpp"
#include "Utils/StringHelper.hpp"
#include <algorithm>

IntSetting::IntSetting(const std::string& name, int defaultValue, const std::string& description)
{
    m_name = name;
    m_value = defaultValue;
    m_defaultValue = defaultValue;
    m_description = description;
    m_inputValidation = SettingInputValidation::None;
    m_type = "int";
}

IntSetting::IntSetting(const std::string& name, int defaultValue, const std::string& description, const std::list<int>& options) : IntSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::OptionsList;
    m_validator = options;
}

IntSetting::IntSetting(const std::string& name, int defaultValue, const std::string& description, bool(*compare)(int)) : IntSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::CompareFunc;
    m_validator = compare;
}

IntSetting::IntSetting(const std::string& name, int defaultValue, const std::string& description, int min, int max, int stepSize) : IntSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::Range;
    m_validator = std::tuple<int,int,int>(min,max,stepSize);
}

void IntSetting::setValue(int value)
{
    if (m_inputValidation == SettingInputValidation::Range)
    {
        auto& tuple = std::get<std::tuple<int,int,int>>(m_validator);
        m_value = std::clamp(value, std::get<0>(tuple), std::get<1>(tuple));
    }
    else if (m_inputValidation == SettingInputValidation::OptionsList)
    {
        auto& options = std::get<std::list<int>>(m_validator);

        if (std::find(options.begin(), options.end(), value) != options.end())
        {
            m_value = value;
        }
    }
    else if (m_inputValidation == SettingInputValidation::CompareFunc)
    {
        bool(*compare)(int) = std::get<bool(*)(int)>(m_validator);
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

int IntSetting::getValue() const
{
    return m_value;
}

int IntSetting::getDefaultValue() const
{
    return m_defaultValue;
}

std::list<int> IntSetting::getOptions() const
{
    if (m_inputValidation != SettingInputValidation::OptionsList)
        return {};
    
    return std::get<std::list<int>>(m_validator);
}

std::tuple<int,int,int> IntSetting::getRangeData() const
{
    if (m_inputValidation != SettingInputValidation::Range)
        return {};
    
    return std::get<std::tuple<int,int,int>>(m_validator);
}

int IntSetting::getMin() const
{
    if (m_inputValidation != SettingInputValidation::Range)
        return {};
    
    return std::get<0>(std::get<std::tuple<int,int,int>>(m_validator));
}

int IntSetting::getMax() const
{
    if (m_inputValidation != SettingInputValidation::Range)
        return {};
    
    return std::get<1>(std::get<std::tuple<int,int,int>>(m_validator));
}

int IntSetting::getStepSize() const
{
    if (m_inputValidation != SettingInputValidation::Range)
        return {};
    
    return std::get<2>(std::get<std::tuple<int,int,int>>(m_validator));
}

std::string IntSetting::getValueStr(uint8_t decimalRounding) const
{
    return std::to_string(m_value); // dont care about rounding because its an int
}

std::string IntSetting::getDefaultValueStr(uint8_t decimalRounding) const
{
    return std::to_string(m_defaultValue); // dont care about rounding because its an int
}

std::list<std::string> IntSetting::getOptionsStr(uint8_t decimalRounding) const
{
    if (m_inputValidation != SettingInputValidation::OptionsList)
            return {};
    
    std::list<std::string> list;

    for (int value: std::get<std::list<int>>(m_validator))
        list.emplace_back(std::to_string(value));

    return list;
}

void IntSetting::m_onValueSet(const std::string& valueStr)
{
    setValue(StringHelper::toInt(valueStr, m_defaultValue));
}
