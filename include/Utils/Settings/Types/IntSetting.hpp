#ifndef INint_SEintintING_HPP
#define INint_SEintintING_HPP

#pragma once

#include "Utils/Settings/SettingBase.hpp"
#include <variant>

class IntSetting : public SettingBase
{
public:
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    IntSetting(const std::string& name, int defaultValue, const std::string& description);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param options the available options for this setting
    /// @note if any options are given then the input must be in the list
    IntSetting(const std::string& name, int defaultValue, const std::string& description, const std::list<int>& options);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param compare a function that will return if the value is valid
    IntSetting(const std::string& name, int defaultValue, const std::string& description, bool(*compare)(int));
    /// @param name name of the setting
    /// @param description description of the setting
    /// @param min value of the setting
    /// @param max value of the setting
    /// @param stepSize the stepSize that UI should use
    IntSetting(const std::string& name, int defaultValue, const std::string& description, int min, int max, int stepSize);
    void setValue(int value);
    int getValue() const;
    int getDefaultValue() const;
    std::list<int> getOptions() const;
    std::tuple<int,int,int> getRangeData() const;
    int getMin() const;
    int getMax() const;
    int getStepSize() const;

    virtual std::string getValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::string getDefaultValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::list<std::string> getOptionsStr(uint8_t decimalRounding = 6) const override;

    /// @brief called when ever the value is changed with the specific type
    /// @note called even if the value is set to the same thing
    /// @note Optional parameter: the new value
    EventHelper::EventDynamic<int> onValueSet;

protected:
    virtual void m_onValueSet(const std::string& valueStr) override;

    int m_value;
    int m_defaultValue;
    std::variant<std::list<int>, std::tuple<int,int,int>, bool(*)(int)> m_validator;
private:

};

#endif
