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

static const Settings::Key KEY_AutoRestoreSessionAfterCrash(krasko_module_name, "krasko/autoRestoreSessionAfterCrash");

static const Settings::Key KEY_FocusExportButtonOnExportDialog(krasko_module_name, "krasko/focusExportButtonOnExportDialog");

static const Settings::Key KEY_NavNextPrevPanelNavigatesToNextPrevControl(krasko_module_name, "krasko/navNextPrevPanelNavigatesToNextPrevControl");

static const Settings::Key KEY_EditElementKeyCyclesThroughGrips(krasko_module_name, "krasko/editElementKeyCyclesThroughGrips");

static const Settings::Key KEY_EscapeKeyWhileEditingKeepsSelection(krasko_module_name, "krasko/escapeKeyWhileEditingKeepsSelection");

static const Settings::Key KEY_ShowSameColorCheckBoxOnSelectMoreDialog(krasko_module_name, "krasko/showSameColorCheckBoxOnSelectMoreDialog");

static const Settings::Key KEY_EnableAltModifierKeyForNudging(krasko_module_name, "krasko/enableAltModifierKeyForNudging");

static const Settings::Key KEY_EnableHighPrecisionNudging(krasko_module_name, "krasko/enableHighPrecisionNudging");

static const Settings::Key KEY_StepForSpinControlsOnAppearanceTab(krasko_module_name, "krasko/stepForSpinControlsOnAppearanceTab");

static const Settings::Key KEY_TextStylesToUseFontHeight(krasko_module_name, "krasko/textStylesToUseFontHeight");

static const Settings::Key KEY_InvisibleElementsColor(krasko_module_name, "krasko/invisibleElementsColor");

static const Settings::Key KEY_FixBeamedNotesFingeringTooCloseToStaff(krasko_module_name, "krasko/fixBeamedNotesFingeringTooCloseToStaff");

static const Settings::Key KEY_FixExtraSpacingOnMultilineFingering(krasko_module_name, "krasko/fixExtraSpacingOnMultilineFingering");

static const Settings::Key KEY_ScrollbarColor(krasko_module_name, "krasko/scrollbarColor");

static const Settings::Key KEY_ActiveGripColor(krasko_module_name, "krasko/activeGripColor");

static const Settings::Key KEY_FlickDeceleration(krasko_module_name, "krasko/flickDeceleration");

static const Settings::Key KEY_VerticalPanelDefaultWidth(krasko_module_name, "krasko/verticalPanelDefaultWidth");

static const Settings::Key KEY_ExpandShowMore(krasko_module_name, "krasko/expandShowMore");

static const Settings::Key KEY_HoverDisabledItems(krasko_module_name, "krasko/hoverDisabledItems");

static const Settings::Key KEY_MenuFontFollowsPreferencesFont(krasko_module_name, "krasko/menuFontFollowsPreferencesFont");

static const Settings::Key KEY_MenuFontSizeRatio(krasko_module_name, "krasko/menuFontSizeRatio");

static const Settings::Key KEY_ShowScrollbarOnDropDownLists(krasko_module_name, "krasko/showScrollbarOnDropDownLists");

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
    SettingsCreator sc(settings());

    sc.createSetting(KEY_AutoRestoreSessionAfterCrash)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Auto-restore session on startup"))
        .setExplanation(muse::trc("krasko",
            "When true, the previous/last session will be automatically restored after a crash without asking. "
            "For this to work, \"Preferences\" -> \"General\" -> \"Program start\" must be set to \"Continue last session\"."));

    sc.createSetting(KEY_FocusExportButtonOnExportDialog)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Focus the Export button on the Export dialog on open"))
        .setExplanation(muse::trc("krasko",
            "When true, the Export dialog will focus the Export button on open so the export can be completed "
            "quickly by simply pressing the ENTER key."));

    sc.createSetting(KEY_NavNextPrevPanelNavigatesToNextPrevControl)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "TAB navigates betwen controls"))
        .setExplanation(muse::trc("krasko",
            "Determines whether pressing the shortcut keys for \"nav-next-panel\" and \"nav-prev-panel\" (TAB and SHIFT+TAB "
            "by default) will navigate to the next/prev control (when true) or the next/prev panel (when false)."));

    sc.createSetting(KEY_EditElementKeyCyclesThroughGrips)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Edit key cycles through grips"))
        .setExplanation(muse::trc("krasko",
            "When true, pressing the \"Edit Element\" shortcut key (F2 by default) will cycle through an element's grips "
            "just like the TAB key if the element already has the grips displayed. This lets you press the \"Edit Element\" key "
            "once to diplay the grips/handles (the element must be selected) and then continue pressing the same key "
            "(instead of TAB) to activate the desired grip/handle to adjust it."));

    sc.createSetting(KEY_EscapeKeyWhileEditingKeepsSelection)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "ESC key keeps selection when editing"))
        .setExplanation(muse::trc("krasko",
            "When true, pressing the ESC key while editing an element, e.g. a slur or hairpin, will stop the editing "
            "but not deselect the element. This enables you to press the \"Edit Element\" key and continue editing "
            "until you are happy with the result. When the value is false, the element will also be deselected."));

    sc.createSetting(KEY_ShowSameColorCheckBoxOnSelectMoreDialog)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Show 'Same Color' checkbox on Select more dialog"))
        .setExplanation(muse::trc("krasko",
            "When true, will display a \"Same color\" checkbox on the Select More... dialogs (for note and non-note) so that "
            "the selection can be limited to the elements having the same color as the selected element."));

    sc.createSetting(KEY_EnableAltModifierKeyForNudging)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Enable the ALT key for nudging"))
        .setExplanation(muse::trc("krasko",
            "When true, the ALT key will be enabled to participate in shortcuts such as ALT+arrow keys "
            "for nudging grips/handles/elements."));

    sc.createSetting(KEY_EnableHighPrecisionNudging)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Enable high-precision nudging"))
        .setExplanation(muse::trc("krasko",
            "When true, nudging things such as grips, beams, etc. with the arrow keys will nudge by a smaller amount "
            "thus allowing for better control. If the ALT modifier is enabled, using ALT+arrow keys will nudge even less."));

    sc.createSetting(KEY_StepForSpinControlsOnAppearanceTab)
        .setDefaultValue(Val(0.5))
        .setDescription(muse::trc("krasko", "Step for spin controls on Appearance tab of Properties panel"))
        .setExplanation(muse::trc("krasko",
            "Specifies the step (amount) by which spin controls on the Appearance tab of the Properties panel such as the Offsets, "
            "Leading space, Min distance, etc. will change their values."));

    sc.createSetting(KEY_TextStylesToUseFontHeight)
        .setDefaultValue(Val(""))
        .setDescription(muse::trc("krasko", "Text styles to use font height"))
        .setExplanation(muse::trc("krasko",
            "Comma-separated list of text styles. The height of any text (i.e. text object) with any of those text styles "
            "will use the font's full height. This will fix vertical misalignment issues when different texts are displayed "
            "on the same line/row and some of the texts contain characters with ascenders/descenders and the others do not. "
            "Multiline texts are affected as well. The caveat is that this could slightly increase the spacing below the texts "
            "such as on fingerings above the staff. When a text with style NOT specified in this setting is drawn, its height "
            "will be the actual height of the characters of the text - the so called tight bounding rectange. "
            "The allowed values are the TextStyleType enum values."));

    sc.createSetting(KEY_InvisibleElementsColor)
        .setDefaultValue(Val(QColor("#808080")))
        .setDescription(muse::trc("krasko", "Invisible elements color"))
        .setExplanation(muse::trc("krasko",
            "The color to use for the invisible elements."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_invisibleElementsColorChanged.send(mu::engraving::Color::fromQColor(val.toQColor()));
        });

    sc.createSetting(KEY_FixBeamedNotesFingeringTooCloseToStaff)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Fix fingering-to-staff issue on beamed notes"))
        .setExplanation(muse::trc("krasko",
            "When true, will fix an issue where fingering on beamed notes does not respect the min distance to staff. The fingering "
            "must be on the side of the beam(s) and the beams should be far enough into the staff (from the edge of the staff). "
            "In this case the fingering is placed too close to the staff."));

    sc.createSetting(KEY_FixExtraSpacingOnMultilineFingering)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Fix extra line spacing on multi-line fingering"))
        .setExplanation(muse::trc("krasko",
            "When true, will fix an issue where multiline fingering has extra spacing above/below. The more lines, the larger "
            "the spacing. The larger the font size, the larger the spacing too."));

    sc.createSetting(KEY_ScrollbarColor)
        .setDefaultValue(Val(QColor("")))
        .setDescription(muse::trc("krasko", "Scrollbar color"))
        .setExplanation(muse::trc("krasko",
            "The color to use for the scrollbars."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_scrollbarColorChanged.send(mu::engraving::Color::fromQColor(val.toQColor()));
        });

    sc.createSetting(KEY_ActiveGripColor)
        .setDefaultValue(Val(mu::engraving::Color("#A0A0A4").toQColor()))
        .setDescription(muse::trc("krasko", "Active grip color"))
        .setExplanation(muse::trc("krasko",
            "The color to use for the active grip."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_activeGripColorChanged.send(mu::engraving::Color::fromQColor(val.toQColor()));
        });

    sc.createSetting(KEY_FlickDeceleration)
        .setDefaultValue(Val(12000))
        .setDescription(muse::trc("krasko", "Scroll deceleration"))
        .setExplanation(muse::trc("krasko",
            "The deceleration to use when scrolling flickable controls (palettes, properties panel, etc.). The higher the value, "
            "the sooner the scrolling will stop when the user stops scrolling. Lower values will make the scrolling last longer."));

    sc.createSetting(KEY_VerticalPanelDefaultWidth)
        .setDefaultValue(Val(300))
        .setDescription(muse::trc("krasko", "Width of the vertical panels"))
        .setExplanation(muse::trc("krasko",
            "The default width of the vertical panels such as the Palettes, Instruments, Properties, Selection filter."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_verticalPanelDefaultWidthChanged.send(val.toInt());
        });

    sc.createSetting(KEY_ExpandShowMore)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Expand 'Show more' sections"))
        .setExplanation(muse::trc("krasko",
            "When true, all \"Show more\" sections will be initially expanded. Later on, the user can collapse them."));

    sc.createSetting(KEY_HoverDisabledItems)
        .setDefaultValue(Val(true))
        .setDescription(muse::trc("krasko", "Hover disabled items"))
        .setExplanation(muse::trc("krasko",
            "When true, disabled menu and list items will be highlighted on hover. When false, they will not be highlighted "
            "when the mouse passes over them to create a better perception of the disabledness of the items."));

    sc.createSetting(KEY_MenuFontFollowsPreferencesFont)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Menu font will follow Preferences font"))
        .setExplanation(muse::trc("krasko",
            "When true, the font of the main menu will be the same as the font of all other UI elements set in the preferences."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_menuFontFollowsPreferencesFontChanged.send(val.toBool());
        });

    sc.createSetting(KEY_MenuFontSizeRatio)
        .setDefaultValue(Val("1/1"))
        .setDescription(muse::trc("krasko", "Menu font size ratio"))
        .setExplanation(muse::trc("krasko",
            "Specifies a ratio, e.g. \"9/10\" that will be multiplied by the font size set in the preferences "
            "for the rest of the UI elements to obtain the size of the main menubar."))
        .valueChanged().onReceive(this, [this](const Val& val) {
            m_menuFontSizeRatioChanged.send(val.toString());
        });

    sc.createSetting(KEY_ShowScrollbarOnDropDownLists)
        .setDefaultValue(Val(false))
        .setDescription(muse::trc("krasko", "Show scrollbar on scrollable drop-down lists"))
        .setExplanation(muse::trc("krasko",
            "When true, the scrollbar on scrollable drop-down lists will always be visible. "
            "When false, it will only appear when the scrollbar is hovered."));
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
    return settings()->value(KEY_AutoRestoreSessionAfterCrash).toBool();
}

bool AppShellConfiguration::focusExportButtonOnExportDialog() const
{
    return settings()->value(KEY_FocusExportButtonOnExportDialog).toBool();
}

bool AppShellConfiguration::navNextPrevPanelNavigatesToNextPrevControl() const
{
    return settings()->value(KEY_NavNextPrevPanelNavigatesToNextPrevControl).toBool();
}

bool AppShellConfiguration::editElementKeyCyclesThroughGrips() const
{
    return settings()->value(KEY_EditElementKeyCyclesThroughGrips).toBool();
}

bool AppShellConfiguration::escapeKeyWhileEditingKeepsSelection() const
{
    return settings()->value(KEY_EscapeKeyWhileEditingKeepsSelection).toBool();
}

bool AppShellConfiguration::showSameColorCheckBoxOnSelectMoreDialog() const
{
    return settings()->value(KEY_ShowSameColorCheckBoxOnSelectMoreDialog).toBool();
}

bool AppShellConfiguration::enableAltModifierKeyForNudging() const
{
    return settings()->value(KEY_EnableAltModifierKeyForNudging).toBool();
}

bool AppShellConfiguration::enableHighPrecisionNudging() const
{
    return settings()->value(KEY_EnableHighPrecisionNudging).toBool();
}

double AppShellConfiguration::stepForSpinControlsOnAppearanceTab() const
{
    return settings()->value(KEY_StepForSpinControlsOnAppearanceTab).toDouble();
}

bool AppShellConfiguration::textStylesToUseFontHeight(const std::string csvTextStyles) const
{
    std::string value = settings()->value(KEY_TextStylesToUseFontHeight).toString();
    return isStrInCSVString(csvTextStyles, value);
}

mu::engraving::Color AppShellConfiguration::invisibleElementsColor() const
{
    return mu::engraving::Color::fromQColor(settings()->value(KEY_InvisibleElementsColor).toQColor());
}

muse::async::Channel<mu::engraving::Color> AppShellConfiguration::invisibleElementsColorChanged() const
{
    return m_invisibleElementsColorChanged;
}

bool AppShellConfiguration::fixBeamedNotesFingeringTooCloseToStaff() const
{
    return settings()->value(KEY_FixBeamedNotesFingeringTooCloseToStaff).toBool();
}

bool AppShellConfiguration::fixExtraSpacingOnMultilineFingering() const
{
    return settings()->value(KEY_FixExtraSpacingOnMultilineFingering).toBool();
}

mu::engraving::Color AppShellConfiguration::scrollbarColor() const
{
    return mu::engraving::Color::fromQColor(settings()->value(KEY_ScrollbarColor).toQColor());
}

muse::async::Channel<mu::engraving::Color> AppShellConfiguration::scrollbarColorChanged() const
{
    return m_scrollbarColorChanged;
}

mu::engraving::Color AppShellConfiguration::activeGripColor() const
{
    return mu::engraving::Color::fromQColor(settings()->value(KEY_ActiveGripColor).toQColor());
}

muse::async::Channel<mu::engraving::Color> AppShellConfiguration::activeGripColorChanged() const
{
    return m_activeGripColorChanged;
}

int AppShellConfiguration::flickDeceleration() const
{
    return settings()->value(KEY_FlickDeceleration).toInt();
}

int AppShellConfiguration::verticalPanelDefaultWidth() const
{
    return settings()->value(KEY_VerticalPanelDefaultWidth).toInt();
}

muse::async::Channel<int> AppShellConfiguration::verticalPanelDefaultWidthChanged() const
{
    return m_verticalPanelDefaultWidthChanged;
}

bool AppShellConfiguration::expandShowMore() const
{
    return settings()->value(KEY_ExpandShowMore).toBool();
}

bool AppShellConfiguration::hoverDisabledItems() const
{
    return settings()->value(KEY_HoverDisabledItems).toBool();
}

bool AppShellConfiguration::menuFontFollowsPreferencesFont() const
{
    return settings()->value(KEY_MenuFontFollowsPreferencesFont).toBool();
}

muse::async::Channel<bool> AppShellConfiguration::menuFontFollowsPreferencesFontChanged() const
{
    return m_menuFontFollowsPreferencesFontChanged;
}

std::string AppShellConfiguration::menuFontSizeRatio() const
{
    return settings()->value(KEY_MenuFontSizeRatio).toString();
}

muse::async::Channel<std::string> AppShellConfiguration::menuFontSizeRatioChanged() const
{
    return m_menuFontSizeRatioChanged;
}

bool AppShellConfiguration::showScrollbarOnDropDownLists() const
{
    return settings()->value(KEY_ShowScrollbarOnDropDownLists).toBool();
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
