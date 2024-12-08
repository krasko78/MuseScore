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

// --- KRASKO'S SETTINGS START ---

static constexpr char krasko_module_name[] = "krasko";

static constexpr char KEY_AutoRestoreSessionAfterCrash[] = "krasko/autoRestoreSessionAfterCrash";

static constexpr char KEY_FocusExportButtonOnExportDialog[] = "krasko/focusExportButtonOnExportDialog";

static constexpr char KEY_NavNextPrevPanelNavigatesToNextPrevControl[] = "krasko/navNextPrevPanelNavigatesToNextPrevControl";

static constexpr char KEY_EditElementKeyCyclesThroughGrips[] = "krasko/editElementKeyCyclesThroughGrips";

static constexpr char KEY_EscapeKeyWhileEditingKeepsSelection[] = "krasko/escapeKeyWhileEditingKeepsSelection";

static constexpr char KEY_ShowSameColorCheckBoxOnSelectMoreDialog[] = "krasko/showSameColorCheckBoxOnSelectMoreDialog";

static constexpr char KEY_EnableAltModifierKeyForNudging[] = "krasko/enableAltModifierKeyForNudging";

static constexpr char KEY_EnableHighPrecisionNudging[] = "krasko/enableHighPrecisionNudging";

static constexpr char KEY_StepForSpinControlsOnAppearanceTab[] = "krasko/stepForSpinControlsOnAppearanceTab";

static constexpr char KEY_TextStylesToUseFontHeight[] = "krasko/textStylesToUseFontHeight";

static constexpr char KEY_InvisibleElementsColor[] = "krasko/invisibleElementsColor";

static constexpr char KEY_FixBeamedNotesFingeringTooCloseToStaff[] = "krasko/fixBeamedNotesFingeringTooCloseToStaff";

static constexpr char KEY_FixExtraSpacingOnMultilineFingering[] = "krasko/fixExtraSpacingOnMultilineFingering";

static constexpr char KEY_ScrollbarColor[] = "krasko/scrollbarColor";

static constexpr char KEY_ActiveGripColor[] = "krasko/activeGripColor";

static constexpr char KEY_FlickDeceleration[] = "krasko/flickDeceleration";

static constexpr char KEY_VerticalPanelsWidth[] = "krasko/verticalPanelsWidth";

static constexpr char KEY_ExpandShowMore[] = "krasko/expandShowMore";

static constexpr char KEY_HoverDisabledItems[] = "krasko/hoverDisabledItems";

static constexpr char KEY_MenuFontFollowsPreferencesFont[] = "krasko/menuFontFollowsPreferencesFont";

static constexpr char KEY_MenuFontSizeRatio[] = "krasko/menuFontSizeRatio";

static constexpr char KEY_ShowScrollbarOnDropDownLists[] = "krasko/showScrollbarOnDropDownLists";

// --- KRASKO'S SETTINGS END ---


static const Settings::Key HAS_COMPLETED_FIRST_LAUNCH_SETUP(module_name, "application/hasCompletedFirstLaunchSetup");

static const Settings::Key STARTUP_MODE_TYPE(module_name, "application/startup/modeStart");
static const Settings::Key STARTUP_SCORE_PATH(module_name, "application/startup/startScore");

static const std::string MUSESCORE_ONLINE_HANDBOOK_URL_PATH("/handbook/4");
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
    qDeleteAll(m_kraskoSettingsKeys);
}

void AppShellConfiguration::init()
{
    initKraskoSettings(); // krasko

    settings()->setDefaultValue(HAS_COMPLETED_FIRST_LAUNCH_SETUP, Val(false));

    settings()->setDefaultValue(STARTUP_MODE_TYPE, Val(StartupModeType::StartEmpty));
    settings()->setDefaultValue(STARTUP_SCORE_PATH, Val(projectConfiguration()->myFirstProjectPath().toStdString()));

    fileSystem()->makePath(sessionDataPath());
}

// --- KRASKO'S SETTINGS START ---

void AppShellConfiguration::initKraskoSettings()
{
    createKraskoSettings();
    updateRenamedKraskoSettings();
    deleteUnusedKraskoSettings();
}

void AppShellConfiguration::createKraskoSettings()
{
    SettingsCreator sc(settings());

    sc.createSetting(krasko_module_name, KEY_AutoRestoreSessionAfterCrash)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Auto-restore session on startup"))
        .setHelpString(muse::trc("krasko",
            "When true, the previous/last session will be automatically restored after a crash without asking. "
            "For this to work, \"Preferences\" -> \"General\" -> \"Program start\" must be set to \"Continue last session\"."));

    sc.createSetting(krasko_module_name, KEY_FocusExportButtonOnExportDialog)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Focus the Export button on the Export dialog on open"))
        .setHelpString(muse::trc("krasko",
            "When true, the Export dialog will focus the Export button on open so the export can be completed "
            "quickly by simply pressing the ENTER key."));

    sc.createSetting(krasko_module_name, KEY_NavNextPrevPanelNavigatesToNextPrevControl)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "TAB navigates between controls"))
        .setHelpString(muse::trc("krasko",
            "Determines whether pressing the shortcut keys for \"nav-next-panel\" and \"nav-prev-panel\" (TAB and SHIFT+TAB "
            "by default) will navigate to the next/prev control (when true) or the next/prev panel (when false)."));

    sc.createSetting(krasko_module_name, KEY_EditElementKeyCyclesThroughGrips)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Edit key cycles through grips"))
        .setHelpString(muse::trc("krasko",
            "When true, pressing the \"Edit Element\" shortcut key (F2 by default) will cycle through an element's grips "
            "just like the TAB key if the element already has the grips displayed. This lets you press the \"Edit Element\" key "
            "once to diplay the grips/handles (the element must be selected) and then continue pressing the same key "
            "(instead of TAB) to activate the desired grip/handle to adjust it."));

    sc.createSetting(krasko_module_name, KEY_EscapeKeyWhileEditingKeepsSelection)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "ESC key keeps selection when editing"))
        .setHelpString(muse::trc("krasko",
            "When true, pressing the ESC key while editing an element, e.g. a slur or hairpin, will stop the editing "
            "but not deselect the element. This enables you to press the \"Edit Element\" key and continue editing "
            "until you are happy with the result. When the value is false, the element will also be deselected."));

    sc.createSetting(krasko_module_name, KEY_ShowSameColorCheckBoxOnSelectMoreDialog)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Show 'Same Color' checkbox on Select more dialog"))
        .setHelpString(muse::trc("krasko",
            "When true, will display a \"Same color\" checkbox on the Select More... dialogs (for note and non-note) so that "
            "the selection can be limited to the elements having the same color as the selected element."));

    sc.createSetting(krasko_module_name, KEY_EnableAltModifierKeyForNudging)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Enable the ALT key for nudging"))
        .setHelpString(muse::trc("krasko",
            "When true, the ALT key will be enabled to participate in shortcuts such as ALT+arrow keys "
            "for nudging grips/handles/elements."));

    sc.createSetting(krasko_module_name, KEY_EnableHighPrecisionNudging)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Enable high-precision nudging"))
        .setHelpString(muse::trc("krasko",
            "When true, nudging things such as grips, beams, etc. with the arrow keys will nudge by a smaller amount "
            "thus allowing for better control. If the ALT modifier is enabled, using ALT+arrow keys will nudge even less."));

    sc.createSetting(krasko_module_name, KEY_StepForSpinControlsOnAppearanceTab)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(0.5))
        .setDescription(muse::trc("krasko", "Step for spin controls on Appearance tab of Properties panel"))
        .setHelpString(muse::trc("krasko",
            "Specifies the step (amount) by which spin controls on the Appearance tab of the Properties panel such as the Offsets, "
            "Leading space, Min distance, etc. will change their values."));

    sc.createSetting(krasko_module_name, KEY_TextStylesToUseFontHeight)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(""))
        .setDescription(muse::trc("krasko", "Text styles to use font height"))
        .setHelpString(muse::trc("krasko",
            "Comma-separated list of text styles. The height of any text (i.e. text object) with any of those text styles "
            "will use the font's full height. This will fix vertical misalignment issues when different texts are displayed "
            "on the same line/row and some of the texts contain characters with ascenders/descenders and the others do not. "
            "Multiline texts are affected as well. The caveat is that this could slightly increase the spacing below the texts "
            "such as on fingerings above the staff. When a text with style NOT specified in this setting is drawn, its height "
            "will be the actual height of the characters of the text - the so called tight bounding rectange. "
            "The allowed values are the TextStyleType enum values."));

    sc.createSetting(krasko_module_name, KEY_InvisibleElementsColor)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(QColor("#808080")))
        .setDescription(muse::trc("krasko", "Invisible elements color"))
        .setHelpString(muse::trc("krasko",
            "The color to use for the invisible elements."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_invisibleElementsColorChanged.send(mu::engraving::Color::fromQColor(val.toQColor()));
        });

    sc.createSetting(krasko_module_name, KEY_FixBeamedNotesFingeringTooCloseToStaff)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Fix fingering-to-staff issue on beamed notes"))
        .setHelpString(muse::trc("krasko",
            "When true, will fix an issue where fingering on beamed notes does not respect the min distance to staff. The fingering "
            "must be on the side of the beam(s) and the beams should be far enough into the staff (from the edge of the staff). "
            "In this case the fingering is placed too close to the staff."));

    sc.createSetting(krasko_module_name, KEY_FixExtraSpacingOnMultilineFingering)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Fix extra line spacing on multi-line fingering"))
        .setHelpString(muse::trc("krasko",
            "When true, will fix an issue where multiline fingering has extra spacing above/below. The more lines, the larger "
            "the spacing. The larger the font size, the larger the spacing too."));

    sc.createSetting(krasko_module_name, KEY_ScrollbarColor)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(QColor("")))
        .setDescription(muse::trc("krasko", "Scrollbar color"))
        .setHelpString(muse::trc("krasko",
            "The color to use for the scrollbars."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_scrollbarColorChanged.send(mu::engraving::Color::fromQColor(val.toQColor()));
        });

    sc.createSetting(krasko_module_name, KEY_ActiveGripColor)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(mu::engraving::Color("#A0A0A4").toQColor()))
        .setDescription(muse::trc("krasko", "Active grip color"))
        .setHelpString(muse::trc("krasko",
            "The color to use for the active grip."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_activeGripColorChanged.send(mu::engraving::Color::fromQColor(val.toQColor()));
        });

    sc.createSetting(krasko_module_name, KEY_FlickDeceleration)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(12000))
        .setDescription(muse::trc("krasko", "Scroll deceleration"))
        .setHelpString(muse::trc("krasko",
            "The deceleration to use when scrolling flickable controls (palettes, properties panel, etc.). The higher the value, "
            "the sooner the scrolling will stop when the user stops scrolling. Lower values will make the scrolling last longer."));

    sc.createSetting(krasko_module_name, KEY_VerticalPanelsWidth)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(300))
        .setDescription(muse::trc("krasko", "Width of the vertical panels"))
        .setHelpString(muse::trc("krasko",
            "The width of the vertical panels such as the Palettes, Instruments, Properties, Selection filter."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_verticalPanelsWidthChanged.send(val.toInt());
        });

    sc.createSetting(krasko_module_name, KEY_ExpandShowMore)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Expand 'Show more' sections"))
        .setHelpString(muse::trc("krasko",
            "When true, all \"Show more\" sections will be initially expanded. Later on, the user can collapse them."));

    sc.createSetting(krasko_module_name, KEY_HoverDisabledItems)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(true))
        .setDescription(muse::trc("krasko", "Hover disabled items"))
        .setHelpString(muse::trc("krasko",
            "When true, disabled menu and list items will be highlighted on hover. When false, they will not be highlighted "
            "when the mouse passes over them to create a better perception of the disabledness of the items."));

    sc.createSetting(krasko_module_name, KEY_MenuFontFollowsPreferencesFont)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Menu font will follow Preferences font"))
        .setHelpString(muse::trc("krasko",
            "When true, the font of the main menu will be the same as the font of all other UI elements set in the preferences."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_menuFontFollowsPreferencesFontChanged.send(val.toBool());
        });

    sc.createSetting(krasko_module_name, KEY_MenuFontSizeRatio)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val("1/1"))
        .setDescription(muse::trc("krasko", "Menu font size ratio"))
        .setHelpString(muse::trc("krasko",
            "Specifies a ratio, e.g. \"9/10\" that will be multiplied by the font size set in the preferences "
            "for the rest of the UI elements to obtain the size of the main menubar."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_menuFontSizeRatioChanged.send(val.toString());
        });

    sc.createSetting(krasko_module_name, KEY_ShowScrollbarOnDropDownLists)
        .addKeyTo(m_kraskoSettingsKeys)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Show scrollbar on scrollable drop-down lists"))
        .setHelpString(muse::trc("krasko",
            "When true, the scrollbar on scrollable drop-down lists will always be visible. "
            "When false, it will only appear when the scrollbar is hovered."));
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

const Settings::Key* AppShellConfiguration::findKraskoSettingKey(const std::string& keyName) const
{
    for (auto& currentKey : m_kraskoSettingsKeys)
    {
        if (currentKey->key == keyName)
        {
            return currentKey;
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

bool AppShellConfiguration::isStrInCSVString(std::string s, std::string csvStr) const
{
    std::size_t pos;

    if (csvStr == s)
        return true;

    if (csvStr.find(s + ",") == 0)
        return true;

    if (csvStr.find(s + ", ") == 0)
        return true;

    if (((pos = csvStr.find("," + s)) != std::string::npos) && (pos == (csvStr.length() - s.length() - 1)))
        return true;

    if (((pos = csvStr.find(", " + s)) != std::string::npos) && (pos == (csvStr.length() - s.length() - 2)))
        return true;

    if (csvStr.find("," + s+ ",") != std::string::npos)
        return true;

    if (csvStr.find(", " + s + ",") != std::string::npos)
        return true;

    return false;
}

bool AppShellConfiguration::autoRestoreSessionAfterCrash() const
{
    return kraskoSettingValue(KEY_AutoRestoreSessionAfterCrash).toBool();
}

bool AppShellConfiguration::focusExportButtonOnExportDialog() const
{
    return kraskoSettingValue(KEY_FocusExportButtonOnExportDialog).toBool();
}

bool AppShellConfiguration::navNextPrevPanelNavigatesToNextPrevControl() const
{
    return kraskoSettingValue(KEY_NavNextPrevPanelNavigatesToNextPrevControl).toBool();
}

bool AppShellConfiguration::editElementKeyCyclesThroughGrips() const
{
    return kraskoSettingValue(KEY_EditElementKeyCyclesThroughGrips).toBool();
}

bool AppShellConfiguration::escapeKeyWhileEditingKeepsSelection() const
{
    return kraskoSettingValue(KEY_EscapeKeyWhileEditingKeepsSelection).toBool();
}

bool AppShellConfiguration::showSameColorCheckBoxOnSelectMoreDialog() const
{
    return kraskoSettingValue(KEY_ShowSameColorCheckBoxOnSelectMoreDialog).toBool();
}

bool AppShellConfiguration::enableAltModifierKeyForNudging() const
{
    return kraskoSettingValue(KEY_EnableAltModifierKeyForNudging).toBool();
}

bool AppShellConfiguration::enableHighPrecisionNudging() const
{
    return kraskoSettingValue(KEY_EnableHighPrecisionNudging).toBool();
}

double AppShellConfiguration::stepForSpinControlsOnAppearanceTab() const
{
    return kraskoSettingValue(KEY_StepForSpinControlsOnAppearanceTab).toDouble();
}

bool AppShellConfiguration::textStylesToUseFontHeight(const std::string csvTextStyles) const
{
    std::string value = kraskoSettingValue(KEY_TextStylesToUseFontHeight).toString();
    return isStrInCSVString(csvTextStyles, value);
}

mu::engraving::Color AppShellConfiguration::invisibleElementsColor() const
{
    return mu::engraving::Color::fromQColor(kraskoSettingValue(KEY_InvisibleElementsColor).toQColor());
}

muse::async::Channel<mu::engraving::Color> AppShellConfiguration::invisibleElementsColorChanged() const
{
    return m_invisibleElementsColorChanged;
}

bool AppShellConfiguration::fixBeamedNotesFingeringTooCloseToStaff() const
{
    return kraskoSettingValue(KEY_FixBeamedNotesFingeringTooCloseToStaff).toBool();
}

bool AppShellConfiguration::fixExtraSpacingOnMultilineFingering() const
{
    return kraskoSettingValue(KEY_FixExtraSpacingOnMultilineFingering).toBool();
}

mu::engraving::Color AppShellConfiguration::scrollbarColor() const
{
    return mu::engraving::Color::fromQColor(kraskoSettingValue(KEY_ScrollbarColor).toQColor());
}

muse::async::Channel<mu::engraving::Color> AppShellConfiguration::scrollbarColorChanged() const
{
    return m_scrollbarColorChanged;
}

mu::engraving::Color AppShellConfiguration::activeGripColor() const
{
    return mu::engraving::Color::fromQColor(kraskoSettingValue(KEY_ActiveGripColor).toQColor());
}

muse::async::Channel<mu::engraving::Color> AppShellConfiguration::activeGripColorChanged() const
{
    return m_activeGripColorChanged;
}

int AppShellConfiguration::flickDeceleration() const
{
    return kraskoSettingValue(KEY_FlickDeceleration).toInt();
}

int AppShellConfiguration::verticalPanelsWidth() const
{
    return kraskoSettingValue(KEY_VerticalPanelsWidth).toInt();
}

muse::async::Channel<int> AppShellConfiguration::verticalPanelsWidthChanged() const
{
    return m_verticalPanelsWidthChanged;
}

bool AppShellConfiguration::expandShowMore() const
{
    return kraskoSettingValue(KEY_ExpandShowMore).toBool();
}

bool AppShellConfiguration::hoverDisabledItems() const
{
    return kraskoSettingValue(KEY_HoverDisabledItems).toBool();
}

bool AppShellConfiguration::menuFontFollowsPreferencesFont() const
{
    return kraskoSettingValue(KEY_MenuFontFollowsPreferencesFont).toBool();
}

muse::async::Channel<bool> AppShellConfiguration::menuFontFollowsPreferencesFontChanged() const
{
    return m_menuFontFollowsPreferencesFontChanged;
}

std::string AppShellConfiguration::menuFontSizeRatio() const
{
    return kraskoSettingValue(KEY_MenuFontSizeRatio).toString();
}

muse::async::Channel<std::string> AppShellConfiguration::menuFontSizeRatioChanged() const
{
    return m_menuFontSizeRatioChanged;
}

bool AppShellConfiguration::showScrollbarOnDropDownLists() const
{
    return kraskoSettingValue(KEY_ShowScrollbarOnDropDownLists).toBool();
}

// --- KRASKO'S SETTINGS END ---


bool AppShellConfiguration::hasCompletedFirstLaunchSetup() const
{
    return settings()->value(HAS_COMPLETED_FIRST_LAUNCH_SETUP).toBool();
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

muse::io::path_t AppShellConfiguration::startupScorePath() const
{
    return settings()->value(STARTUP_SCORE_PATH).toString();
}

void AppShellConfiguration::setStartupScorePath(const muse::io::path_t& scorePath)
{
    settings()->setSharedValue(STARTUP_SCORE_PATH, Val(scorePath.toStdString()));
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

    return museScoreUrl() + MUSESCORE_ONLINE_HANDBOOK_URL_PATH + "?" + params.join("&").toStdString();
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

void AppShellConfiguration::revertToFactorySettings(bool keepDefaultSettings, bool notifyAboutChanges) const
{
    settings()->reset(keepDefaultSettings, notifyAboutChanges);
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
