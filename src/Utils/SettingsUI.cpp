#include "Utils/SettingsUI.hpp"
#include "Utils/StringHelper.hpp"
#include "TGUI/Widgets/Panel.hpp"
#include "TGUI/Widgets/ToggleButton.hpp"
#include "TGUI/Widgets/CheckBox.hpp"
#include "TGUI/Widgets/ComboBox.hpp"
#include "TGUI/Widgets/Label.hpp"
#include "TGUI/Widgets/EditBox.hpp"
#include "TGUI/Widgets/ScrollablePanel.hpp"
#include "TGUI/Widgets/Slider.hpp"

SettingsUI::SettingsUI(Canvas* canvas, const tgui::Layout2d& size, const tgui::Layout2d& position)
{
    m_parent = tgui::Panel::create(size);
    m_parent->setPosition(position);
    canvas->add(m_parent);
    this->setVisible(false);

    m_onDestroyQueued([this](){
        if (m_parent)
        {
            m_parent->setVisible(false);
            m_parent->setEnabled(false);
        }
    });

    SPACE_BETWEEN_WIDGETS = m_parent->getSize().y * 0.01;
    HEIGHT = m_parent->getSize().y * 0.1;
    TEXT_SIZE = m_parent->getSize().y * 0.035;

    auto sections = tgui::HorizontalWrap::create();
    auto scrollablePanel = tgui::ScrollablePanel::create({"20%", "100%"});
    scrollablePanel->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
    m_parent->add(scrollablePanel, "SectionPanel");
    scrollablePanel->add(sections, "SectionSelector");
    sections->getRenderer()->setSpaceBetweenWidgets(SPACE_BETWEEN_WIDGETS);

    Settings::onSectionCreated([this, sections, scrollablePanel](std::string section){
        auto button = tgui::ToggleButton::create(section, sections->getWidgets().size() == 0);
        button->setSize("100%", HEIGHT);
        button->setTextSize(TEXT_SIZE);
        sections->add(button, section);
        sections->setHeight(button->getPosition().y + button->getFullSize().y + scrollablePanel->getSharedRenderer()->getPadding().getBottom());
        scrollablePanel->setHeight(tgui::bindMin("100%", sections->getSize().y + scrollablePanel->getSharedRenderer()->getPadding().getTop() + scrollablePanel->getSharedRenderer()->getPadding().getBottom()*2));

        auto panel = tgui::ScrollablePanel::create({"79%", "100%"});
        panel->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        panel->setPosition("21%", 0);
        panel->setEnabled(button->isDown());
        panel->setVisible(button->isDown());
        auto wrap = tgui::HorizontalWrap::create();
        panel->add(wrap);
        panel->setHeight(tgui::bindMin("100%", tgui::bindHeight(wrap) + panel->getSharedRenderer()->getPadding().getTop() + panel->getSharedRenderer()->getPadding().getBottom()*2));
        m_parent->add(panel, section + "Panel");
        wrap->getRenderer()->setSpaceBetweenWidgets(SPACE_BETWEEN_WIDGETS); 
        m_sections.emplace(section, wrap);

        button->onToggle([this, sections, section, button](bool down){
            if (!down)
            {
                if (m_sections.find(section)->second->isVisible())
                {
                    button->onToggle.setEnabled(false);
                    button->setDown(true);
                    button->onToggle.setEnabled(true);
                }
                return;
            }
            for (auto widget: sections->getWidgets())
            {
                if (widget->getWidgetName() != section)
                {
                    auto btn = widget->cast<tgui::ToggleButton>();
                    btn->onToggle.setEnabled(false);
                    btn->setDown(false);
                    btn->onToggle.setEnabled(true);
                }
            }
            for (auto iter: m_sections)
            {
                if (iter.first == section)
                {
                    iter.second->getParent()->setVisible(true);
                    iter.second->getParent()->setEnabled(true);
                    iter.second->getParent()->showWithEffect(m_showEffect, m_showDuration);
                    continue;
                }

                iter.second->getParent()->setVisible(false);
                iter.second->getParent()->setEnabled(false);
            }
        });
    });
    Settings::onSettingCreated([this](std::string section, SettingBase* setting){
        if (setting == nullptr)
            return;
        auto wrap = m_sections.find(section)->second; // we know for a fact this exists
        auto widget = SettingsUI::createSettingUI(setting);
        wrap->add(widget);
        wrap->setHeight(widget->getPosition().y + widget->getFullSize().y + wrap->getParent()->cast<tgui::ScrollablePanel>()->getSharedRenderer()->getPadding().getBottom());
    });
}

SettingsUI::~SettingsUI()
{
    if (m_parent) 
    {
        auto parent = m_parent->getParent();
        if (parent)
            parent->remove(m_parent);
        m_parent = nullptr;
    }
}

void SettingsUI::updateForTheme()
{
    auto scrollablePanel = m_parent->get("SectionPanel")->cast<tgui::ScrollablePanel>();
    auto sections = scrollablePanel->get("SectionSelector")->cast<tgui::HorizontalWrap>();
    if (sections->getWidgets().size() != 0)
        sections->setHeight(sections->getWidgets().back()->getPosition().y + sections->getWidgets().back()->getFullSize().y + scrollablePanel->getSharedRenderer()->getPadding().getBottom());
    scrollablePanel->setHeight(tgui::bindMin("100%", sections->getSize().y + scrollablePanel->getSharedRenderer()->getPadding().getTop() + scrollablePanel->getSharedRenderer()->getPadding().getBottom()*2));

    m_onUpdateTheme.invoke();

    for (auto wrap: m_sections)
    {
        auto widget = wrap.second->getWidgets().back();
        auto panel = wrap.second->getParent()->cast<tgui::ScrollablePanel>();
        wrap.second->setHeight(widget->getPosition().y + widget->getFullSize().y + panel->getSharedRenderer()->getPadding().getBottom());
        panel->setHeight(tgui::bindMin("100%", tgui::bindHeight(wrap.second) + panel->getSharedRenderer()->getPadding().getTop() + panel->getSharedRenderer()->getPadding().getBottom()*2));
    }
}

void SettingsUI::setVisible(bool visible)
{
    assert(("Must initUI before trying to set visiblity", m_parent != nullptr));

    m_parent->setVisible(visible);
    m_parent->setEnabled(visible);
    if (visible)
    {
        m_parent->moveToFront();
        m_parent->setFocused(true);
    }
    onVisibilityChanged.invoke(visible);
}

bool SettingsUI::isVisible() const
{
    return m_parent != nullptr && m_parent->isVisible();
}

void SettingsUI::createSubSectionLabel(const std::string& section, const std::string& subSectionText)
{
    createSection(section);

    auto wrap = m_sections.find(section)->second; // we know for a fact this exists
    auto group = tgui::Group::create();
    auto panel = tgui::Panel::create();
    auto label = tgui::Label::create(subSectionText);
    group->setHeight(HEIGHT);
    m_onUpdateTheme([panel, label, this](){
        panel->setSize(tgui::bindMin(tgui::bindWidth(label) + panel->getSharedRenderer()->getPadding().getLeft() + panel->getSharedRenderer()->getPadding().getRight(), "100%"), HEIGHT);
    });
    panel->setSize(tgui::bindMin(tgui::bindWidth(label) + panel->getSharedRenderer()->getPadding().getLeft() + panel->getSharedRenderer()->getPadding().getRight(), "100%"), HEIGHT);
    panel->setOrigin(0.5, 0.5);
    panel->setPosition("50%", "50%");
    panel->add(label);
    label->setOrigin(0.5, 0.5);
    label->setPosition("50%", "50%");
    label->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
    label->setVerticalAlignment(tgui::VerticalAlignment::Center);
    label->setTextSize(TEXT_SIZE*1.25);
    label->setAutoSize(true);
    group->add(panel);
    wrap->add(group);
    wrap->setHeight(group->getPosition().y + group->getFullSize().y + wrap->getParent()->cast<tgui::ScrollablePanel>()->getSharedRenderer()->getPadding().getBottom());
}

void SettingsUI::setDecimalRounding(uint8_t rounding)
{
    m_decimalRounding = rounding;
}

unsigned int SettingsUI::getDecimalRounding() const
{
    return m_decimalRounding;
}

void SettingsUI::setShowEffect(tgui::ShowEffectType showEffect, tgui::Duration duration)
{
    m_showEffect = showEffect;
    m_showDuration = duration;
}

tgui::ShowEffectType SettingsUI::getShowEffect() const
{
    return m_showEffect;
}

tgui::Duration SettingsUI::getShowEffectDuration() const
{
    return m_showDuration;
}

tgui::Widget::Ptr SettingsUI::createSettingUI(SettingBase* setting)
{
    if (setting == nullptr)
        return nullptr;

    auto panel = tgui::Panel::create();
    if (setting->getDescription() != "")
    {
        auto tooltip = tgui::Panel::create();
        auto label = tgui::Label::create(setting->getDescription());
        tooltip->setSize(tgui::bindSize(label) + tooltip->getSharedRenderer()->getPadding().getOffset()*2);
        tooltip->add(label);
        tooltip->setOrigin(0,1);
        panel->setToolTip(tooltip);
        m_onUpdateTheme([tooltip, label](){
            tooltip->setSize(tgui::bindSize(label) + tooltip->getSharedRenderer()->getPadding().getOffset()*2);
        });
    }
    panel->setHeight(HEIGHT);
    auto label = tgui::Label::create(setting->getName());
    panel->add(label);
    label->setSize("35%", "80%");
    label->setPosition("10%", "10%");
    label->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
    label->setVerticalAlignment(tgui::VerticalAlignment::Center);
    label->setTextSize(TEXT_SIZE);

    const tgui::Layout2d inputSize{"47.5%" - tgui::bindHeight(panel)*0.1, "80%"};
    const tgui::Layout2d inputPos{"52.5%", "10%"};

    if (setting->getInputValidation() == SettingInputValidation::OptionsList) // we dont need any input specific UI
    {
        auto comboBox = tgui::ComboBox::create();
        comboBox->setTextSize(TEXT_SIZE);
        panel->add(comboBox);
        comboBox->setSize(inputSize);
        comboBox->setPosition(inputPos);
        for (auto option: setting->getOptionsStr(m_decimalRounding))
            comboBox->addItem(option);

        comboBox->setSelectedItem(setting->getValueStr(m_decimalRounding));

        comboBox->onItemSelect([setting](tgui::String selected){
            setting->setValueStr(selected.toStdString());
        });

        setting->onValueSetStr([comboBox](std::string value){
            comboBox->onItemSelect.setEnabled(false);
            comboBox->setSelectedItem(value);
            comboBox->onItemSelect.setEnabled(true);
        });

        return panel;
    }
    else if (setting->getInputValidation() == SettingInputValidation::Range) // we want to do it differently
    {
        float min, max, stepSize, currentValue; // even though the settings have different types the UI takes it in as a float so it does not matter
        switch (setting->getType())
        {
        case SettingType::Int:
        {
            auto temp = static_cast<Setting<int>*>(setting);
            min = temp->getMin();
            max = temp->getMax();
            stepSize = temp->getStepSize();
            currentValue = (float)temp->getValue();
            break;
        }
        
        case SettingType::UInt:
        {
            auto temp = static_cast<Setting<unsigned int>*>(setting);
            min = temp->getMin();
            max = temp->getMax();
            stepSize = temp->getStepSize();
            currentValue = (float)temp->getValue();
            break;
        }

        case SettingType::Float:
        {
            auto temp = static_cast<Setting<float>*>(setting);
            min = temp->getMin();
            max = temp->getMax();
            stepSize = temp->getStepSize();
            currentValue = (float)temp->getValue();
            break;
        }

        default:
            throw std::logic_error("Setting type not in switch case! (SettingsUI::createSettingUI (while creating range based UI))");
            break;
        }
        auto editBox = tgui::EditBox::create();
        editBox->setDefaultText(setting->getDefaultValueStr(m_decimalRounding));
        editBox->setTextSize(TEXT_SIZE);
        auto slider = tgui::Slider::create(min, max);
        slider->setStep(stepSize);
        slider->setValue(currentValue);

        slider->setSize(inputSize.x/2, inputSize.y/2);
        slider->setPosition(inputPos.x, inputPos.y + inputSize.y/4);
        editBox->setSize(inputSize.x/2-tgui::Layout{"1%"}, inputSize.y);
        editBox->setPosition(inputPos.x + tgui::bindWidth(slider) + tgui::Layout{"1%"}, inputPos.y);

        slider->onValueChange([setting, this](float value){
            setting->setValueStr(std::to_string(value));
        });
        editBox->onReturnOrUnfocus([setting, editBox, this](tgui::String text){
            setting->setValueStr(text.toStdString());
        });
        setting->onValueSetStr([editBox, slider, this](std::string value){
            slider->onValueChange.setEnabled(false);
            slider->setValue(StringHelper::toFloat(value));
            slider->onValueChange.setEnabled(true);
            editBox->setText(StringHelper::FloatToStringRound(slider->getValue(), m_decimalRounding));
        });

        panel->add(editBox);
        panel->add(slider);

        return panel;
    }

    auto editBox = tgui::EditBox::create();
    editBox->setTextSize(TEXT_SIZE);
    editBox->setSize(inputSize);
    editBox->setPosition(inputPos);
    editBox->setDefaultText(setting->getDefaultValueStr(m_decimalRounding));
    editBox->onReturnOrUnfocus([setting](tgui::String input){
        setting->setValueStr(input.toStdString());
    });

    switch (setting->getType())
    {
    case SettingType::Int:
    {
        editBox->setInputValidator(tgui::EditBox::Validator::Int);
        auto temp = static_cast<Setting<int>*>(setting);
        temp->onValueSet([editBox, temp, this](int value){
            if (value == temp->getDefaultValue())
                editBox->setText("");
            else
                editBox->setText(std::to_string(value));
        });
        panel->add(editBox);
        break;
    }
    
    case SettingType::UInt:
    {
        editBox->setInputValidator(tgui::EditBox::Validator::UInt);
        auto temp = static_cast<Setting<unsigned int>*>(setting);
        temp->onValueSet([editBox, temp, this](unsigned int value){
            if (value == temp->getDefaultValue())
                editBox->setText("");
            else
                editBox->setText(std::to_string(value));
        });
        panel->add(editBox);
        break;
    }

    case SettingType::Float:
    {
        editBox->setInputValidator(tgui::EditBox::Validator::Float);
        auto temp = static_cast<Setting<float>*>(setting);
        temp->onValueSet([editBox, temp, this](float value){
            if (value == temp->getDefaultValue())
                editBox->setText("");
            else
                editBox->setText(StringHelper::FloatToStringRound(value, m_decimalRounding));
        });
        panel->add(editBox);
        break;
    }

    case SettingType::String:
    {
        setting->onValueSetStr([editBox, setting, this](std::string value){
            if (value == setting->getDefaultValueStr())
                editBox->setText("");
            else
                editBox->setText(value);
        });
        panel->add(editBox);
        break;
    }

    case SettingType::Bool:
    {
        auto checkbox = tgui::CheckBox::create();
        checkbox->setChecked(static_cast<Setting<bool>*>(setting)->getValue());
        checkbox->setPosition(inputPos.x + tgui::bindWidth(panel)/4 - tgui::bindWidth(checkbox)/2, inputPos.y);
        checkbox->setSize(tgui::bindHeight(checkbox), inputSize.y);

        checkbox->onChange([setting](bool checked){
            setting->setValueStr(std::to_string(checked));
        });

        static_cast<Setting<bool>*>(setting)->onValueSet([checkbox](bool value){
            checkbox->onChange.setEnabled(false);
            checkbox->setChecked(value);
            checkbox->onChange.setEnabled(true);
        });

        panel->add(checkbox);
        break;
    }

    default:
        throw std::logic_error("Setting type not in switch case! (SettingsUI::createSettingUI)");
        break;
    }

    return panel;
}
