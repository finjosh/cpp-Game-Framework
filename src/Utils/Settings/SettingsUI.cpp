#include "Utils/Settings/SettingsUI.hpp"
#include "Utils/StringHelper.hpp"
#include "Utils/Settings/AllSettingTypes.hpp"
#include "Utils/TerminatingFunction.hpp"
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
        }
    });

    m_onEnabled(SettingsUI::setVisible, this, true);
    m_onDisabled(SettingsUI::setVisible, this, false);

    SPACE_BETWEEN_WIDGETS = m_parent->getSize().y * 0.01;
    HEIGHT = m_parent->getSize().y * 0.1;
    TEXT_SIZE = m_parent->getSize().y * 0.035;

    auto sections = tgui::HorizontalWrap::create();
    auto scrollablePanel = tgui::ScrollablePanel::create({"20%", "100%"});
    scrollablePanel->getHorizontalScrollbar()->setPolicy(tgui::Scrollbar::Policy::Never);
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
        panel->getHorizontalScrollbar()->setPolicy(tgui::Scrollbar::Policy::Never);
        panel->setPosition("21%", 0);
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
                    iter.second->getParent()->showWithEffect(m_showEffect, m_showDuration);
                    continue;
                }

                iter.second->getParent()->setVisible(false);
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
    onVisibilitySet.invoke(visible);
}

void SettingsUI::showWithEffect(tgui::ShowEffectType showEffect, tgui::Duration duration)
{
    m_parent->setEnabled(true);
    m_parent->moveToFront();
    m_parent->setFocused(true);
    m_parent->showWithEffect(showEffect, duration);
    onVisibilitySet.invoke(true);
}

void SettingsUI::hideWithEffect(tgui::ShowEffectType hideEffect, tgui::Duration duration)
{
    m_parent->setEnabled(false);
    m_parent->hideWithEffect(hideEffect, duration);
    onVisibilitySet.invoke(false);
}

bool SettingsUI::isVisible() const
{
    return m_parent != nullptr && m_parent->isVisible();
}

tgui::Container::Ptr SettingsUI::getParentGUI()
{
    return m_parent;
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

    const tgui::Layout2d INPUT_SIZE{"47.5%" - tgui::bindHeight(panel)*0.1, "80%"};
    const tgui::Layout INPUT_SPACING{"1%"}; //* use if you have more than one input widget (this spacing is different from the label and input size spacing)
    const tgui::Layout2d INPUT_POS{"52.5%", "10%"};

    if (setting->getInputValidation() == SettingInputValidation::OptionsList) // we dont need any input specific UI
    {
        auto comboBox = tgui::ComboBox::create();
        comboBox->setTextSize(TEXT_SIZE);
        panel->add(comboBox);
        comboBox->setSize(INPUT_SIZE);
        comboBox->setPosition(INPUT_POS);
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
    else if (setting->getInputValidation() == SettingInputValidation::Range) // we want to do it differently based on UI input and that it is a range
    {
        float min, max, stepSize, currentValue; // even though the settings have different types the UI takes it in as a float so it does not matter
        if (setting->getType() == "int")
        {
            auto temp = setting->cast<IntSetting>(); // we know its a int
            min = temp->getMin();
            max = temp->getMax();
            stepSize = temp->getStepSize();
            currentValue = (float)temp->getValue();
        }
        else if (setting->getType() == "uint")        
        {
            auto temp = setting->cast<UIntSetting>(); // we know its a uint
            min = temp->getMin();
            max = temp->getMax();
            stepSize = temp->getStepSize();
            currentValue = (float)temp->getValue();
        }
        else if (setting->getType() == "float")
        {
            auto temp = setting->cast<FloatSetting>(); // we know its a float
            min = temp->getMin();
            max = temp->getMax();
            stepSize = temp->getStepSize();
            currentValue = (float)temp->getValue();
        }
        else
        {
            // TODO implement custom implementation function/callback
            throw std::logic_error("Setting type not in switch case! (SettingsUI::createSettingUI (while creating range based UI))");
        }
        auto editBox = tgui::EditBox::create();
        editBox->setDefaultText(setting->getDefaultValueStr(m_decimalRounding));
        editBox->setTextSize(TEXT_SIZE);
        auto slider = tgui::Slider::create(min, max);
        slider->setStep(stepSize);
        slider->setValue(currentValue);

        slider->setSize(INPUT_SIZE.x/2, INPUT_SIZE.y/2);
        slider->setPosition(INPUT_POS.x, INPUT_POS.y + INPUT_SIZE.y/4);
        editBox->setSize(INPUT_SIZE.x/2-INPUT_SPACING/2, INPUT_SIZE.y);
        editBox->setPosition(INPUT_POS.x + tgui::bindWidth(slider) + INPUT_SPACING, INPUT_POS.y);

        slider->onValueChange([setting, this](float value){
            setting->setValueStr(std::to_string(value));
        });
        editBox->onReturnOrUnfocus([setting, editBox, this](tgui::String text){
            setting->setValueStr(text.toStdString());
        });
        setting->onValueSetStr([editBox, slider, setting, this](std::string value){
            slider->onValueChange.setEnabled(false);
            slider->setValue(StringHelper::toFloat(value));
            slider->onValueChange.setEnabled(true);
            
            std::string newValue = setting->getValueStr(m_decimalRounding);
            if (newValue == setting->getDefaultValueStr(m_decimalRounding))
                editBox->setText("");
            else
                editBox->setText(newValue);
        });

        panel->add(editBox);
        panel->add(slider);

        return panel;
    }

    auto editBox = tgui::EditBox::create();
    editBox->setTextSize(TEXT_SIZE);
    editBox->setSize(INPUT_SIZE);
    editBox->setPosition(INPUT_POS);
    editBox->setDefaultText(setting->getDefaultValueStr(m_decimalRounding));
    editBox->onReturnOrUnfocus([setting](tgui::String input){
        setting->setValueStr(input.toStdString());
    });

    if (setting->getType() == "int")
    {
        editBox->setInputValidator(tgui::EditBox::Validator::Int);
        auto temp = setting->cast<IntSetting>();
        temp->onValueSet([editBox, temp, this](int value){
            if (value == temp->getDefaultValue())
                editBox->setText("");
            else
                editBox->setText(std::to_string(value));
        });
        panel->add(editBox);
    }
    else if (setting->getType() == "uint")
    {
        editBox->setInputValidator(tgui::EditBox::Validator::UInt);
        auto temp = setting->cast<UIntSetting>();
        temp->onValueSet([editBox, temp, this](unsigned int value){
            if (value == temp->getDefaultValue())
                editBox->setText("");
            else
                editBox->setText(std::to_string(value));
        });
        panel->add(editBox);
    }
    else if (setting->getType() == "float")
    {
        editBox->setInputValidator(tgui::EditBox::Validator::Float);
        auto temp = setting->cast<FloatSetting>();
        temp->onValueSet([editBox, temp, this](float value){
            if (value == temp->getDefaultValue())
                editBox->setText("");
            else
                editBox->setText(StringHelper::FloatToStringRound(value, m_decimalRounding));
        });
        panel->add(editBox);
    }
    else if (setting->getType() == "string")
    {
        setting->onValueSetStr([editBox, setting, this](std::string value){
            if (value == setting->getDefaultValueStr())
                editBox->setText("");
            else
                editBox->setText(value);
        });
        panel->add(editBox);
    }
    else if (setting->getType() == "bool")
    {
        auto checkbox = tgui::CheckBox::create();
        checkbox->setChecked(setting->cast<BoolSetting>()->getValue());
        checkbox->setPosition(INPUT_POS.x + tgui::bindWidth(panel)/4 - tgui::bindWidth(checkbox)/2, INPUT_POS.y);
        checkbox->setSize(tgui::bindHeight(checkbox), INPUT_SIZE.y);

        checkbox->onChange([setting](bool checked){
            setting->setValueStr(std::to_string(checked));
        });

        setting->cast<BoolSetting>()->onValueSet([checkbox](bool value){
            checkbox->onChange.setEnabled(false);
            checkbox->setChecked(value);
            checkbox->onChange.setEnabled(true);
        });

        panel->add(checkbox);
    }
    else if (setting->getType() == "input_action_event")
    {
        auto toggleBtn = tgui::ToggleButton::create(setting->getValueStr()); // rounding does not matter for input
        toggleBtn->setTextSize(TEXT_SIZE);
        toggleBtn->setSize(INPUT_SIZE);
        toggleBtn->setPosition(INPUT_POS);

        TFunction inputCheck{[setting, toggleBtn](TData* data){
            data->setRunning();
            if (data->getTotalTime() > data->getDeltaTime() || data->getTotalTime() >= 0.5f) // either we have more total time than one frame or total time of 0.5f seconds
            {
                for (const auto& frameData: Input::get().getFrameData())
                {
                    if (frameData.state == Input::State::JustReleased)
                    {
                        data->setFinished();
                        toggleBtn->setDown(false);
                        break;
                    }
                }
                setting->cast<InputSetting>()->setValue(Input::get().getAllOf({Input::State::Pressed, Input::State::JustReleased, Input::State::JustPressed}));
            }
        }};

        toggleBtn->onToggle([inputCheck](bool toggled){ // start taking inputs for this events inputs
            if (toggled)
            {
                TFunc::remove(inputCheck.getTypeid());
                TFunc::Add(inputCheck);
            }
            else
                TFunc::remove(inputCheck.getTypeid());
        }); 

        setting->onValueSetStr([toggleBtn](std::string value){
            toggleBtn->setText(value);
        });

        panel->add(toggleBtn);
    }
    else if (setting->getType() == "color")
    {
        const tgui::Layout COLOR_BOX_WIDTH = (INPUT_SIZE.x-INPUT_SPACING*3)/4;

        auto colorSetting = setting->cast<ColorSetting>();
        auto rBox = tgui::EditBox::create();
        rBox->setSize(COLOR_BOX_WIDTH, INPUT_SIZE.y);
        rBox->setPosition(INPUT_POS);
        rBox->setTextSize(TEXT_SIZE);
        rBox->setInputValidator(tgui::EditBox::Validator::UInt);
        rBox->setDefaultText(std::to_string(colorSetting->getValue().r));
        rBox->onReturnOrUnfocus([colorSetting, rBox](tgui::String value){
            Color temp = colorSetting->getValue();
            temp.r = static_cast<std::uint8_t>(value.toInt(colorSetting->getDefaultValue().r));
            colorSetting->setValue(temp);
        });
        
        auto gBox = tgui::EditBox::create();
        gBox->setSize(COLOR_BOX_WIDTH, INPUT_SIZE.y);
        gBox->setPosition(INPUT_POS.x + COLOR_BOX_WIDTH + INPUT_SPACING, INPUT_POS.y);
        gBox->setTextSize(TEXT_SIZE);
        gBox->setInputValidator(tgui::EditBox::Validator::UInt);
        gBox->setDefaultText(std::to_string(colorSetting->getValue().g));
        gBox->onReturnOrUnfocus([colorSetting, gBox](tgui::String value){
            Color temp = colorSetting->getValue();
            temp.g = static_cast<std::uint8_t>(value.toInt(colorSetting->getDefaultValue().g));
            colorSetting->setValue(temp);
        });

        auto bBox = tgui::EditBox::create();
        bBox->setSize(COLOR_BOX_WIDTH, INPUT_SIZE.y);
        bBox->setPosition(INPUT_POS.x + (COLOR_BOX_WIDTH)*2 + INPUT_SPACING*2, INPUT_POS.y);
        bBox->setTextSize(TEXT_SIZE);
        bBox->setInputValidator(tgui::EditBox::Validator::UInt);
        bBox->setDefaultText(std::to_string(colorSetting->getValue().b));
        bBox->onReturnOrUnfocus([colorSetting, bBox](tgui::String value){
            Color temp = colorSetting->getValue();
            temp.b = static_cast<std::uint8_t>(value.toInt(colorSetting->getDefaultValue().b));
            colorSetting->setValue(temp);
        });

        auto aBox = tgui::EditBox::create();
        aBox->setSize(COLOR_BOX_WIDTH, INPUT_SIZE.y);
        aBox->setPosition(INPUT_POS.x + (COLOR_BOX_WIDTH)*3 + INPUT_SPACING*3, INPUT_POS.y);
        aBox->setTextSize(TEXT_SIZE);
        aBox->setInputValidator(tgui::EditBox::Validator::UInt);
        aBox->setDefaultText(std::to_string(colorSetting->getValue().a));
        aBox->onReturnOrUnfocus([colorSetting, aBox](tgui::String value){
            Color temp = colorSetting->getValue();
            temp.a = static_cast<std::uint8_t>(value.toInt(colorSetting->getDefaultValue().a));
            colorSetting->setValue(temp);
        });

        colorSetting->onValueSet([rBox, gBox, bBox, aBox, colorSetting](Color color){
            if (colorSetting->getDefaultValue().r == color.r)
                rBox->setText("");
            else
                rBox->setText(std::to_string(color.r));
            
            if (colorSetting->getDefaultValue().g == color.g)
                gBox->setText("");
            else
                gBox->setText(std::to_string(color.g));
            
            if (colorSetting->getDefaultValue().b == color.b)
                bBox->setText("");
            else
                bBox->setText(std::to_string(color.b));
            
            if (colorSetting->getDefaultValue().a == color.a)
                aBox->setText("");
            else
                aBox->setText(std::to_string(color.a));
        });

        panel->add(rBox, "rBox");
        panel->add(gBox, "gBox");
        panel->add(bBox, "bBox");
        panel->add(aBox, "aBox");
    }
    else
    {
        // TODO implement custom implementation function/callback
        throw std::logic_error("Setting type not in switch case! (SettingsUI::createSettingUI)");
    }

    return panel;
}

tgui::Button::Ptr SettingsUI::createButton(const std::string& section, const Vector2& sizeScale)
{
    tgui::Group::Ptr parent = createSpacer(section, sizeScale.y);
    tgui::Button::Ptr widget = tgui::Button::create();
    widget->setSize(tgui::Layout{"100%"} * sizeScale.x, "100%");
    widget->setPosition(("100%" - tgui::bindWidth(widget)) / 2, 0.f);
    widget->setTextSize(TEXT_SIZE);
    parent->add(widget);

    return widget;
}

tgui::BitmapButton::Ptr SettingsUI::createBitmapButton(const std::string& section, const Vector2& sizeScale)
{
    tgui::Group::Ptr parent = createSpacer(section, sizeScale.y);
    tgui::BitmapButton::Ptr widget = tgui::BitmapButton::create();
    widget->setSize(tgui::Layout{"100%"} * sizeScale.x, "100%");
    widget->setPosition(("100%" - tgui::bindWidth(widget)) / 2, 0.f);
    widget->setTextSize(TEXT_SIZE);
    parent->add(widget);

    return widget;
}

void SettingsUI::createResetButton(const std::string& section, const std::list<std::string>& settings, const std::string& tooltip, const Vector2& sizeScale)
{
    tgui::Button::Ptr button = createButton(section, sizeScale);
    button->setText("Reset to Default");

    if (tooltip != "")
    {
        auto panel = tgui::Panel::create();
        auto label = tgui::Label::create(tooltip);
        panel->setSize(tgui::bindSize(label) + panel->getSharedRenderer()->getPadding().getOffset()*2);
        panel->add(label);
        panel->setOrigin(0,1);
        button->setToolTip(panel);
        m_onUpdateTheme([panel, label](){
            panel->setSize(tgui::bindSize(label) + panel->getSharedRenderer()->getPadding().getOffset()*2);
        });
    }

    button->onClick([this](std::string section, std::list<std::string> settings){
        for (const std::string& setting: settings)
        {
            SettingBase* temp = Settings::getSetting(section, setting);
            temp->setValueStr(temp->getDefaultValueStr());
        }
    }, section, settings);
}

tgui::Group::Ptr SettingsUI::createSpacer(const std::string& section, float heightScale)
{
    Settings::createSection(section);
    auto sectionWrap = m_sections.find(section)->second; // guarantied to exist
    
    tgui::Group::Ptr widget = tgui::Group::create({"100%", HEIGHT * heightScale});
    sectionWrap->add(widget);
    sectionWrap->setHeight(widget->getPosition().y + widget->getFullSize().y + sectionWrap->getParent()->cast<tgui::ScrollablePanel>()->getSharedRenderer()->getPadding().getBottom());

    return widget;
}
