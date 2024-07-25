#ifndef SETTING_BASE_HPP
#define SETTING_BASE_HPP

#pragma once

#include <string>
#include "Utils/EventHelper.hpp"

class Settings;

enum SettingType
{
    Int = 0,
    UInt,
    Float,
    Bool,
    String
};

enum SettingInputValidation
{
    None = 0,
    CompareFunc,
    OptionsList,
    Range
};

class SettingBase
{
public:
    std::string getName() const;
    std::string getDescription() const;
    void setValueStr(const std::string& value);
    /// @param decimalRounding if the stored value is a floating point type how many decimals to round to
    virtual std::string getValueStr(uint8_t decimalRounding = 6) const = 0;
    /// @param decimalRounding if the stored value is a floating point type how many decimals to round to
    virtual std::string getDefaultValueStr(uint8_t decimalRounding = 6) const = 0;
    /// @note if the input validation type is not options list the list will be empty
    /// @param decimalRounding if the stored value is a floating point type how many decimals to round to
    /// @returns converts the stored list into a list of strings for input options
    virtual std::list<std::string> getOptionsStr(uint8_t decimalRounding = 6) const = 0;
    SettingType getType() const;
    /// @returns the type of input validation that is used
    SettingInputValidation getInputValidation() const;

    bool operator < (const SettingBase& settingBase) const;
    bool operator > (const SettingBase& settingBase) const;
    bool operator == (const SettingBase& settingBase) const;

    /// @brief called whenever the string value is changed
    /// @note the value could still be the same after it is set
    /// @note Optional parameter: the "new" string value
    EventHelper::EventDynamic<std::string> onValueSetStr;

protected:
    friend Settings;

    virtual void m_onValueSet(const std::string& valueStr) = 0;
    virtual void Delete() = 0;

    std::string m_name;
    /// @brief value stored in a string
    // std::string m_valueStr;
    std::string m_description;
    SettingType m_type;
    SettingInputValidation m_inputValidation = SettingInputValidation::None;

private:

};

class _SettingBaseComp
{
public:
    bool operator ()(const SettingBase* lhs, const SettingBase* rhs) const;
};

#endif
