#include "Utils/Settings/Types/InputSetting.hpp"

InputSetting::InputSetting(const std::string& name, Input::Action::Event defaultValue, const std::string& description)
{
    m_name = name;
    m_value = defaultValue;
    m_defaultValue = defaultValue;
    m_description = description;
    m_inputValidation = SettingInputValidation::None;
    m_type = "input_action_event";
}

InputSetting::InputSetting(const std::string& name, Input::Action::Event defaultValue, const std::string& description, bool(*compare)(const Input::Action::Event&)) : InputSetting(name, defaultValue, description)
{
    m_inputValidation = SettingInputValidation::CompareFunc;
    m_validator = compare;
}

void InputSetting::setValue(const Input::Action::Event& value)
{
    if (m_inputValidation == SettingInputValidation::CompareFunc)
    {
        bool(*compare)(const Input::Action::Event&) = std::get<bool(*)(const Input::Action::Event&)>(m_validator);
        if (compare == nullptr) // always true
            m_value = value;
        else if (compare(value))
            m_value = value;
    }
    else // input validation == none
    {
        m_value = value;
    }

    onValueSet.invoke(m_value);
    onValueSetStr.invoke(getValueStr());
}

Input::Action::Event InputSetting::getValue() const
{
    return m_value;
}

Input::Action::Event InputSetting::getDefaultValue() const
{
    return m_defaultValue;
}

std::string InputSetting::getValueStr(uint8_t decimalRounding) const
{
    return m_value.toString();
}

std::string InputSetting::getDefaultValueStr(uint8_t decimalRounding) const
{
    return m_defaultValue.toString();
}

std::list<std::string> InputSetting::getOptionsStr(uint8_t decimalRounding) const
{
    return {}; // no implementation for option list with InputSetting
}

void InputSetting::m_onValueSet(const std::string& valueStr)
{
    setValue(Input::fromString_Action_Event(valueStr));
}
