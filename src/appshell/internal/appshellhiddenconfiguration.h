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
#ifndef MU_APPSHELL_APPSHELLHIDDENCONFIGURATION_H
#define MU_APPSHELL_APPSHELLHIDDENCONFIGURATION_H

#include <settings.h>
#include "iappshellhiddenconfiguration.h"

using namespace muse;

namespace mu::appshell {
class AppShellHiddenConfiguration : public IAppShellHiddenConfiguration, public muse::Injectable
{
public:
    AppShellHiddenConfiguration(const muse::modularity::ContextPtr& iocCtx)
        : muse::Injectable(iocCtx) {}

    void init();


    // ------------------------------------------------------------------------------------------------------------------------

    bool fixNonWorkingLeadingSpaceOnClefChange() const override;
    void setFixNonWorkingLeadingSpaceOnClefChange(bool value) override;


    // ------------------------------------------------------------------------------------------------------------------------

    bool autoRestoreSessionAfterCrash() const override;
    void setAutoRestoreSessionAfterCrash(bool value) override;

    bool exportDialogWillFocusTheExportButton() const override;
    void setExportDialogWillFocusTheExportButton(bool value) override;


    // ------------------------------------------------------------------------------------------------------------------------

    bool navNextPrevPanelNavigatesToNextPrevControl() const override;
    void setNavNextPrevPanelNavigatesToNextPrevControl(bool value) override;

    bool disableArrowKeysForNavigation() const override;
    void setDisableArrowKeysForNavigation(bool value) override;

    bool enableAltModifierKey() const override;
    void setEnableAltModifierKey(bool value) override;


    // ------------------------------------------------------------------------------------------------------------------------

    bool editElementKeyWillCycleThroughGrips() const override;
    void setEditElementKeyWillCycleThroughGrips(bool value) override;

    bool enableHighPrecisionNudging() const override;
    void setEnableHighPrecisionNudging(bool value) override;

    double stepForSpinControlsOnAppearanceTab() const override;
    void setStepForSpinControlsOnAppearanceTab(double value) override;

    bool enableSameColorSelection() const override;
    void setEnableSameColorSelection(bool value) override;


    // ------------------------------------------------------------------------------------------------------------------------

    bool textObjectShouldUseFontHeight(std::string textStyle) const override;
    void setTextObjectsToUseFontHeight(std::string value) override;

    bool beamedNotesFingeringWillRespectMinDistanceToStaff() const override;
    void setBeamedNotesFingeringWillRespectMinDistanceToStaff(bool value) override;

    bool fixExtraSpacingOnMultilineFingering() const override;
    void setFixExtraSpacingOnMultilineFingering(bool value) override;

private:
    bool getBool(const Settings::Key& key) const;
    void setBool(const Settings::Key& key, const bool value);

    double getDouble(const Settings::Key& key) const;
    void setDouble(const Settings::Key& key, const double value);

    std::string getString(const Settings::Key& key) const;
    void setString(const Settings::Key& key, const std::string value);
};
}

#endif // MU_APPSHELL_APPSHELLHIDDENCONFIGURATION_H
