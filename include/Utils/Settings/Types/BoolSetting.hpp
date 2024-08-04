#ifndef BOOL_SETTING_HPP
#define BOOL_SETTING_HPP

#pragma once

#include "Utils/Settings/SettingBase.hpp"
#include <variant>

class BoolSetting : public SettingBase
{
public:
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    BoolSetting(const std::string& name, bool defaultValue, const std::string& description);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param compare a function that will return if the value is valid
    BoolSetting(const std::string& name, bool defaultValue, const std::string& description, bool(*compare)(bool));
    void setValue(bool value);
    bool getValue() const;
    bool getDefaultValue() const;

    virtual std::string getValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::string getDefaultValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::list<std::string> getOptionsStr(uint8_t decimalRounding = 6) const override;

    /// @brief called when ever the value is changed with the specific type
    /// @note called even if the value is set to the same thing
    /// @note Optional parameter: the new value
    EventHelper::EventDynamic<bool> onValueSet;

protected:
    virtual void m_onValueSet(const std::string& valueStr) override;

    bool m_value;
    bool m_defaultValue;
    std::variant<bool(*)(bool)> m_validator;
private:

};

#endif
