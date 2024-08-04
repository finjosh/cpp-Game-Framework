#ifndef INPUT_SETTING_HPP
#define INPUT_SETTING_HPP

#pragma once

#include "SFML/Window/Event.hpp"

#include "Utils/Settings/SettingBase.hpp"
#include "Input.hpp"

#include <variant>

class InputSetting : public SettingBase
{
public:
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    InputSetting(const std::string& name, Input::Action::Event defaultValue, const std::string& description);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param compare a function that will return if the value is valid
    InputSetting(const std::string& name, Input::Action::Event defaultValue, const std::string& description, bool(*compare)(const Input::Action::Event&));
    void setValue(const Input::Action::Event& value);
    Input::Action::Event getValue() const;
    Input::Action::Event getDefaultValue() const;

    virtual std::string getValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::string getDefaultValueStr(uint8_t decimalRounding = 6) const override;
    /// @warning this will always return an empty list as input setting does not have any implementation for a option list
    virtual std::list<std::string> getOptionsStr(uint8_t decimalRounding = 6) const override;

    /// @brief called when ever the value is changed with the specific type
    /// @note called even if the value is set to the same thing
    /// @note Optional parameter: the new value
    EventHelper::EventDynamic<Input::Action::Event> onValueSet;

protected:
    virtual void m_onValueSet(const std::string& valueStr) override;

    Input::Action::Event m_value;
    Input::Action::Event m_defaultValue;
    std::variant<std::list<Input::Action::Event>, bool(*)(const Input::Action::Event&)> m_validator;
private:

};

#endif
