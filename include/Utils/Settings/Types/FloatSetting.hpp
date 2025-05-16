#ifndef FLOAT_SETTING_HPP
#define FLOAT_SETTING_HPP

#pragma once

#include "Utils/Settings/SettingBase.hpp"
#include <variant>

class FloatSetting : public SettingBase
{
public:
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    FloatSetting(const std::string& name, float defaultValue, const std::string& description);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param options the available options for this setting
    /// @note if any options are given then the input must be in the list
    FloatSetting(const std::string& name, float defaultValue, const std::string& description, const std::list<float>& options);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param compare a function that will return if the value is valid
    FloatSetting(const std::string& name, float defaultValue, const std::string& description, bool(*compare)(float));
    /// @param name name of the setting
    /// @param description description of the setting
    /// @param min value of the setting
    /// @param max value of the setting
    /// @param stepSize the stepSize that UI should use
    FloatSetting(const std::string& name, float defaultValue, const std::string& description, float min, float max, float stepSize);
    void setValue(float value);
    float getValue() const;
    float getDefaultValue() const;
    std::list<float> getOptions() const;
    std::tuple<float,float,float> getRangeData() const;
    float getMin() const;
    float getMax() const;
    float getStepSize() const;

    virtual std::string getValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::string getDefaultValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::list<std::string> getOptionsStr(uint8_t decimalRounding = 6) const override;

    /// @brief called when ever the value is changed with the specific type
    /// @note called even if the value is set to the same thing
    /// @note Optional parameter: the new value
    EventHelper::EventDynamic<float> onValueSet;

protected:
    virtual void m_onValueSet(const std::string& valueStr) override;

    float m_value;
    float m_defaultValue;
    std::variant<std::list<float>, std::tuple<float,float,float>, bool(*)(float)> m_validator;
private:

};

#endif
