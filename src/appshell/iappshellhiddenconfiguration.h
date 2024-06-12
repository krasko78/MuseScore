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
#ifndef MU_APPSHELL_IAPPSHELLHIDDENCONFIGURATION_H
#define MU_APPSHELL_IAPPSHELLHIDDENCONFIGURATION_H

#include "modularity/imoduleinterface.h"

namespace mu::appshell {
class IAppShellHiddenConfiguration : MODULE_EXPORT_INTERFACE
{
    INTERFACE_ID(IAppshellHiddenConfiguration)

public:
    // ------------------------------------------------------------------------------------------------------------------------

    virtual bool fixNonWorkingLeadingSpaceOnClefChange() const = 0;
    virtual void setFixNonWorkingLeadingSpaceOnClefChange(bool value) = 0;


    // ------------------------------------------------------------------------------------------------------------------------

    virtual bool autoRestoreSessionAfterCrash() const = 0;
    virtual void setAutoRestoreSessionAfterCrash(bool value) = 0;

    virtual bool exportDialogWillFocusTheExportButton() const = 0;
    virtual void setExportDialogWillFocusTheExportButton(bool value) = 0;


    // ------------------------------------------------------------------------------------------------------------------------

    virtual bool navNextPrevPanelNavigatesToNextPrevControl() const = 0;
    virtual void setNavNextPrevPanelNavigatesToNextPrevControl(bool value) = 0;

    virtual bool disableArrowKeysForNavigation() const = 0;
    virtual void setDisableArrowKeysForNavigation(bool value) = 0;

    virtual bool enableAltModifierKey() const = 0;
    virtual void setEnableAltModifierKey(bool value) = 0;


    // ------------------------------------------------------------------------------------------------------------------------

    virtual bool editElementKeyWillCycleThroughGrips() const = 0;
    virtual void setEditElementKeyWillCycleThroughGrips(bool value) = 0;

    virtual bool enableHighPrecisionNudging() const = 0;
    virtual void setEnableHighPrecisionNudging(bool value) = 0;

    virtual double stepForSpinControlsOnAppearanceTab() const = 0;
    virtual void setStepForSpinControlsOnAppearanceTab(double value) = 0;

    virtual bool enableSameColorSelection() const = 0;
    virtual void setEnableSameColorSelection(bool value) = 0;


    // ------------------------------------------------------------------------------------------------------------------------

    virtual bool textObjectShouldUseFontHeight(std::string textStyle) const = 0;
    virtual void setTextObjectsToUseFontHeight(std::string value) = 0;

    virtual bool beamedNotesFingeringWillRespectMinDistanceToStaff() const = 0;
    virtual void setBeamedNotesFingeringWillRespectMinDistanceToStaff(bool value) = 0;

    virtual bool fixExtraSpacingOnMultilineFingering() const = 0;
    virtual void setFixExtraSpacingOnMultilineFingering(bool value) = 0;
};
}

#endif // MU_APPSHELL_IAPPSHELLHIDDENCONFIGURATION_H
