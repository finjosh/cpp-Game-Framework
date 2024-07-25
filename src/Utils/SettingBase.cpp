#include "Utils/SettingBase.hpp"

std::string SettingBase::getName() const
{
    return m_name;
}

std::string SettingBase::getDescription() const
{
    return m_description;
}

void SettingBase::setValueStr(const std::string& value)
{
    m_onValueSet(value); 
    onValueSetStr.invoke(getValueStr()); // we want to know what the new str is after checking if input is valid
}

SettingType SettingBase::getType() const
{
    return m_type;
}

SettingInputValidation SettingBase::getInputValidation() const
{
    return m_inputValidation;
}

bool SettingBase::operator < (const SettingBase& settingBase) const
{
    return m_name < settingBase.m_name;
}

bool SettingBase::operator > (const SettingBase& settingBase) const
{
    return m_name > settingBase.m_name;
}

bool SettingBase::operator == (const SettingBase& settingBase) const
{
    return m_name == settingBase.m_name;
}

bool _SettingBaseComp::operator ()(const SettingBase* lhs, const SettingBase* rhs) const
{
    return lhs->getName() < rhs->getName();
}
