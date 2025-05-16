#ifndef STRING_SETTING_HPP
#define STRING_SETTING_HPP

#pragma once

#include "Utils/Settings/SettingBase.hpp"
#include <variant>

class StringSetting : public SettingBase
{
public:
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    StringSetting(const std::string& name, std::string defaultValue, const std::string& description);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param options the available options for this setting
    /// @note if any options are given then the input must be in the list
    StringSetting(const std::string& name, std::string defaultValue, const std::string& description, const std::list<std::string>& options);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param compare a function that will return if the value is valid
    StringSetting(const std::string& name, std::string defaultValue, const std::string& description, bool(*compare)(std::string));
    void setValue(std::string value);
    std::string getValue() const;
    std::string getDefaultValue() const;
    std::list<std::string> getOptions() const;

    virtual std::string getValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::string getDefaultValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::list<std::string> getOptionsStr(uint8_t decimalRounding = 6) const override;

    /// @brief called when ever the value is changed with the specific type
    /// @note called even if the value is set to the same thing
    /// @note Optional parameter: the new value
    EventHelper::EventDynamic<std::string> onValueSet;

protected:
    virtual void m_onValueSet(const std::string& valueStr) override;

    std::string m_value;
    std::string m_defaultValue;
    std::variant<std::list<std::string>, bool(*)(std::string)> m_validator;
private:

};

#endif
