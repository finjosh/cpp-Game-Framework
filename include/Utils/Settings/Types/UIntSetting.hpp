#ifndef UINT_SETTING_HPP
#define UINT_SETTING_HPP

#pragma once

#include "Utils/Settings/SettingBase.hpp"
#include <variant>

class UIntSetting : public SettingBase
{
public:
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    UIntSetting(const std::string& name, unsigned int defaultValue, const std::string& description);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param options the available options for this setting
    /// @note if any options are given then the input must be in the list
    UIntSetting(const std::string& name, unsigned int defaultValue, const std::string& description, const std::list<unsigned int>& options);
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param compare a function that will return if the value is valid
    UIntSetting(const std::string& name, unsigned int defaultValue, const std::string& description, bool(*compare)(unsigned int));
    /// @param name name of the setting
    /// @param description description of the setting
    /// @param min value of the setting
    /// @param max value of the setting
    /// @param stepSize the stepSize that UI should use
    UIntSetting(const std::string& name, unsigned int defaultValue, const std::string& description, unsigned int min, unsigned int max, unsigned int stepSize);
    void setValue(unsigned int value);
    unsigned int getValue() const;
    unsigned int getDefaultValue() const;
    std::list<unsigned int> getOptions() const;
    std::tuple<unsigned int,unsigned int,unsigned int> getRangeData() const;
    unsigned int getMin() const;
    unsigned int getMax() const;
    unsigned int getStepSize() const;

    virtual std::string getValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::string getDefaultValueStr(uint8_t decimalRounding = 6) const override;
    virtual std::list<std::string> getOptionsStr(uint8_t decimalRounding = 6) const override;

    /// @brief called when ever the value is changed with the specific type
    /// @note called even if the value is set to the same thing
    /// @note Optional parameter: the new value
    EventHelper::EventDynamic<int> onValueSet;

protected:
    virtual void m_onValueSet(const std::string& valueStr) override;

    unsigned int m_value;
    unsigned int m_defaultValue;
    std::variant<std::list<unsigned int>, std::tuple<unsigned int,unsigned int,unsigned int>, bool(*)(unsigned int)> m_validator;
private:

};

#endif
