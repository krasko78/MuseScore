/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "appshellconfiguration.h"

#include <QJsonArray>
#include <QJsonDocument>

#include "settings.h"

#include "multiinstances/resourcelockguard.h"

#include "log.h"

using namespace muse;
using namespace mu;
using namespace mu::appshell;
using namespace mu::notation;

static const std::string module_name("appshell");

// krasko start: KRASKO'S SETTINGS

static constexpr char krasko_module_name[] = "krasko";

static constexpr char KEY_KraskosSettingsHeader[] = "krasko/kraskosSettingsHeaderNotUsed"; // not used, not editable, won't be saved to settings file

static constexpr char KEY_AutoRestoreSessionOnStart[] = "krasko/autoRestoreSessionOnStart";

static constexpr char KEY_FocusExportButtonOnExportDialog[] = "krasko/focusExportButtonOnExportDialog";

static constexpr char KEY_TabAndShiftTabNavigateBetweenControls[] = "krasko/tabAndShiftTabNavigateBetweenControls";

static constexpr char KEY_ChangeActiveGripWithEditElementKey[] = "krasko/changeActiveGripWithEditElementKey";

static constexpr char KEY_EscKeyPreservesSelectionWhenEditing[] = "krasko/escKeyPreservesSelectionWhenEditing";

static constexpr char KEY_ExpandShowMoreSectionsInPropertiesPanel[] = "krasko/expandShowMoreSectionsInPropertiesPanel";

static constexpr char KEY_DoNotHighlightDisabledItemsOnHover[] = "krasko/doNotHighlightDisabledItemsOnHover";

static constexpr char KEY_ShowScrollbarOnScrollableDropDownLists[] = "krasko/showScrollbarOnScrollableDropDownLists";

static constexpr char KEY_ActiveGripColor[] = "krasko/activeGripColor";

static constexpr char KEY_ScrollbarColor[] = "krasko/scrollbarColor";

static constexpr char KEY_MainMenuFontFollowsPreferencesFont[] = "krasko/mainMenuFontFollowsPreferencesFont";

static constexpr char KEY_MainMenuFontSizeMultiplier[] = "krasko/mainMenuFontSizeMultiplier";

static constexpr char KEY_EnableHighPrecisionNudging[] = "krasko/enableHighPrecisionNudging";

static constexpr char KEY_ShowSameColorCheckBoxOnSelectMoreDialog[] = "krasko/showSameColorCheckBoxOnSelectMoreDialog";

static constexpr char KEY_VerticalPanelsWidth[] = "krasko/verticalPanelsWidth";

static constexpr char KEY_ScrollDecelerationOfListsAndPanels[] = "krasko/scrollDecelerationOfListsAndPanels";

static constexpr char KEY_StepForSpinControlsOnAppearanceTab[] = "krasko/stepForSpinControlsOnAppearanceTab";

static constexpr char KEY_TextStylesToUseFullFontHeight[] = "krasko/textStylesToUseFullFontHeight";

static constexpr char KEY_FixFingeringTooCloseToStaffOnBeamedNotes[] = "krasko/fixFingeringTooCloseToStaffOnBeamedNotes";

static constexpr char KEY_RemoveExtraSpacingOnMultilineFingering[] = "krasko/removeExtraSpacingOnMultilineFingering";

// krasko end: KRASKO'S SETTINGS


static const Settings::Key HAS_COMPLETED_FIRST_LAUNCH_SETUP(module_name, "application/hasCompletedFirstLaunchSetup");

static const Settings::Key STARTUP_MODE_TYPE(module_name, "application/startup/modeStart");
static const Settings::Key STARTUP_SCORE_PATH(module_name, "application/startup/startScore");

static const std::string MUSESCORE_ONLINE_HANDBOOK_URL("https://handbook.musescore.org");

static const std::string MUSESCORE_ASK_FOR_HELP_URL_PATH("/redirect/post/question");
static const std::string MUSESCORE_FORUM_URL_PATH("/forum");
static const std::string MUSESCORE_CONTRIBUTE_URL_PATH("/contribute");
static const std::string MUSICXML_URL("https://w3.org");
static const std::string MUSICXML_LICENSE_URL(MUSICXML_URL + "/community/about/process/final/");
static const std::string MUSICXML_LICENSE_DEED_URL(MUSICXML_URL + "/community/about/process/fsa-deed/");

static const std::string UTM_MEDIUM_MENU("menu");

static const QString NOTATION_NAVIGATOR_VISIBLE_KEY("showNavigator");
static const Settings::Key SPLASH_SCREEN_VISIBLE_KEY(module_name, "ui/application/startup/showSplashScreen");

static const muse::io::path_t SESSION_FILE("/session.json");
static const std::string SESSION_RESOURCE_NAME("SESSION");

AppShellConfiguration::~AppShellConfiguration() // krasko
{
    // krasko todo: this is currently not called!
    delete m_settingsCreator;
}

void AppShellConfiguration::init()
{
    initKraskoSettings(); // krasko

    settings()->setDefaultValue(HAS_COMPLETED_FIRST_LAUNCH_SETUP, Val(false));

    settings()->setDefaultValue(STARTUP_MODE_TYPE, Val(StartupModeType::StartEmpty));
    settings()->valueChanged(STARTUP_MODE_TYPE).onReceive(this, [this](const Val&) {
        m_startupModeTypeChanged.notify();
    });

    settings()->setDefaultValue(STARTUP_SCORE_PATH, Val(projectConfiguration()->myFirstProjectPath().toStdString()));
    settings()->valueChanged(STARTUP_SCORE_PATH).onReceive(this, [this](const Val&) {
        m_startupScorePathChanged.notify();
    });

    fileSystem()->makePath(sessionDataPath());
}

// krasko start: KRASKO'S SETTINGS

void AppShellConfiguration::initKraskoSettings()
{
    createKraskoSettings();
    updateRenamedKraskoSettings();
    deleteUnusedKraskoSettings();
    notifyAboutChangedKraskoSettings();
}

void AppShellConfiguration::createKraskoSettings()
{
    m_settingsCreator = new SettingsCreator(settings());
    SettingsCreator& sc = *m_settingsCreator;

	// This first one is not editable since the title starts with --- => will be a separator
    sc.createSetting(krasko_module_name, KEY_KraskosSettingsHeader)
        .setDefaultValue(Val())
        .setDescription(muse::trc("krasko", "––– KRASKO'S SETTINGS –––––––––––––––"))
        .setHelpString("")
        .withoutValueChangedNotifications();

    sc.createSetting(krasko_module_name, KEY_AutoRestoreSessionOnStart)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Auto-restore session on start"))
        .setHelpString(muse::trc("krasko",
            "Determines whether the last session will be restored automatically without asking "
            "when MuseScore Studio starts. Only has effect when 'Preferences' –> 'General' –> "
            "'Program start' is set to 'Continue last session'."))
        .withoutValueChangedNotifications();

    sc.createSetting(krasko_module_name, KEY_FocusExportButtonOnExportDialog)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Focus Export button on Export dialog"))
        .setHelpString(muse::trc("krasko",
            "When enabled, the Export dialog will focus the Export button on open. This allows "
            "the export to be completed quickly by pressing Enter or the Spacebar."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_focusExportButtonOnExportDialogChanged.send(val.toBool());
        });

    sc.createSetting(krasko_module_name, KEY_TabAndShiftTabNavigateBetweenControls)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Tab and Shift+Tab navigate between controls"))
        .setHelpString(muse::trc("krasko",
            "When enabled, the keys for 'nav-next-panel' and 'nav-prev-panel' (Tab and Shift+Tab "
            "by default) will navigate between controls instead of panels."))
        .withoutValueChangedNotifications();

    sc.createSetting(krasko_module_name, KEY_ChangeActiveGripWithEditElementKey)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Change active grip with 'Edit element' key"))
        .setHelpString(muse::trc("krasko",
            "When enabled, pressing the 'Edit Element' key (F2 by default) will cycle through "
			"an element's grips (just like the Tab key) if the grips are already displayed. "
			"This lets you press the 'Edit Element' key once to display the grips (the element "
			"must be selected first) and then continue pressing the same key (instead of Tab) "
			"to activate a particular grip."))
        .withoutValueChangedNotifications();

    sc.createSetting(krasko_module_name, KEY_EscKeyPreservesSelectionWhenEditing)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Esc key preserves selection when editing"))
        .setHelpString(muse::trc("krasko",
            "When enabled, pressing the Esc key while editing an element will end the editing "
            "but not deselect the element. This allows you to press the 'Edit Element' key "
            "and continue editing until you are happy with the result. Useful for example "
            "for elements with grips. When disabled, the element will be deselected after "
            "pressing Esc and won't be editable until re-selected."))
        .withoutValueChangedNotifications();

    sc.createSetting(krasko_module_name, KEY_ExpandShowMoreSectionsInPropertiesPanel)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Expand 'Show more' sections in Properties panel"))
        .setHelpString(muse::trc("krasko",
            "When enabled, 'Show more' sections in the Properties panel will be initially expanded."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_expandShowMoreSectionsInPropertiesPanelChanged.send(val.toBool());
        });

    sc.createSetting(krasko_module_name, KEY_DoNotHighlightDisabledItemsOnHover)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Do not highlight disabled items on hover"))
        .setHelpString(muse::trc("krasko",
            "Determines whether disabled menu and list items will be highlighted on mouse hover or not. "
            "Not highlighting them emphasizes their disabled state."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_doNotHighlightDisabledItemsOnHoverChanged.send(val.toBool());
        });

    sc.createSetting(krasko_module_name, KEY_ShowScrollbarOnScrollableDropDownLists)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Show scrollbar on scrollable drop-down lists"))
        .setHelpString(muse::trc("krasko",
            "When enabled, the scrollbar on scrollable drop-down lists will always be visible. "
            "Otherwise, it will only appear when the mouse hovers over it."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_showScrollbarOnScrollableDropDownListsChanged.send(val.toBool());
        });

    sc.createSetting(krasko_module_name, KEY_ActiveGripColor)
        .setDefaultValue(Val(QColor("#A0A0A4")))
        .setDescription(muse::trc("krasko", "Active grip color"))
        .setHelpString(muse::trc("krasko",
            "The color to use for the active grip of an element being edited that has grips."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_activeGripColorChanged.send(mu::engraving::Color::fromQColor(val.toQColor()));
        });

    sc.createSetting(krasko_module_name, KEY_ScrollbarColor)
        .setDefaultValue(Val(QColor("")))
        .setDescription(muse::trc("krasko", "Scrollbar color"))
        .setHelpString(muse::trc("krasko", "The color to use for all scrollbars."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_scrollbarColorChanged.send(mu::engraving::Color::fromQColor(val.toQColor()));
        });

    sc.createSetting(krasko_module_name, KEY_MainMenuFontFollowsPreferencesFont)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Main menu font follows preferences font"))
        .setHelpString(muse::trc("krasko",
            "When enabled, the font face of the main menu will be the same as that "
			"of all other UI elements (set under 'Preferences' –> 'Appearance')."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_mainMenuFontFollowsPreferencesFontChanged.send(val.toBool());
        });

    sc.createSetting(krasko_module_name, KEY_MainMenuFontSizeMultiplier)
        .setDefaultValue(Val(0.9))
        .setDescription(muse::trc("krasko", "Main menu font size multiplier"))
        .setHelpString(muse::trc("krasko",
            "Specifies a multiplier, e.g. '1.0' or '0.9' that will be multiplied by the font size "
			"set in the preferences ('Preferences' –> 'Appearance') to obtain the size of the main "
            "menu. For instance, a multiplier of 1.0 will make the main menu font size the same as "
            "that of the other UI elements. A multiplier of 0.9 will make the main menu font size "
            "90% of it, etc. Valid values are from 0.5 to 2.0."))
		.setMinValue(Val(0.5))
		.setMaxValue(Val(2.0))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_mainMenuFontSizeMultiplierChanged.send(val.toDouble());
        });

    sc.createSetting(krasko_module_name, KEY_EnableHighPrecisionNudging)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Enable high-precision nudging"))
        .setHelpString(muse::trc("krasko",
            "When enabled, nudging things around with the arrow keys will nudge them by a smaller "
            "amount allowing for better control and precision. Does not affect nudging with "
            "Ctrl+arrow keys and Alt+arrow keys."))
        .withoutValueChangedNotifications();

	sc.createSetting(krasko_module_name, KEY_ShowSameColorCheckBoxOnSelectMoreDialog)
		.setDefaultValue(Val(false))
		.setDescription(muse::trc("krasko", "Show 'Same Color' checkbox on 'Select more' dialog"))
		.setHelpString(muse::trc("krasko",
			"When enabled, will display a 'Same color' checkbox on the 'Select' –> 'More...' dialog "
			"so that the selection can be limited to the elements having the same color "
			"as the selected element."))
		.withoutValueChangedNotifications();

    sc.createSetting(krasko_module_name, KEY_VerticalPanelsWidth)
        .setDefaultValue(Val(300))
        .setDescription(muse::trc("krasko", "Width of the vertical panels"))
        .setHelpString(muse::trc("krasko",
            "The width of the vertical panels such as the Palettes, Instruments, Properties, "
			"Selection filter, etc. in pixels. Valid values are between 200 and 500."))
        .setMinValue(Val(200))
        .setMaxValue(Val(500))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_verticalPanelsWidthChanged.send(val.toInt());
        });

    sc.createSetting(krasko_module_name, KEY_ScrollDecelerationOfListsAndPanels)
        .setDefaultValue(Val(1500))
        .setDescription(muse::trc("krasko", "Scroll deceleration of lists and panels"))
        .setHelpString(muse::trc("krasko",
            "Determines how fast the scrolling of lists and panels stops. The higher the value, "
            "the sooner the scrolling will stop when the user stops scrolling. Lower values "
            "on the other hand will make the scrolling accelerate more and last longer "
            "decreasing the scrolling precision. Valid values are between 1000 and 50000."))
        .setMinValue(Val(1000))
        .setMaxValue(Val(50000))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_scrollDecelerationOfListsAndPanelsChanged.send(val.toInt());
        });

    sc.createSetting(krasko_module_name, KEY_StepForSpinControlsOnAppearanceTab)
        .setDefaultValue(Val(0.5))
        .setDescription(muse::trc("krasko", "Step for spin controls on Appearance tab of Properties panel"))
        .setHelpString(muse::trc("krasko",
            "Specifies the step (amount) by which spin controls on the Appearance tab of"
            "the Properties panel will change their values when the Up/Down arrow keys are pressed "
            "or the Up/Down buttons are clicked. Valid values are between 0.01 and 10."))
        .setMinValue(Val(0.01))
        .setMaxValue(Val(10))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_stepForSpinControlsOnAppearanceTabChanged.send(val.toDouble());
        });

    sc.createSetting(krasko_module_name, KEY_TextStylesToUseFullFontHeight)
        .setDefaultValue(Val(""))
        .setDescription(muse::trc("krasko", "Text styles to use full font height"))
        .setHelpString(muse::trc("krasko",
            "Comma-separated list of text styles from the TextStyleType enum. Any text with one of"
            "these text styles will use the font's full height. This will fix vertical misalignment "
            "issues when different texts are displayed on the same line and some of them contain "
            "characters with ascenders / descenders but others do not. Multiline texts are affected "
            "as well. The caveat is that this could slightly increase the spacing below the texts "
            "such as on fingerings above the staff. When a text with a style NOT specified in "
            "this setting is drawn, its height will be the actual height of the characters "
            "it contains - without accounting for ascenders and descenders."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_textStylesToUseFullFontHeightChanged.send(val.toString());
        });

    sc.createSetting(krasko_module_name, KEY_FixFingeringTooCloseToStaffOnBeamedNotes)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Fix a fingering-to-staff distance issue on beamed notes"))
        .setHelpString(muse::trc("krasko",
            "When enabled, will fix an issue where fingering on beamed notes does not respect "
			"the min distance to staff. The fingering must be on the side of the beam(s) and "
			"the beam(s) must be far enough into the staff. In this case the fingering "
			"is placed too close to the staff."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_fixFingeringTooCloseToStaffOnBeamedNotesChanged.send(val.toBool());
        });

    sc.createSetting(krasko_module_name, KEY_RemoveExtraSpacingOnMultilineFingering)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Remove extra spacing on multi-line fingering"))
        .setHelpString(muse::trc("krasko",
            "When enabled, will remove extra spacing above or below multiline fingering. "
			"The more lines the fingering contains, the larger the spacing. The larger "
			"the font size, the larger the spacing too."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_removeExtraSpacingOnMultilineFingeringChanged.send(val.toBool());
        });
}

void AppShellConfiguration::updateRenamedKraskoSettings()
{
    // When a krasko setting is renamed, it should receive (inherit) the value of whichever predecessor
    // with an older name exists in the config file (if any). This ensures the renaming happens
    // seemlessly. At most one such predecessor should exist: as soon as a successor is added,
    // the predecessor will be deleted. Typically the predecessor will be the most recent one
    // if the user updates MuseScore Studio regularly. If not and the setting has been renamed more
    // than once between the user's previous verion of MuseScore Studio and the current version, then
    // the predecessor will not be the latest. For example: if setting A gets renamed to B and later
    // to C, below we should have a call to copy the value of A into C, then a call to copy B's value
    // into C since one user can have setting A in their config file, and another user may have B.
    // The user that has setting A has skipped all of the versions of MuseScore studio where B is present.
    //
    // We must preserve the type of the target setting (last param of the calls to copyValue below)
    // because the setting we are copying the value from no longer exists in the code: it is present
    // only in the user's config file and will be removed by deleteUnusedKraskoSettings().

    const Settings::Key* keyVerticalPanelsWidth = findKraskoSettingKey(KEY_VerticalPanelsWidth);
    if (keyVerticalPanelsWidth != nullptr) {
        settings()->copyValue(*keyVerticalPanelsWidth, "krasko/verticalPanelDefaultWidth", true);
    }
}

void AppShellConfiguration::deleteUnusedKraskoSettings()
{
    // Deletes all unused settings: those that are in the user's config file
    // but are no longer in this version of MuseScore Studio (i.e. are not in the code).

    Settings::Items allSettings = settings()->items();

    QList<Settings::Item> settingsToDelete;

    for (auto it = allSettings.cbegin(); it != allSettings.cend(); ++it) {
        const std::string& key = it->second.key.key;
        if (QString::fromStdString(key).startsWith("krasko/") && !existsKraskoSetting(key)) {
            settingsToDelete << it->second;
        }
    }

    for (auto& settingToDelete : settingsToDelete)
    {
        LOGW() << "Removing unused setting " << settingToDelete.key.key << "...";
        settings()->remove(settingToDelete.key);
    }
}

void AppShellConfiguration::notifyAboutChangedKraskoSettings()
{
    // Once krasko's settings have been created, we need to manually send out
    // few notifications to force dependent settings in other modules that were created
    // BEFORE krasko's settings to update. An example is the default font in uiconfiguration.
    m_mainMenuFontFollowsPreferencesFontChanged.send(mainMenuFontFollowsPreferencesFont());
    m_mainMenuFontSizeMultiplierChanged.send(mainMenuFontSizeMultiplier());
}

const Settings::Key* AppShellConfiguration::findKraskoSettingKey(const std::string& keyName) const
{
    if (m_settingsCreator != nullptr)
    {
        for (auto& currentKey : m_settingsCreator->allKeys())
        {
            if (currentKey->key == keyName)
            {
                return currentKey;
            }
        }
    }
    return nullptr;
}

bool AppShellConfiguration::existsKraskoSetting(const std::string& keyName) const
{
    return findKraskoSettingKey(keyName) != nullptr;
}

Val AppShellConfiguration::kraskoSettingValue(const std::string& keyName) const
{
    const Settings::Key* foundKey = findKraskoSettingKey(keyName);
    if (foundKey != nullptr)
    {
        return settings()->value(*foundKey);
    }
    return Val();
}

bool AppShellConfiguration::isValueInCsvList(const std::string& s, const std::string& csvList) const
{
    std::size_t pos;

    if (csvList == s)
        return true;

    if (csvList.find(s + ",") == 0)
        return true;

    if (csvList.find(s + ", ") == 0)
        return true;

    if (((pos = csvList.find("," + s)) != std::string::npos) && (pos == (csvList.length() - s.length() - 1)))
        return true;

    if (((pos = csvList.find(", " + s)) != std::string::npos) && (pos == (csvList.length() - s.length() - 2)))
        return true;

    if (csvList.find("," + s+ ",") != std::string::npos)
        return true;

    if (csvList.find(", " + s + ",") != std::string::npos)
        return true;

    return false;
}

bool AppShellConfiguration::autoRestoreSessionOnStart() const
{
    return kraskoSettingValue(KEY_AutoRestoreSessionOnStart).toBool();
}

bool AppShellConfiguration::focusExportButtonOnExportDialog() const
{
    return kraskoSettingValue(KEY_FocusExportButtonOnExportDialog).toBool();
}

muse::async::Channel<bool> AppShellConfiguration::focusExportButtonOnExportDialogChanged() const
{
    return m_focusExportButtonOnExportDialogChanged;
}

bool AppShellConfiguration::tabAndShiftTabNavigateBetweenControls() const
{
    return kraskoSettingValue(KEY_TabAndShiftTabNavigateBetweenControls).toBool();
}

bool AppShellConfiguration::changeActiveGripWithEditElementKey() const
{
    return kraskoSettingValue(KEY_ChangeActiveGripWithEditElementKey).toBool();
}

bool AppShellConfiguration::escKeyPreservesSelectionWhenEditing() const
{
    return kraskoSettingValue(KEY_EscKeyPreservesSelectionWhenEditing).toBool();
}

bool AppShellConfiguration::expandShowMoreSectionsInPropertiesPanel() const
{
	return kraskoSettingValue(KEY_ExpandShowMoreSectionsInPropertiesPanel).toBool();
}

muse::async::Channel<bool> AppShellConfiguration::expandShowMoreSectionsInPropertiesPanelChanged() const
{
    return m_expandShowMoreSectionsInPropertiesPanelChanged;
}

bool AppShellConfiguration::doNotHighlightDisabledItemsOnHover() const
{
	return kraskoSettingValue(KEY_DoNotHighlightDisabledItemsOnHover).toBool();
}

muse::async::Channel<bool> AppShellConfiguration::doNotHighlightDisabledItemsOnHoverChanged() const
{
    return m_doNotHighlightDisabledItemsOnHoverChanged;
}

bool AppShellConfiguration::showScrollbarOnScrollableDropDownLists() const
{
    return kraskoSettingValue(KEY_ShowScrollbarOnScrollableDropDownLists).toBool();
}

muse::async::Channel<bool> AppShellConfiguration::showScrollbarOnScrollableDropDownListsChanged() const
{
    return m_showScrollbarOnScrollableDropDownListsChanged;
}

mu::engraving::Color AppShellConfiguration::activeGripColor() const
{
    return mu::engraving::Color::fromQColor(kraskoSettingValue(KEY_ActiveGripColor).toQColor());
}

muse::async::Channel<mu::engraving::Color> AppShellConfiguration::activeGripColorChanged() const
{
    return m_activeGripColorChanged;
}

mu::engraving::Color AppShellConfiguration::scrollbarColor() const
{
    return mu::engraving::Color::fromQColor(kraskoSettingValue(KEY_ScrollbarColor).toQColor());
}

muse::async::Channel<mu::engraving::Color> AppShellConfiguration::scrollbarColorChanged() const
{
    return m_scrollbarColorChanged;
}

bool AppShellConfiguration::mainMenuFontFollowsPreferencesFont() const
{
    return kraskoSettingValue(KEY_MainMenuFontFollowsPreferencesFont).toBool();
}

muse::async::Channel<bool> AppShellConfiguration::mainMenuFontFollowsPreferencesFontChanged() const
{
    return m_mainMenuFontFollowsPreferencesFontChanged;
}

double AppShellConfiguration::mainMenuFontSizeMultiplier() const
{
    return kraskoSettingValue(KEY_MainMenuFontSizeMultiplier).toDouble();
}

muse::async::Channel<double> AppShellConfiguration::mainMenuFontSizeMultiplierChanged() const
{
    return m_mainMenuFontSizeMultiplierChanged;
}

bool AppShellConfiguration::enableHighPrecisionNudging() const
{
    return kraskoSettingValue(KEY_EnableHighPrecisionNudging).toBool();
}

bool AppShellConfiguration::showSameColorCheckBoxOnSelectMoreDialog() const
{
    return kraskoSettingValue(KEY_ShowSameColorCheckBoxOnSelectMoreDialog).toBool();
}

int AppShellConfiguration::verticalPanelsWidth() const
{
    return kraskoSettingValue(KEY_VerticalPanelsWidth).toInt();
}

muse::async::Channel<int> AppShellConfiguration::verticalPanelsWidthChanged() const
{
    return m_verticalPanelsWidthChanged;
}

int AppShellConfiguration::scrollDecelerationOfListsAndPanels() const
{
    return kraskoSettingValue(KEY_ScrollDecelerationOfListsAndPanels).toInt();
}

muse::async::Channel<int> AppShellConfiguration::scrollDecelerationOfListsAndPanelsChanged() const
{
    return m_scrollDecelerationOfListsAndPanelsChanged;
}

double AppShellConfiguration::stepForSpinControlsOnAppearanceTab() const
{
    return kraskoSettingValue(KEY_StepForSpinControlsOnAppearanceTab).toDouble();
}

muse::async::Channel<double> AppShellConfiguration::stepForSpinControlsOnAppearanceTabChanged() const
{
    return m_stepForSpinControlsOnAppearanceTabChanged;
}

std::string AppShellConfiguration::textStylesToUseFullFontHeight() const
{
    return kraskoSettingValue(KEY_TextStylesToUseFullFontHeight).toString();
}

muse::async::Channel<std::string> AppShellConfiguration::textStylesToUseFullFontHeightChanged() const
{
    return m_textStylesToUseFullFontHeightChanged;
}

bool AppShellConfiguration::fixFingeringTooCloseToStaffOnBeamedNotes() const
{
    return kraskoSettingValue(KEY_FixFingeringTooCloseToStaffOnBeamedNotes).toBool();
}

muse::async::Channel<bool> AppShellConfiguration::fixFingeringTooCloseToStaffOnBeamedNotesChanged() const
{
    return m_fixFingeringTooCloseToStaffOnBeamedNotesChanged;
}

bool AppShellConfiguration::removeExtraSpacingOnMultilineFingering() const
{
    return kraskoSettingValue(KEY_RemoveExtraSpacingOnMultilineFingering).toBool();
}

muse::async::Channel<bool> AppShellConfiguration::removeExtraSpacingOnMultilineFingeringChanged() const
{
    return m_removeExtraSpacingOnMultilineFingeringChanged;
}

// krasko end: KRASKO'S SETTINGS


bool AppShellConfiguration::hasCompletedFirstLaunchSetup() const
{
#ifdef Q_OS_WASM
    return true;
#else
    return settings()->value(HAS_COMPLETED_FIRST_LAUNCH_SETUP).toBool();
#endif
}

void AppShellConfiguration::setHasCompletedFirstLaunchSetup(bool has)
{
    settings()->setSharedValue(HAS_COMPLETED_FIRST_LAUNCH_SETUP, Val(has));
}

StartupModeType AppShellConfiguration::startupModeType() const
{
    return settings()->value(STARTUP_MODE_TYPE).toEnum<StartupModeType>();
}

void AppShellConfiguration::setStartupModeType(StartupModeType type)
{
    settings()->setSharedValue(STARTUP_MODE_TYPE, Val(type));
}

async::Notification AppShellConfiguration::startupModeTypeChanged() const
{
    return m_startupModeTypeChanged;
}

muse::io::path_t AppShellConfiguration::startupScorePath() const
{
    return settings()->value(STARTUP_SCORE_PATH).toString();
}

void AppShellConfiguration::setStartupScorePath(const muse::io::path_t& scorePath)
{
    settings()->setSharedValue(STARTUP_SCORE_PATH, Val(scorePath.toStdString()));
}

async::Notification AppShellConfiguration::startupScorePathChanged() const
{
    return m_startupScorePathChanged;
}

muse::io::path_t AppShellConfiguration::userDataPath() const
{
    return globalConfiguration()->userDataPath();
}

std::string AppShellConfiguration::handbookUrl() const
{
    std::string utm = utmParameters(UTM_MEDIUM_MENU);
    std::string languageCode = currentLanguageCode();

    QStringList params = {
        "tag=handbook",
        "locale=" + QString::fromStdString(languageCode),
        QString::fromStdString(utm)
    };

    return MUSESCORE_ONLINE_HANDBOOK_URL + "?" + params.join("&").toStdString();
}

std::string AppShellConfiguration::askForHelpUrl() const
{
    std::string languageCode = currentLanguageCode();

    QStringList params = {
        "locale=" + QString::fromStdString(languageCode)
    };

    return museScoreUrl() + MUSESCORE_ASK_FOR_HELP_URL_PATH + "?" + params.join("&").toStdString();
}

std::string AppShellConfiguration::museScoreUrl() const
{
    return globalConfiguration()->museScoreUrl();
}

std::string AppShellConfiguration::museScoreForumUrl() const
{
    return museScoreUrl() + MUSESCORE_FORUM_URL_PATH;
}

std::string AppShellConfiguration::museScoreContributionUrl() const
{
    return museScoreUrl() + MUSESCORE_CONTRIBUTE_URL_PATH;
}

std::string AppShellConfiguration::musicXMLLicenseUrl() const
{
    return MUSICXML_LICENSE_URL;
}

std::string AppShellConfiguration::musicXMLLicenseDeedUrl() const
{
    return MUSICXML_LICENSE_DEED_URL;
}

std::string AppShellConfiguration::museScoreVersion() const
{
    return String(application()->version().toString() + u"." + application()->build()).toStdString();
}

std::string AppShellConfiguration::museScoreRevision() const
{
    return application()->revision().toStdString();
}

bool AppShellConfiguration::isNotationNavigatorVisible() const
{
    return uiConfiguration()->isVisible(NOTATION_NAVIGATOR_VISIBLE_KEY, false);
}

void AppShellConfiguration::setIsNotationNavigatorVisible(bool visible) const
{
    uiConfiguration()->setIsVisible(NOTATION_NAVIGATOR_VISIBLE_KEY, visible);
}

muse::async::Notification AppShellConfiguration::isNotationNavigatorVisibleChanged() const
{
    return uiConfiguration()->isVisibleChanged(NOTATION_NAVIGATOR_VISIBLE_KEY);
}

bool AppShellConfiguration::needShowSplashScreen() const
{
    return settings()->value(SPLASH_SCREEN_VISIBLE_KEY).toBool();
}

void AppShellConfiguration::setNeedShowSplashScreen(bool show)
{
    settings()->setSharedValue(SPLASH_SCREEN_VISIBLE_KEY, Val(show));
}

const QString& AppShellConfiguration::preferencesDialogLastOpenedPageId() const
{
    return m_preferencesDialogCurrentPageId;
}

void AppShellConfiguration::setPreferencesDialogLastOpenedPageId(const QString& lastOpenedPageId)
{
    m_preferencesDialogCurrentPageId = lastOpenedPageId;
}

void AppShellConfiguration::startEditSettings()
{
    settings()->beginTransaction();
}

void AppShellConfiguration::applySettings()
{
    settings()->commitTransaction();
}

void AppShellConfiguration::rollbackSettings()
{
    settings()->rollbackTransaction();
}

void AppShellConfiguration::revertToFactorySettings(bool keepDefaultSettings, bool notifyAboutChanges, bool notifyOtherInstances) const
{
    settings()->reset(keepDefaultSettings, notifyAboutChanges, notifyOtherInstances);
}

muse::io::paths_t AppShellConfiguration::sessionProjectsPaths() const
{
    RetVal<ByteArray> retVal = readSessionState();
    if (!retVal.ret) {
        LOGE() << retVal.ret.toString();
        return {};
    }

    return parseSessionProjectsPaths(retVal.val.toQByteArrayNoCopy());
}

muse::Ret AppShellConfiguration::setSessionProjectsPaths(const muse::io::paths_t& paths)
{
    QJsonArray jsonArray;
    for (const muse::io::path_t& path : paths) {
        jsonArray << QJsonValue(path.toQString());
    }

    QByteArray data = QJsonDocument(jsonArray).toJson();
    return writeSessionState(data);
}

std::string AppShellConfiguration::utmParameters(const std::string& utmMedium) const
{
    return "utm_source=desktop&utm_medium=" + utmMedium
           + "&utm_content=" + application()->revision().toStdString()
           + "&utm_campaign=MuseScore" + application()->version().toStdString();
}

std::string AppShellConfiguration::currentLanguageCode() const
{
    QString languageCode = languagesConfiguration()->currentLanguageCode().val;
    QLocale locale(languageCode);

    return locale.bcp47Name().toStdString();
}

muse::io::path_t AppShellConfiguration::sessionDataPath() const
{
    return globalConfiguration()->userAppDataPath() + "/session";
}

muse::io::path_t AppShellConfiguration::sessionFilePath() const
{
    return sessionDataPath() + SESSION_FILE;
}

RetVal<muse::ByteArray> AppShellConfiguration::readSessionState() const
{
    muse::mi::ReadResourceLockGuard lock_guard(multiInstancesProvider.get(), SESSION_RESOURCE_NAME);
    return fileSystem()->readFile(sessionFilePath());
}

muse::Ret AppShellConfiguration::writeSessionState(const QByteArray& data)
{
    muse::mi::WriteResourceLockGuard lock_guard(multiInstancesProvider.get(), SESSION_RESOURCE_NAME);
    return fileSystem()->writeFile(sessionFilePath(), ByteArray::fromQByteArrayNoCopy(data));
}

muse::io::paths_t AppShellConfiguration::parseSessionProjectsPaths(const QByteArray& json) const
{
    QJsonParseError err;
    QJsonDocument jsodDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError || !jsodDoc.isArray()) {
        LOGE() << "failed parse, err: " << err.errorString();
        return {};
    }

    io::paths_t result;
    const QVariantList pathsList = jsodDoc.array().toVariantList();
    for (const QVariant& pathVal : pathsList) {
        muse::io::path_t path = pathVal.toString().toStdString();
        if (!path.empty()) {
            result.push_back(path);
        }
    }

    return result;
}
