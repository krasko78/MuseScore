/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
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
#include "appshellhiddenconfiguration.h"
#include "settings.h"

using namespace mu;
using namespace mu::appshell;

static const std::string module_name_appshell_2("appshell_2");

// ------------------------------------------------------------------------------------------------------------------------

static const Settings::Key FIX_NON_WORKING_LEADING_SPACE_ON_CLEF_CHANGE(module_name_appshell_2,
    "krasko/FixNonWorkingLeadingSpaceOnClefChange");
        //  When true, will make MuseScore respect the leading space on a clef added to indicate a clef change.
        //  Normal clefs at the start of staves are not affected. Default: true.


// ------------------------------------------------------------------------------------------------------------------------

static const Settings::Key AUTO_RESTORE_SESSION_AFTER_CRASH(module_name_appshell_2,
    "krasko/AutoRestoreSessionAfterCrash");
        //  When true, the previous/last session will be automatically restored after a crash (without asking)
        //  provided Preferences -> General -> "Program start" is set to "Continue last session". Default: true.

static const Settings::Key EXPORT_DIALOG_WILL_FOCUS_THE_EXPORT_BUTTON(module_name_appshell_2,
    "krasko/ExportDialogWillFocusTheExportButton");
        //  When true, the Export dialog will focus the Export button on open.
        //  Default: false.


// ------------------------------------------------------------------------------------------------------------------------

static const Settings::Key NAV_NEXT_PREV_PANEL_NAVIGATES_TO_NEXT_PREV_CONTROL(module_name_appshell_2,
    "krasko/NavNextPrevPanelNavigatesToNextPrevControl");
        //  Determines whether pressing the shortcut keys for "nav-next-panel" and "nav-prev-panel" (TAB and SHIFT+TAB
        //  by default) will navigate to the next/prev control (value: true) or next/prev panel (value: false). Default: true.

static const Settings::Key DISABLE_ARROW_KEYS_FOR_NAVIGATION(module_name_appshell_2,
    "krasko/DisableArrowKeysForNavigation");
        //  When true, will disable navigation between controls via the arrow keys (or the nav-up, nav-down,
        //  nav-left and nav-right shortcut keys). Default: false.

static const Settings::Key ENABLE_ALT_MODIFIER_KEY(module_name_appshell_2,
    "krasko/EnableAltModifierKey");
        //  When true, the ALT key will be enabled to participate in shortcuts such as nudging a grip/handle
        //  with ALT+arrow keys. Default: false.


// ------------------------------------------------------------------------------------------------------------------------

static const Settings::Key EDIT_ELEMENT_KEY_WILL_CYCLE_THROUGH_GRIPS(module_name_appshell_2,
    "krasko/EditElementKeyWillCycleThroughGrips");
        //  When true, pressing the "Edit Element" shortcut key (F2 by default) will cycle through an element's grips
        //  just like the TAB key if the element already has the grips displayed. This lets you press the "Edit Element" key
        //  once to diplay the grips/handles (if the element is selected) and then continue pressing the same key
        //  (instead of TAB) to activate the desired grip/handle to adjust. Default: true.

static const Settings::Key ENABLE_HIGH_PRECISION_NUDGING(module_name_appshell_2,
    "krasko/EnableHighPrecisionNudging");
        //  When true, nudging things such as grips/handles with the arrow keys will nudge by a smaller amount thus allowing
        //  for better control. If the ALT modifier is enabled, using ALT+arrow keys will nudge even less. Default: false.

static const Settings::Key STEP_FOR_SPIN_CONTROLS_ON_APPEARANCE_TAB(module_name_appshell_2,
    "krasko/StepForSpinControlsOnAppearanceTab");
        //  Specifies the step (amount) by which spin controls on the Appearance tab of the Properties panel such as the Offsets,
        //  Leading space, Min distance, etc. will change their values. Default: 0.5.

static const Settings::Key ENABLE_SAME_COLOR_SELECTION(module_name_appshell_2,
    "krasko/EnableSameColorSelection");
        //  When true, will display a "Same color" checkbox on the Select More... dialog so that the selection
        //  can be limited to the elements having the same color. Default: true.


// ------------------------------------------------------------------------------------------------------------------------

static const Settings::Key TEXT_OBJECTS_TO_USE_FONT_HEIGHT(module_name_appshell_2,
    "krasko/TextObjectsToUseFontHeight");
        //  Is a comma-separated list of text styles. The height of any text (i.e. text object) with any of those text styles
        //  will use the font's full height. This will fix vertical misalignment issues when different texts are displayed
        //  on the same line/row and some of the texts contain characters with ascenders/descenders and the others do not.
        //  Multiline texts are affected as well. The caveat is that this could slightly increase the spacing below the texts
        //  such as on fingerings above the staff. When a text with style NOT specified in this setting, the height will be
        //  the actual height of the characters of the text. The allowed values are the TextStyleType enum values.
        //  Default: HEADER,FOOTER.

static const Settings::Key BEAMED_NOTES_FINGERING_WILL_RESPECT_MIN_DISTANCE_TO_STAFF(module_name_appshell_2,
    "krasko/BeamedNotesFingeringWillRespectMinDistanceToStaff");
        //  When true, will fix an issue where fingering on beamed notes does not respect the min distance to staff. The fingering
        //  should be on the side of the beam(s) and the beams should be far enough into the staff (far enough from the edge
        //  of the staff). In this case the fingering is placed too close to the staff. Default: true.

static const Settings::Key FIX_EXTRA_SPACING_ON_MULTILINE_FINGERING(module_name_appshell_2,
    "krasko/FixExtraSpacingOnMultilineFingering");
        //  When true, will fix an issue where multiline fingering has extra spacing above/below. The more lines, the larger
        //  the spacing. The larger the font size, the larger the spacing. Default: true.



// ------------------------------------------------------------------------------------------------------------------------

void AppShellHiddenConfiguration::init()
{
    settings()->setDefaultValue(FIX_NON_WORKING_LEADING_SPACE_ON_CLEF_CHANGE, Val(true));

    settings()->setDefaultValue(AUTO_RESTORE_SESSION_AFTER_CRASH, Val(true));
    settings()->setDefaultValue(EXPORT_DIALOG_WILL_FOCUS_THE_EXPORT_BUTTON, Val(false));

    settings()->setDefaultValue(NAV_NEXT_PREV_PANEL_NAVIGATES_TO_NEXT_PREV_CONTROL, Val(true));
    settings()->setDefaultValue(DISABLE_ARROW_KEYS_FOR_NAVIGATION, Val(false));
    settings()->setDefaultValue(ENABLE_ALT_MODIFIER_KEY, Val(false));

    settings()->setDefaultValue(EDIT_ELEMENT_KEY_WILL_CYCLE_THROUGH_GRIPS, Val(true));
    settings()->setDefaultValue(ENABLE_HIGH_PRECISION_NUDGING, Val(false));
    settings()->setDefaultValue(STEP_FOR_SPIN_CONTROLS_ON_APPEARANCE_TAB, Val(0.5));
    settings()->setDefaultValue(ENABLE_SAME_COLOR_SELECTION, Val(true));

    settings()->setDefaultValue(TEXT_OBJECTS_TO_USE_FONT_HEIGHT, Val("HEADER,FOOTER"));
    settings()->setDefaultValue(BEAMED_NOTES_FINGERING_WILL_RESPECT_MIN_DISTANCE_TO_STAFF, Val(true));
    settings()->setDefaultValue(FIX_EXTRA_SPACING_ON_MULTILINE_FINGERING, Val(true));
}

bool AppShellHiddenConfiguration::getBool(const Settings::Key& key) const
{
    return settings()->value(key).toBool();
}

void AppShellHiddenConfiguration::setBool(const Settings::Key& key, const bool value)
{
    settings()->setSharedValue(key, Val(value));
}

double AppShellHiddenConfiguration::getDouble(const Settings::Key& key) const
{
    return settings()->value(key).toDouble();
}

void AppShellHiddenConfiguration::setDouble(const Settings::Key& key, const double value)
{
    settings()->setSharedValue(key, Val(value));
}

std::string AppShellHiddenConfiguration::getString(const Settings::Key& key) const
{
    return settings()->value(key).toString();
}

void AppShellHiddenConfiguration::setString(const Settings::Key& key, const std::string value)
{
    settings()->setSharedValue(key, Val(value));
}


// ------------------------------------------------------------------------------------------------------------------------

bool AppShellHiddenConfiguration::fixNonWorkingLeadingSpaceOnClefChange() const { return getBool(FIX_NON_WORKING_LEADING_SPACE_ON_CLEF_CHANGE); }
void AppShellHiddenConfiguration::setFixNonWorkingLeadingSpaceOnClefChange(bool value) { setBool(FIX_NON_WORKING_LEADING_SPACE_ON_CLEF_CHANGE, value); }


// ------------------------------------------------------------------------------------------------------------------------

bool AppShellHiddenConfiguration::autoRestoreSessionAfterCrash() const { return getBool(AUTO_RESTORE_SESSION_AFTER_CRASH); }
void AppShellHiddenConfiguration::setAutoRestoreSessionAfterCrash(bool value) { setBool(AUTO_RESTORE_SESSION_AFTER_CRASH, value); }

bool AppShellHiddenConfiguration::exportDialogWillFocusTheExportButton() const { return getBool(EXPORT_DIALOG_WILL_FOCUS_THE_EXPORT_BUTTON); }
void AppShellHiddenConfiguration::setExportDialogWillFocusTheExportButton(bool value) { setBool(EXPORT_DIALOG_WILL_FOCUS_THE_EXPORT_BUTTON, value); }


// ------------------------------------------------------------------------------------------------------------------------

bool AppShellHiddenConfiguration::navNextPrevPanelNavigatesToNextPrevControl() const { return getBool(NAV_NEXT_PREV_PANEL_NAVIGATES_TO_NEXT_PREV_CONTROL); }
void AppShellHiddenConfiguration::setNavNextPrevPanelNavigatesToNextPrevControl(bool value) { setBool(NAV_NEXT_PREV_PANEL_NAVIGATES_TO_NEXT_PREV_CONTROL, value); }

bool AppShellHiddenConfiguration::disableArrowKeysForNavigation() const { return getBool(DISABLE_ARROW_KEYS_FOR_NAVIGATION); }
void AppShellHiddenConfiguration::setDisableArrowKeysForNavigation(bool value) { setBool(DISABLE_ARROW_KEYS_FOR_NAVIGATION, value); }

bool AppShellHiddenConfiguration::enableAltModifierKey() const { return getBool(ENABLE_ALT_MODIFIER_KEY); }
void AppShellHiddenConfiguration::setEnableAltModifierKey(bool value) { setBool(ENABLE_ALT_MODIFIER_KEY, value); }


// ------------------------------------------------------------------------------------------------------------------------

bool AppShellHiddenConfiguration::editElementKeyWillCycleThroughGrips() const { return getBool(EDIT_ELEMENT_KEY_WILL_CYCLE_THROUGH_GRIPS); }
void AppShellHiddenConfiguration::setEditElementKeyWillCycleThroughGrips(bool value) { setBool(EDIT_ELEMENT_KEY_WILL_CYCLE_THROUGH_GRIPS, value); }

bool AppShellHiddenConfiguration::enableHighPrecisionNudging() const { return getBool(ENABLE_HIGH_PRECISION_NUDGING); }
void AppShellHiddenConfiguration::setEnableHighPrecisionNudging(bool value) { setBool(ENABLE_HIGH_PRECISION_NUDGING, value); }

double AppShellHiddenConfiguration::stepForSpinControlsOnAppearanceTab() const { return getDouble(STEP_FOR_SPIN_CONTROLS_ON_APPEARANCE_TAB); }
void AppShellHiddenConfiguration::setStepForSpinControlsOnAppearanceTab(double value) { setDouble(STEP_FOR_SPIN_CONTROLS_ON_APPEARANCE_TAB, value); }

bool AppShellHiddenConfiguration::enableSameColorSelection() const { return getBool(ENABLE_SAME_COLOR_SELECTION); }
void AppShellHiddenConfiguration::setEnableSameColorSelection(bool value) { setBool(ENABLE_SAME_COLOR_SELECTION, value); }


// ------------------------------------------------------------------------------------------------------------------------

bool AppShellHiddenConfiguration::textObjectShouldUseFontHeight(std::string textStyle) const
{
    std::size_t pos;

    std::string value = getString(TEXT_OBJECTS_TO_USE_FONT_HEIGHT);
    if (value == textStyle)
        return true;

    if (value.find(textStyle + ",") == 0)
        return true;

    if (value.find(textStyle + ", ") == 0)
        return true;

    if (((pos = value.find("," + textStyle)) != std::string::npos) && (pos == (value.length() - textStyle.length() - 1)))
        return true;

    if (((pos = value.find(", " + textStyle)) != std::string::npos) && (pos == (value.length() - textStyle.length() - 2)))
        return true;

    if (value.find("," + textStyle + ",") != std::string::npos)
        return true;

    if (value.find(", " + textStyle + ",") != std::string::npos)
        return true;

    return false;
}
void AppShellHiddenConfiguration::setTextObjectsToUseFontHeight(std::string value) { setString(TEXT_OBJECTS_TO_USE_FONT_HEIGHT, value); }

bool AppShellHiddenConfiguration::beamedNotesFingeringWillRespectMinDistanceToStaff() const { return getBool(BEAMED_NOTES_FINGERING_WILL_RESPECT_MIN_DISTANCE_TO_STAFF); }
void AppShellHiddenConfiguration::setBeamedNotesFingeringWillRespectMinDistanceToStaff(bool value) { setBool(BEAMED_NOTES_FINGERING_WILL_RESPECT_MIN_DISTANCE_TO_STAFF, value); }

bool AppShellHiddenConfiguration::fixExtraSpacingOnMultilineFingering() const { return getBool(FIX_EXTRA_SPACING_ON_MULTILINE_FINGERING); }
void AppShellHiddenConfiguration::setFixExtraSpacingOnMultilineFingering(bool value) { setBool(FIX_EXTRA_SPACING_ON_MULTILINE_FINGERING, value); }

