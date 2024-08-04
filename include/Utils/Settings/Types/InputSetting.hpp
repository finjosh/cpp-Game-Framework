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
    InputSetting(const std::string& name, Input::Action defaultValue, const std::string& description);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param options the available options for this setting
    /// @note if any options are given then the input must be in the list
    InputSetting(const std::string& name, Input::Action defaultValue, const std::string& description, const std::list<Input::Action>& options);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param compare a function that will return if the value is valid
    InputSetting(const std::string& name, Input::Action defaultValue, const std::string& description, bool(*compare)(Input::Action));
    void setValue(Input::Action value);
    Input::Action getValue() const;
    Input::Action getDefaultValue() const;
    std::list<Input::Action> getOptions() const;

    virtual std::string getValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::string getDefaultValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::list<std::string> getOptionsStr(uint8_t decimalRounding = 6) const override;

    /// @brief called when ever the value is changed with the specific type
    /// @note called even if the value is set to the same thing
    /// @note Optional parameter: the new value
    EventHelper::EventDynamic<Input::Action> onValueSet;

protected:
    virtual void m_onValueSet(const std::string& valueStr) override;

    Input::Action m_value;
    Input::Action m_defaultValue;
    std::variant<std::list<Input::Action>, bool(*)(Input::Action)> m_validator;
private:

};

#endif
