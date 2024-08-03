#ifndef SETTING_HPP
#define SETTING_HPP

#pragma once

#include "Utils/Settings/SettingBase.hpp"
#include "Utils/StringHelper.hpp"
#include <list>
#include <variant>

template <typename T, typename std::enable_if<std::is_same_v<T, int> || std::is_same_v<T, unsigned int> || std::is_same_v<T, float>
                                              || std::is_same_v<T, std::string> || std::is_same_v<T, bool>>* = nullptr>
class Setting : public SettingBase
{
public:
    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    inline Setting(const std::string& name, T defaultValue, const std::string& description)
    {
        m_name = name;
        m_value = defaultValue;
        m_defaultValue = defaultValue;
        m_description = description;
        m_inputValidation = SettingInputValidation::None;
        m_setType();
    }

    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param options the available options for this setting
    /// @note if any options are given then the input must be in the list
    inline Setting(const std::string& name, T defaultValue, const std::string& description, const std::list<T>& options) : Setting(name, defaultValue, description)
    {
        m_validator = options;
        m_inputValidation = SettingInputValidation::OptionsList;
    }

    /// @param name the name of the setting
    /// @param description the description of the setting
    /// @param defaultValue the default value of the description
    /// @param compare a function that will return if the value is valid
    inline Setting(const std::string& name, T defaultValue, const std::string& description, bool(*compare)(T)) : Setting(name, defaultValue, description)
    {
        m_validator = compare;
        m_inputValidation = SettingInputValidation::CompareFunc;
    }

    /// @param name name of the setting
    /// @param description description of the setting
    /// @param min value of the setting
    /// @param max value of the setting
    /// @param stepSize the stepSize that UI should use
    template <typename std::enable_if<std::is_same_v<T, int> || std::is_same_v<T, unsigned int> || std::is_same_v<T, float>>* = nullptr>
    inline Setting(const std::string& name, T defaultValue, const std::string& description, T min, T max, T stepSize) : Setting(name, defaultValue, description)
    {
        m_validator = std::tuple<T,T,T>(min,max,stepSize);
        m_inputValidation = SettingInputValidation::Range;
    }

    /// @brief if this was created with a non empty options list then the value will only be set IF the value is in the list
    /// @param value 
    inline void setValue(T value)
    {
        if (m_inputValidation == SettingInputValidation::Range)
        {
            auto& tuple = std::get<std::tuple<T,T,T>>(m_validator);
            m_value = std::clamp(value, std::get<0>(tuple), std::get<1>(tuple));
        }
        else if (m_inputValidation == SettingInputValidation::OptionsList)
        {
            auto& options = std::get<std::list<T>>(m_validator);

            if (std::find(options.begin(), options.end(), value) != options.end())
            {
                m_value = value;
            }
        }
        else if (m_inputValidation == SettingInputValidation::CompareFunc)
        {
            bool(*compare)(T) = std::get<bool(*)(T)>(m_validator);
            if (compare == nullptr) // always true
                m_value = value;
            else if (compare(value) || value == m_defaultValue)
                m_value = value;
        }
        else // input validation == none
        {
            m_value = value;
        }

        onValueSet.invoke(m_value);
    }

    inline T getValue() const
    {
        return m_value;
    }

    inline T getDefaultValue() const
    {
        return m_defaultValue;
    }

    inline std::string getValueStr(uint8_t decimalRounding = 6) const override
    {
        if constexpr (std::is_same_v<T, std::string>)
            return m_value;
        else if constexpr (std::is_same_v<T, bool>)
            return m_value ? "True" : "False";
        else if constexpr (std::is_same_v<T, float>)
            return StringHelper::FloatToStringRound(m_value, decimalRounding);
        else
            return std::to_string(m_value);
    }

    inline std::string getDefaultValueStr(uint8_t decimalRounding = 6) const override
    {
        if constexpr (std::is_same_v<T, std::string>)
            return m_defaultValue;
        else if constexpr (std::is_same_v<T, bool>)
            return m_defaultValue ? "True" : "False";
        else if constexpr (std::is_same_v<T, float>)
            return StringHelper::FloatToStringRound(m_defaultValue, decimalRounding);
        else
            return std::to_string(m_defaultValue);
    }

    inline std::list<std::string> getOptionsStr(uint8_t decimalRounding = 6) const override
    {
        if (m_inputValidation != SettingInputValidation::OptionsList)
            return {};
        std::list<std::string> list;

        for (T value: std::get<std::list<T>>(m_validator))
            if constexpr (std::is_same_v<T, std::string>)
                list.emplace_back(value);
            else if constexpr (std::is_same_v<T, float>)
                list.emplace_back(StringHelper::FloatToStringRound(value, decimalRounding));
            else
                list.emplace_back(std::to_string(value));

        return list;
    }

    /// @note if this input validation is not options list then the list will be empty
    /// @return a list of all options
    inline std::list<T> getOptions() const
    {
        if (m_inputValidation != SettingInputValidation::OptionsList)
            return {};

        return std::get<std::list<T>>(m_validator);
    }

    /// @note if the input validation type is not Range then the data stored will be invalid
    /// @returns the tuple storing min, max, and stepSize in order
    inline std::tuple<T,T,T> getRangeData() const
    {
        if (m_inputValidation != SettingInputValidation::Range)
            return std::tuple<T,T,T>();
        
        return std::get<std::tuple<T,T,T>>(m_validator);
    }

    /// @note if the input validation type is not Range then the data stored will be invalid
    /// @returns the min value for this setting
    inline T getMin() const
    {
        if (m_inputValidation != SettingInputValidation::Range)
            return T();
        
        return std::get<0>(std::get<std::tuple<T,T,T>>(m_validator));
    }

    /// @note if the input validation type is not Range then the data stored will be invalid
    /// @returns the max value for this setting
    inline T getMax() const
    {
        if (m_inputValidation != SettingInputValidation::Range)
            return T();
        
        return std::get<1>(std::get<std::tuple<T,T,T>>(m_validator));
    }

    /// @note if the input validation type is not Range then the data stored will be invalid
    /// @returns the stepSize for this setting
    inline T getStepSize() const
    {
        if (m_inputValidation != SettingInputValidation::Range)
            return T();
        
        return std::get<2>(std::get<std::tuple<T,T,T>>(m_validator));
    }

    /// @brief called when ever the value is changed with the specific type
    /// @note called even if the value is set to the same thing
    /// @note Optional parameter: the new value
    EventHelper::EventDynamic<T> onValueSet;

protected:
    inline void m_onValueSet(const std::string& valueStr) override
    {
        if constexpr (std::is_same_v<T, int>)
        {
            setValue(StringHelper::toInt(valueStr, m_defaultValue));
        }
        else if constexpr (std::is_same_v<T, unsigned int>)
        {
            setValue(StringHelper::toUInt(valueStr, m_defaultValue));
        }
        else if constexpr (std::is_same_v<T, float>)
        {
            setValue(StringHelper::toFloat(valueStr, m_defaultValue));
        }
        else if constexpr (std::is_same_v<T, std::string>)
        {
            setValue(valueStr);
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            setValue(StringHelper::toBool(valueStr, m_defaultValue));
        }
    }

    inline void Delete()
    {
        delete(this);
    }

    T m_value;
    T m_defaultValue;
    std::variant<std::list<T>, std::tuple<T,T,T>, bool(*)(T)> m_validator;

private:
    inline void m_setType()
    {
        if constexpr (std::is_same_v<T, int>)
        {
            m_type = SettingType::Int;
        }
        else if constexpr (std::is_same_v<T, unsigned int>)
        {
            m_type = SettingType::UInt;
        }
        else if constexpr (std::is_same_v<T, float>)
        {
            m_type = SettingType::Float;
        }
        else if constexpr (std::is_same_v<T, std::string>)
        {
            m_type = SettingType::String;
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            m_type = SettingType::Bool;
        }
    }
};

#endif
