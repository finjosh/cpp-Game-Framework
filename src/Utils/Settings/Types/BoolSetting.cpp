#include "Utils/Settings/Types/BoolSetting.hpp"
#include "Utils/StringHelper.hpp"

BoolSetting::BoolSetting(const std::string& name, bool defaultValue, const std::string& description)
{
    m_name = name;
    m_value = defaultValue;
    m_defaultValue = defaultValue;
    m_description = description;
    m_inputValidation = SettingInputValidation::None;
    m_type = "bool";
}

BoolSetting::BoolSetting(const std::string& name, bool defaultValue, const std::string& description, bool(*compare)(bool)) : BoolSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::CompareFunc;
    m_validator = compare;
}

void BoolSetting::setValue(bool value)
{
    if (m_inputValidation == SettingInputValidation::CompareFunc)
    {
        bool(*compare)(bool) = std::get<bool(*)(bool)>(m_validator);
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

bool BoolSetting::getValue() const
{
    return m_value;
}

bool BoolSetting::getDefaultValue() const
{
    return m_defaultValue;
}

std::string BoolSetting::getValueStr(uint8_t decimalRounding) const
{
    return m_value ? "True" : "False"; // dont care about rounding because its an bool
}

std::string BoolSetting::getDefaultValueStr(uint8_t decimalRounding) const
{
    return m_defaultValue ? "True" : "False"; // dont care about rounding because its an bool
}

std::list<std::string> BoolSetting::getOptionsStr(uint8_t decimalRounding) const
{
    return {}; // because a bool setting never has an options list for a validator
}

void BoolSetting::m_onValueSet(const std::string& valueStr)
{
    setValue(StringHelper::toBool(valueStr, m_defaultValue));
}
