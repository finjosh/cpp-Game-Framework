#include "Utils/Settings/Types/UIntSetting.hpp"
#include "Utils/StringHelper.hpp"
#include <algorithm>

UIntSetting::UIntSetting(const std::string& name, unsigned int defaultValue, const std::string& description)
{
    m_name = name;
    m_value = defaultValue;
    m_defaultValue = defaultValue;
    m_description = description;
    m_inputValidation = SettingInputValidation::None;
    m_type = "uint";
}

UIntSetting::UIntSetting(const std::string& name, unsigned int defaultValue, const std::string& description, const std::list<unsigned int>& options) : UIntSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::OptionsList;
    m_validator = options;
}

UIntSetting::UIntSetting(const std::string& name, unsigned int defaultValue, const std::string& description, bool(*compare)(unsigned int)) : UIntSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::CompareFunc;
    m_validator = compare;
}

UIntSetting::UIntSetting(const std::string& name, unsigned int defaultValue, const std::string& description, unsigned int min, unsigned int max, unsigned int stepSize) : UIntSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::Range;
    m_validator = std::tuple<unsigned int,unsigned int,unsigned int>(min,max,stepSize);
}

void UIntSetting::setValue(unsigned int value)
{
    if (m_inputValidation == SettingInputValidation::Range)
    {
        auto& tuple = std::get<std::tuple<unsigned int,unsigned int,unsigned int>>(m_validator);
        m_value = std::clamp(value, std::get<0>(tuple), std::get<1>(tuple));
    }
    else if (m_inputValidation == SettingInputValidation::OptionsList)
    {
        auto& options = std::get<std::list<unsigned int>>(m_validator);

        if (std::find(options.begin(), options.end(), value) != options.end())
        {
            m_value = value;
        }
    }
    else if (m_inputValidation == SettingInputValidation::CompareFunc)
    {
        bool(*compare)(unsigned int) = std::get<bool(*)(unsigned int)>(m_validator);
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

unsigned int UIntSetting::getValue() const
{
    return m_value;
}

unsigned int UIntSetting::getDefaultValue() const
{
    return m_defaultValue;
}

std::list<unsigned int> UIntSetting::getOptions() const
{
    if (m_inputValidation != SettingInputValidation::OptionsList)
        return {};
    
    return std::get<std::list<unsigned int>>(m_validator);
}

std::tuple<unsigned int,unsigned int,unsigned int> UIntSetting::getRangeData() const
{
    if (m_inputValidation != SettingInputValidation::Range)
        return {};
    
    return std::get<std::tuple<unsigned int,unsigned int,unsigned int>>(m_validator);
}

unsigned int UIntSetting::getMin() const
{
    if (m_inputValidation != SettingInputValidation::Range)
        return {};
    
    return std::get<0>(std::get<std::tuple<unsigned int,unsigned int,unsigned int>>(m_validator));
}

unsigned int UIntSetting::getMax() const
{
    if (m_inputValidation != SettingInputValidation::Range)
        return {};
    
    return std::get<1>(std::get<std::tuple<unsigned int,unsigned int,unsigned int>>(m_validator));
}

unsigned int UIntSetting::getStepSize() const
{
    if (m_inputValidation != SettingInputValidation::Range)
        return {};
    
    return std::get<2>(std::get<std::tuple<unsigned int,unsigned int,unsigned int>>(m_validator));
}

std::string UIntSetting::getValueStr(uint8_t decimalRounding) const
{
    return std::to_string(m_value); // dont care about rounding because its an unsigned int
}

std::string UIntSetting::getDefaultValueStr(uint8_t decimalRounding) const
{
    return std::to_string(m_defaultValue); // dont care about rounding because its an unsigned int
}

std::list<std::string> UIntSetting::getOptionsStr(uint8_t decimalRounding) const
{
    if (m_inputValidation != SettingInputValidation::OptionsList)
            return {};
    
    std::list<std::string> list;

    for (unsigned int value: std::get<std::list<unsigned int>>(m_validator))
        list.emplace_back(std::to_string(value));

    return list;
}

void UIntSetting::m_onValueSet(const std::string& valueStr)
{
    setValue(StringHelper::toUInt(valueStr, m_defaultValue));
}
