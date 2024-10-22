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

// --- HIDDEN SETTINGS START ---

static const Settings::Key AUTO_RESTORE_SESSION_AFTER_CRASH(module_name, "krasko/AutoRestoreSessionAfterCrash");
        static constexpr bool AUTO_RESTORE_SESSION_AFTER_CRASH_DEFAULT = true;
        //  When true, the previous/last session will be automatically restored after a crash without asking.
        //  For this to work, "Preferences" -> "General" -> "Program start" must be set to "Continue last session".

static const Settings::Key FOCUS_EXPORT_BUTTON_ON_EXPORT_DIALOG(module_name, "krasko/FocusExportButtonOnExportDialog");
        static constexpr bool FOCUS_EXPORT_BUTTON_ON_EXPORT_DIALOG_DEFAULT = true;
        //  When true, the Export dialog will focus the Export button on open so the export can be completed
        //  quickly by simply pressing the ENTER key.

static const Settings::Key NAV_NEXT_PREV_PANEL_NAVIGATES_TO_NEXT_PREV_CONTROL(module_name, "krasko/NavNextPrevPanelNavigatesToNextPrevControl");
        static constexpr bool NAV_NEXT_PREV_PANEL_NAVIGATES_TO_NEXT_PREV_CONTROL_DEFAULT = true;
        //  Determines whether pressing the shortcut keys for "nav-next-panel" and "nav-prev-panel" (TAB and SHIFT+TAB
        //  by default) will navigate to the next/prev control (when true) or the next/prev panel (when false).

static const Settings::Key EDIT_ELEMENT_KEY_CYCLES_THROUGH_GRIPS(module_name, "krasko/EditElementKeyCyclesThroughGrips");
        static constexpr bool EDIT_ELEMENT_KEY_CYCLES_THROUGH_GRIPS_DEFAULT = true;
        //  When true, pressing the "Edit Element" shortcut key (F2 by default) will cycle through an element's grips
        //  just like the TAB key if the element already has the grips displayed. This lets you press the "Edit Element" key
        //  once to diplay the grips/handles (the element must be selected) and then continue pressing the same key
        //  (instead of TAB) to activate the desired grip/handle to adjust it.

static const Settings::Key ESCAPE_KEY_WHILE_EDITING_KEEPS_SELECTION(module_name, "krasko/EscapeKeyWhileEditingKeepsSelection");
        static constexpr bool ESCAPE_KEY_WHILE_EDITING_KEEPS_SELECTION_DEFAULT = true;
        //  When true, pressing the ESC key while editing an element, e.g. a slur or hairpin, will stop the editing
        //  but not deselect the element. This enables you to press the "Edit Element" key and continue editing
        //  until you are happy with the result. When the value is false, the element will also be deselected.

static const Settings::Key SHOW_SAME_COLOR_CHECKBOX_ON_SELECT_MORE_DIALOG(module_name, "krasko/ShowSameColorCheckBoxOnSelectMoreDialog");
        static constexpr bool SHOW_SAME_COLOR_CHECKBOX_ON_SELECT_MORE_DIALOG_DEFAULT = true;
        //  When true, will display a "Same color" checkbox on the Select More... dialogs (for note and non-note) so that
        //  the selection can be limited to the elements having the same color as the selected element.

static const Settings::Key ENABLE_ALT_MODIFIER_KEY_FOR_NUDGING(module_name, "krasko/EnableAltModifierKeyForNudging");
        static constexpr bool ENABLE_ALT_MODIFIER_KEY_FOR_NUDGING_DEFAULT = true;
        //  When true, the ALT key will be enabled to participate in shortcuts such as ALT+arrow keys
        //  for nudging grips/handles/elements.

static const Settings::Key ENABLE_HIGH_PRECISION_NUDGING(module_name, "krasko/EnableHighPrecisionNudging");
        static constexpr bool ENABLE_HIGH_PRECISION_NUDGING_DEFAULT = false;
        //  When true, nudging things such as grips, beams, etc. with the arrow keys will nudge by a smaller amount
        //  thus allowing for better control. If the ALT modifier is enabled, using ALT+arrow keys will nudge even less.

static const Settings::Key STEP_FOR_SPIN_CONTROLS_ON_APPEARANCE_TAB(module_name, "krasko/StepForSpinControlsOnAppearanceTab");
        static constexpr double STEP_FOR_SPIN_CONTROLS_ON_APPEARANCE_TAB_DEFAULT = 0.5;
        //  Specifies the step (amount) by which spin controls on the Appearance tab of the Properties panel such as the Offsets,
        //  Leading space, Min distance, etc. will change their values.

static const Settings::Key TEXT_STYLES_TO_USE_FONT_HEIGHT(module_name, "krasko/TextStylesToUseFontHeight");
        static constexpr char TEXT_STYLES_TO_USE_FONT_HEIGHT_DEFAULT[] = "HEADER,FOOTER,PAGE_NUMBER";
        //  Comma-separated list of text styles. The height of any text (i.e. text object) with any of those text styles
        //  will use the font's full height. This will fix vertical misalignment issues when different texts are displayed
        //  on the same line/row and some of the texts contain characters with ascenders/descenders and the others do not.
        //  Multiline texts are affected as well. The caveat is that this could slightly increase the spacing below the texts
        //  such as on fingerings above the staff. When a text with style NOT specified in this setting is drawn, its height
        //  will be the actual height of the characters of the text - the so called tight bounding rectange.
        //  The allowed values are the TextStyleType enum values.

static const Settings::Key INVISIBLE_ELEMENTS_COLOR(module_name, "krasko/InvisibleElementsColor");
        static constexpr char INVISIBLE_ELEMENTS_COLOR_DEFAULT[] = "#808080";
        // The color to use for the invisible elements.

static const Settings::Key FIX_BEAMED_NOTES_FINGERING_TOO_CLOSE_TO_STAFF(module_name, "krasko/FixBeamedNotesFingeringTooCloseToStaff");
        static constexpr bool FIX_BEAMED_NOTES_FINGERING_TOO_CLOSE_TO_STAFF_DEFAULT = true;
        //  When true, will fix an issue where fingering on beamed notes does not respect the min distance to staff. The fingering
        //  must be on the side of the beam(s) and the beams should be far enough into the staff (from the edge of the staff).
        //  In this case the fingering is placed too close to the staff.

static const Settings::Key FIX_EXTRA_SPACING_ON_MULTILINE_FINGERING(module_name, "krasko/FixExtraSpacingOnMultilineFingering");
        static constexpr bool FIX_EXTRA_SPACING_ON_MULTILINE_FINGERING_DEFAULT = true;
        //  When true, will fix an issue where multiline fingering has extra spacing above/below. The more lines, the larger
        //  the spacing. The larger the font size, the larger the spacing too.

static const Settings::Key SCROLLBAR_COLOR(module_name, "krasko/ScrollbarColor");
        static constexpr char SCROLLBAR_COLOR_DEFAULT[] = "accentColor";
        //  The color to use for the scrollbars. Some special values like "accentColor" can be used to
        //  state that the color should match any of the theme colors already defined.

static const Settings::Key FLICK_DECELERATION(module_name, "krasko/FlickDeceleration");
        static constexpr int FLICK_DECELERATION_DEFAULT = 12000;
        //  The deceleration to use when scrolling flickable controls (palettes, properties panel, etc.). The higher the value,
        //  the sooner the scrolling will stop when the user stops scrolling. Lower values will make the scrolling continue longer.

// --- HIDDEN SETTINGS END ---


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
    // --- HIDDEN SETTINGS START ---
    settings()->setDefaultValue(AUTO_RESTORE_SESSION_AFTER_CRASH, Val(AUTO_RESTORE_SESSION_AFTER_CRASH_DEFAULT));
    settings()->setDefaultValue(FOCUS_EXPORT_BUTTON_ON_EXPORT_DIALOG, Val(FOCUS_EXPORT_BUTTON_ON_EXPORT_DIALOG_DEFAULT));

    settings()->setDefaultValue(NAV_NEXT_PREV_PANEL_NAVIGATES_TO_NEXT_PREV_CONTROL, Val(NAV_NEXT_PREV_PANEL_NAVIGATES_TO_NEXT_PREV_CONTROL_DEFAULT));

    settings()->setDefaultValue(EDIT_ELEMENT_KEY_CYCLES_THROUGH_GRIPS, Val(EDIT_ELEMENT_KEY_CYCLES_THROUGH_GRIPS_DEFAULT));
    settings()->setDefaultValue(ESCAPE_KEY_WHILE_EDITING_KEEPS_SELECTION, Val(ESCAPE_KEY_WHILE_EDITING_KEEPS_SELECTION_DEFAULT));
    settings()->setDefaultValue(SHOW_SAME_COLOR_CHECKBOX_ON_SELECT_MORE_DIALOG, Val(SHOW_SAME_COLOR_CHECKBOX_ON_SELECT_MORE_DIALOG_DEFAULT));
    settings()->setDefaultValue(ENABLE_ALT_MODIFIER_KEY_FOR_NUDGING, Val(ENABLE_ALT_MODIFIER_KEY_FOR_NUDGING_DEFAULT));
    settings()->setDefaultValue(ENABLE_HIGH_PRECISION_NUDGING, Val(ENABLE_HIGH_PRECISION_NUDGING_DEFAULT));
    settings()->setDefaultValue(STEP_FOR_SPIN_CONTROLS_ON_APPEARANCE_TAB, Val(STEP_FOR_SPIN_CONTROLS_ON_APPEARANCE_TAB_DEFAULT));

    settings()->setDefaultValue(TEXT_STYLES_TO_USE_FONT_HEIGHT, Val(TEXT_STYLES_TO_USE_FONT_HEIGHT_DEFAULT));
    settings()->setDefaultValue(INVISIBLE_ELEMENTS_COLOR, Val(INVISIBLE_ELEMENTS_COLOR_DEFAULT));
    settings()->setDefaultValue(FIX_BEAMED_NOTES_FINGERING_TOO_CLOSE_TO_STAFF, Val(FIX_BEAMED_NOTES_FINGERING_TOO_CLOSE_TO_STAFF_DEFAULT));
    settings()->setDefaultValue(FIX_EXTRA_SPACING_ON_MULTILINE_FINGERING, Val(FIX_EXTRA_SPACING_ON_MULTILINE_FINGERING_DEFAULT));

    settings()->setDefaultValue(SCROLLBAR_COLOR, Val(SCROLLBAR_COLOR_DEFAULT));
    settings()->setDefaultValue(FLICK_DECELERATION, Val(FLICK_DECELERATION_DEFAULT));
    // --- HIDDEN SETTINGS END ---

    settings()->setDefaultValue(HAS_COMPLETED_FIRST_LAUNCH_SETUP, Val(false));

    settings()->setDefaultValue(STARTUP_MODE_TYPE, Val(StartupModeType::StartEmpty));
    settings()->setDefaultValue(STARTUP_SCORE_PATH, Val(projectConfiguration()->myFirstProjectPath().toStdString()));

    fileSystem()->makePath(sessionDataPath());
}

// --- HIDDEN SETTINGS START ---

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
    return settings()->value(AUTO_RESTORE_SESSION_AFTER_CRASH).toBool();
}

bool AppShellConfiguration::focusExportButtonOnExportDialog() const
{
    return settings()->value(FOCUS_EXPORT_BUTTON_ON_EXPORT_DIALOG).toBool();
}

bool AppShellConfiguration::navNextPrevPanelNavigatesToNextPrevControl() const
{
    return settings()->value(NAV_NEXT_PREV_PANEL_NAVIGATES_TO_NEXT_PREV_CONTROL).toBool();
}

bool AppShellConfiguration::editElementKeyCyclesThroughGrips() const
{
    return settings()->value(EDIT_ELEMENT_KEY_CYCLES_THROUGH_GRIPS).toBool();
}

bool AppShellConfiguration::escapeKeyWhileEditingKeepsSelection() const
{
    return settings()->value(ESCAPE_KEY_WHILE_EDITING_KEEPS_SELECTION).toBool();
}

bool AppShellConfiguration::showSameColorCheckBoxOnSelectMoreDialog() const
{
    return settings()->value(SHOW_SAME_COLOR_CHECKBOX_ON_SELECT_MORE_DIALOG).toBool();
}

bool AppShellConfiguration::enableAltModifierKeyForNudging() const
{
    return settings()->value(ENABLE_ALT_MODIFIER_KEY_FOR_NUDGING).toBool();
}

bool AppShellConfiguration::enableHighPrecisionNudging() const
{
    return settings()->value(ENABLE_HIGH_PRECISION_NUDGING).toBool();
}

double AppShellConfiguration::stepForSpinControlsOnAppearanceTab() const
{
    return settings()->value(STEP_FOR_SPIN_CONTROLS_ON_APPEARANCE_TAB).toDouble();
}

bool AppShellConfiguration::textStylesToUseFontHeight(const std::string csvTextStyles) const
{
    std::string value = settings()->value(TEXT_STYLES_TO_USE_FONT_HEIGHT).toString();
    return isStrInCSVString(csvTextStyles, value);
}

std::string AppShellConfiguration::invisibleElementsColor() const
{
    return settings()->value(INVISIBLE_ELEMENTS_COLOR).toString();
}

bool AppShellConfiguration::fixBeamedNotesFingeringTooCloseToStaff() const
{
    return settings()->value(FIX_BEAMED_NOTES_FINGERING_TOO_CLOSE_TO_STAFF).toBool();
}

bool AppShellConfiguration::fixExtraSpacingOnMultilineFingering() const
{
    return settings()->value(FIX_EXTRA_SPACING_ON_MULTILINE_FINGERING).toBool();
}

std::string AppShellConfiguration::scrollbarColor() const
{
    return settings()->value(SCROLLBAR_COLOR).toString();
}

int AppShellConfiguration::flickDeceleration() const
{
    return settings()->value(FLICK_DECELERATION).toInt();
}

// --- HIDDEN SETTINGS END ---


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
