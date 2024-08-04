#include "Utils/Settings/Types/FloatSetting.hpp"
#include "Utils/StringHelper.hpp"

FloatSetting::FloatSetting(const std::string& name, float defaultValue, const std::string& description)
{
    m_name = name;
    m_value = defaultValue;
    m_defaultValue = defaultValue;
    m_description = description;
    m_inputValidation = SettingInputValidation::None;
    m_type = "float";
}

FloatSetting::FloatSetting(const std::string& name, float defaultValue, const std::string& description, const std::list<float>& options) : FloatSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::OptionsList;
    m_validator = options;
}

FloatSetting::FloatSetting(const std::string& name, float defaultValue, const std::string& description, bool(*compare)(float)) : FloatSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::CompareFunc;
    m_validator = compare;
}

FloatSetting::FloatSetting(const std::string& name, float defaultValue, const std::string& description, float min, float max, float stepSize) : FloatSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::Range;
    m_validator = std::tuple<float,float,float>(min,max,stepSize);
}

void FloatSetting::setValue(float value)
{
    if (m_inputValidation == SettingInputValidation::Range)
    {
        auto& tuple = std::get<std::tuple<float,float,float>>(m_validator);
        m_value = std::clamp(value, std::get<0>(tuple), std::get<1>(tuple));
    }
    else if (m_inputValidation == SettingInputValidation::OptionsList)
    {
        auto& options = std::get<std::list<float>>(m_validator);

        if (std::find(options.begin(), options.end(), value) != options.end())
        {
            m_value = value;
        }
    }
    else if (m_inputValidation == SettingInputValidation::CompareFunc)
    {
        bool(*compare)(float) = std::get<bool(*)(float)>(m_validator);
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

float FloatSetting::getValue() const
{
    return m_value;
}

float FloatSetting::getDefaultValue() const
{
    return m_defaultValue;
}

std::list<float> FloatSetting::getOptions() const
{
    if (m_inputValidation != SettingInputValidation::OptionsList)
        return {};
    
    return std::get<std::list<float>>(m_validator);
}

std::tuple<float,float,float> FloatSetting::getRangeData() const
{
    if (m_inputValidation != SettingInputValidation::Range)
        return {};
    
    return std::get<std::tuple<float,float,float>>(m_validator);
}

float FloatSetting::getMin() const
{
    if (m_inputValidation != SettingInputValidation::Range)
        return {};
    
    return std::get<0>(std::get<std::tuple<float,float,float>>(m_validator));
}

float FloatSetting::getMax() const
{
    if (m_inputValidation != SettingInputValidation::Range)
        return {};
    
    return std::get<1>(std::get<std::tuple<float,float,float>>(m_validator));
}

float FloatSetting::getStepSize() const
{
    if (m_inputValidation != SettingInputValidation::Range)
        return {};
    
    return std::get<2>(std::get<std::tuple<float,float,float>>(m_validator));
}

std::string FloatSetting::getValueStr(uint8_t decimalRounding) const
{
    return StringHelper::FloatToStringRound(m_value, decimalRounding);
}

std::string FloatSetting::getDefaultValueStr(uint8_t decimalRounding) const
{
    return StringHelper::FloatToStringRound(m_defaultValue, decimalRounding);
}

std::list<std::string> FloatSetting::getOptionsStr(uint8_t decimalRounding) const
{
    if (m_inputValidation != SettingInputValidation::OptionsList)
        return {};
    
    std::list<std::string> list;

    for (float value: std::get<std::list<float>>(m_validator))
        list.emplace_back(StringHelper::FloatToStringRound(value, decimalRounding));

    return list;
}

void FloatSetting::m_onValueSet(const std::string& valueStr)
{
    setValue(StringHelper::toFloat(valueStr, m_defaultValue));
}
