#ifndef SETTINGS_UI_HPP
#define SETTINGS_UI_HPP

#pragma once

#include "Utils/Settings.hpp"
#include "Object.hpp"
#include "Graphics/Canvas.hpp"

#include "TGUI/Container.hpp"
#include "TGUI/Widgets/HorizontalWrap.hpp"

/// @note settings are added to the UI as they are made so make them in the order you want them to show up
class SettingsUI : public Settings, public virtual Object
{
public:
    typedef Object::Ptr<SettingsUI> Ptr;

    /// @param canvas the canvas to add the settings menu to
    /// @param size the size layout of the menu
    SettingsUI(Canvas* canvas, const tgui::Layout2d& size = {"100%", "100%"}, const tgui::Layout2d& position = {0,0});
    /// @brief updates the UI for the current default theme
    /// @note this should be called when ever the theme changes after creation
    void updateForTheme();
    void setVisible(bool visible = true);
    bool isVisible() const;
    /// @brief creates a label that separates the last setting from the new ones in the given section
    /// @param subSectionText the text that will be applied the the label
    /// @note if the section does not exists it will be created
    void createSubSectionLabel(const std::string& section, const std::string& subSectionText);

    void setDecimalRounding(uint8_t rounding = 6);
    unsigned int getDecimalRounding() const;
    /// @brief sets the effect that will be used to show different sections when changing between them
    /// @param showEffect the effect to be used
    /// @param duration the duration of the given effect
    void setShowEffect(tgui::ShowEffectType showEffect, tgui::Duration duration);
    tgui::ShowEffectType getShowEffect() const;
    tgui::Duration getShowEffectDuration() const;

    /// @note Optional parameter: if it is visible
    EventHelper::EventDynamic<bool> onVisibilityChanged;

protected:
    /// @note if nullptr is given returns a panel that has only a child label
    /// @returns the setting UI setup for the given setting
    tgui::Widget::Ptr createSettingUI(SettingBase* setting);
    /// @brief called when updateForTheme is called
    EventHelper::Event m_onUpdateTheme;

private:
    /// @brief the ptr to this settings UI parent
    tgui::Container::Ptr m_parent = nullptr;
    std::map<std::string, tgui::HorizontalWrap::Ptr> m_sections;

    float SPACE_BETWEEN_WIDGETS;
    float HEIGHT;
    float TEXT_SIZE;
    uint8_t m_decimalRounding = 6;
    tgui::ShowEffectType m_showEffect = tgui::ShowEffectType::Fade;
    tgui::Duration m_showDuration = 0;
};

#endif
