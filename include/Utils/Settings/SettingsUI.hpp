#ifndef SETTINGS_UI_HPP
#define SETTINGS_UI_HPP

#pragma once

#include "Utils/Settings/Settings.hpp"
#include "Object.hpp"
#include "Graphics/Canvas.hpp"
#include "Utils/funcHelper.hpp"

#include "TGUI/Container.hpp"
#include "TGUI/Widgets/HorizontalWrap.hpp"
#include "TGUI/Widgets/Button.hpp"
#include "TGUI/Widgets/BitmapButton.hpp"

/// @note settings are added to the UI as they are made so make them in the order you want them to show up
/// @note uses termination functions for the input setting type
/// @warning input settings wont work properly unless you tell Input that no input was handled while this is visible
/// @bug if a section has any setting which uses a button to change data (e.x. InputSetting) as the first setting AND you are using a scale show effect the program will run into an infinite loop and crash (to fix add something above it that does not use a button e.x. a subSectionLabel or another setting)
class SettingsUI : public Settings, public virtual Object
{
public:
    typedef Object::Ptr<SettingsUI> Ptr;

    /// @param canvas the canvas to add the settings menu to
    /// @param size the size layout of the menu
    SettingsUI(Canvas* canvas, const tgui::Layout2d& size = {"100%", "100%"}, const tgui::Layout2d& position = {0,0});
    virtual ~SettingsUI();
    /// @brief updates the UI for the current default theme
    /// @note this should be called when ever the theme changes after creation
    void updateForTheme();
    void setVisible(bool visible = true);
    void showWithEffect(tgui::ShowEffectType showEffect, tgui::Duration duration);
    void hideWithEffect(tgui::ShowEffectType hideEffect, tgui::Duration duration);
    bool isVisible() const;
    tgui::Container::Ptr getParentGUI();
    /// @brief creates a label that separates the last setting from the new ones in the given section
    /// @param subSectionText the text that will be applied the the label
    /// @note if the section does not exists it will be created
    void createSubSectionLabel(const std::string& section, const std::string& subSectionText);
    /// @brief creates a button in the given section and returns the ptr to said button
    /// @note size and position are already set up (only edit if you know what you are doing)
    /// @param sizeScale the size of the button based on the size of a setting, values from (0-1)
    tgui::Button::Ptr createButton(const std::string& section, const Vector2& sizeScale = {1.f,1.f});
    /// @brief creates a button in the given section and returns the ptr to said button
    /// @note size and position are already set up (only edit if you know what you are doing)
    /// @param sizeScale the size of the button based on the size of a setting, values from (0-1)
    tgui::BitmapButton::Ptr createBitmapButton(const std::string& section, const Vector2& sizeScale = {1.f,1.f});
    /// @brief creates a button using "createButton" in the given section while giving it events to reset all given settings back to default values
    /// @param tooltip the tool tip that will show for the button ("" will result in no tooltip being added)
    /// @param sizeScale the size of the button based on the size of a setting, values from (0-1)
    /// @note creates section if non-existant
    void createResetButton(const std::string& section, const std::list<std::string>& settings = {}, const std::string& tooltip = "", const Vector2& sizeScale = {1.f,1.f});
    /// @brief creates a spacer the given section
    /// @note creates the section if non-existant
    /// @param heightScale the height of the spacer based on the size of a normal setting
    /// @returns the ptr to the group widget that is used as a spacer (only use if you know what you are doing)
    tgui::Group::Ptr createSpacer(const std::string& section, float heightScale = 1.f);

    void setDecimalRounding(uint8_t rounding = 6);
    unsigned int getDecimalRounding() const;
    /// @brief sets the effect that will be used to show different sections when changing between them
    /// @param showEffect the effect to be used
    /// @param duration the duration of the given effect
    void setShowEffect(tgui::ShowEffectType showEffect, tgui::Duration duration);
    tgui::ShowEffectType getShowEffect() const;
    tgui::Duration getShowEffectDuration() const;

    /// @note Optional parameter: if it is visible
    EventHelper::EventDynamic<bool> onVisibilitySet;

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
