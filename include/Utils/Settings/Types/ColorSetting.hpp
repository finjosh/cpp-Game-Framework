#ifndef COLOR_SETTING_HPP
#define COLOR_SETTING_HPP

#pragma once

#include "Utils/Settings/SettingBase.hpp"
#include "Color.hpp"
#include <variant>

class ColorSetting : public SettingBase
{
public:
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    ColorSetting(const std::string& name, Color defaultValue, const std::string& description);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param options the available options for this setting
    /// @note if any options are given then the input must be in the list
    ColorSetting(const std::string& name, Color defaultValue, const std::string& description, const std::list<Color>& options);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param compare a function that will return if the value is valid
    ColorSetting(const std::string& name, Color defaultValue, const std::string& description, bool(*compare)(Color));
    void setValue(Color value);
    Color getValue() const;
    Color getDefaultValue() const;
    std::list<Color> getOptions() const;

    virtual std::string getValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::string getDefaultValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::list<std::string> getOptionsStr(uint8_t decimalRounding = 6) const override;

    /// @brief called when ever the value is changed with the specific type
    /// @note called even if the value is set to the same thing
    /// @note Optional parameter: the new value
    EventHelper::EventDynamic<Color> onValueSet;

protected:
    virtual void m_onValueSet(const std::string& valueStr) override;

    Color m_value;
    Color m_defaultValue;
    std::variant<std::list<Color>, bool(*)(Color)> m_validator;
};

#endif
