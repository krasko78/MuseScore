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
#ifndef MU_APPSHELL_IAPPSHELLCONFIGURATION_H
#define MU_APPSHELL_IAPPSHELLCONFIGURATION_H

#include "modularity/imoduleinterface.h"
#include "types/ret.h"

#include "io/path.h"
#include "appshelltypes.h"
#include "async/channel.h" // krasko
#include "async/notification.h"
#include "engraving/types/types.h" // krasko

namespace mu::appshell {
class IAppShellConfiguration : MODULE_EXPORT_INTERFACE
{
    INTERFACE_ID(IAppshellConfiguration)

// krasko start: KRASKO'S SETTINGS

public:
    virtual bool autoRestoreSessionOnStart() const = 0;

    virtual bool focusExportButtonOnExportDialog() const = 0;
    virtual muse::async::Channel<bool> focusExportButtonOnExportDialogChanged() const = 0;

    virtual bool tabAndShiftTabNavigateBetweenControls() const = 0;

    virtual bool changeActiveGripWithEditElementKey() const = 0;

    virtual bool escKeyPreservesSelectionWhenEditing() const = 0;

    virtual bool expandShowMoreSectionsInPropertiesPanel() const = 0;
    virtual muse::async::Channel<bool> expandShowMoreSectionsInPropertiesPanelChanged() const = 0;

    virtual bool doNotHighlightDisabledItemsOnHover() const = 0;
    virtual muse::async::Channel<bool> doNotHighlightDisabledItemsOnHoverChanged() const = 0;

    virtual bool showScrollbarOnScrollableDropDownLists() const = 0;
    virtual muse::async::Channel<bool> showScrollbarOnScrollableDropDownListsChanged() const = 0;

    virtual mu::engraving::Color activeGripColor() const = 0;
    virtual muse::async::Channel<mu::engraving::Color> activeGripColorChanged() const = 0;

    virtual mu::engraving::Color scrollbarColor() const = 0;
    virtual muse::async::Channel<mu::engraving::Color> scrollbarColorChanged() const = 0;

    virtual bool mainMenuFontFollowsPreferencesFont() const = 0;
    virtual muse::async::Channel<bool> mainMenuFontFollowsPreferencesFontChanged() const = 0;

    virtual double mainMenuFontSizeMultiplier() const = 0;
    virtual muse::async::Channel<double> mainMenuFontSizeMultiplierChanged() const = 0;

    virtual bool enableHighPrecisionNudging() const = 0;

    virtual bool showSameColorCheckBoxOnSelectMoreDialog() const = 0;

    virtual int verticalPanelsWidth() const = 0;
    virtual muse::async::Channel<int> verticalPanelsWidthChanged() const = 0;

    virtual int scrollDecelerationOfListsAndPanels() const = 0;
    virtual muse::async::Channel<int> scrollDecelerationOfListsAndPanelsChanged() const = 0;

    virtual double stepForSpinControlsOnAppearanceTab() const = 0;
    virtual muse::async::Channel<double> stepForSpinControlsOnAppearanceTabChanged() const = 0;

    virtual std::string textStylesToUseFullFontHeight() const = 0;
    virtual muse::async::Channel<std::string> textStylesToUseFullFontHeightChanged() const = 0;

    virtual bool fixFingeringTooCloseToStaffOnBeamedNotes() const = 0;
    virtual muse::async::Channel<bool> fixFingeringTooCloseToStaffOnBeamedNotesChanged() const = 0;

    virtual bool removeExtraSpacingOnMultilineFingering() const = 0;
    virtual muse::async::Channel<bool> removeExtraSpacingOnMultilineFingeringChanged() const = 0;

public:
    virtual bool isValueInCsvList(const std::string& s, const std::string& csvList) const = 0;

// krasko end: KRASKO'S SETTINGS

public:
    virtual ~IAppShellConfiguration() = default;

    virtual bool hasCompletedFirstLaunchSetup() const = 0;
    virtual void setHasCompletedFirstLaunchSetup(bool has) = 0;

    virtual bool welcomeDialogShowOnStartup() const = 0;
    virtual void setWelcomeDialogShowOnStartup(bool show) = 0;
    virtual muse::async::Notification welcomeDialogShowOnStartupChanged() const = 0;

    virtual std::string welcomeDialogLastShownVersion() const = 0;
    virtual void setWelcomeDialogLastShownVersion(const std::string& version) = 0;

    virtual int welcomeDialogLastShownIndex() const = 0;
    virtual void setWelcomeDialogLastShownIndex(int index) = 0;

    virtual StartupModeType startupModeType() const = 0;
    virtual void setStartupModeType(StartupModeType type) = 0;
    virtual muse::async::Notification startupModeTypeChanged() const = 0;

    virtual muse::io::path_t startupScorePath() const = 0;
    virtual void setStartupScorePath(const muse::io::path_t& scorePath) = 0;
    virtual muse::async::Notification startupScorePathChanged() const = 0;

    virtual muse::io::path_t userDataPath() const = 0;

    virtual std::string handbookUrl() const = 0;
    virtual std::string askForHelpUrl() const = 0;
    virtual std::string museScoreUrl() const = 0;
    virtual std::string museScoreForumUrl() const = 0;
    virtual std::string museScoreContributionUrl() const = 0;
    virtual std::string museHubFreeMuseSoundsUrl() const = 0;
    virtual std::string musicXMLLicenseUrl() const = 0;
    virtual std::string musicXMLLicenseDeedUrl() const = 0;

    virtual std::string museScoreVersion() const = 0;
    virtual std::string museScoreRevision() const = 0;

    virtual bool isNotationNavigatorVisible() const = 0;
    virtual void setIsNotationNavigatorVisible(bool visible) const = 0;
    virtual muse::async::Notification isNotationNavigatorVisibleChanged() const = 0;

    virtual bool needShowSplashScreen() const = 0;
    virtual void setNeedShowSplashScreen(bool show) = 0;

    virtual const QString& preferencesDialogLastOpenedPageId() const = 0;
    virtual void setPreferencesDialogLastOpenedPageId(const QString& lastOpenedPageId) = 0;

    virtual void startEditSettings() = 0;
    virtual void applySettings() = 0;
    virtual void rollbackSettings() = 0;

    virtual void revertToFactorySettings(bool keepDefaultSettings = false, bool notifyAboutChanges = true,
                                         bool notifyOtherInstances = true) const = 0;

    virtual muse::io::paths_t sessionProjectsPaths() const = 0;
    virtual muse::Ret setSessionProjectsPaths(const muse::io::paths_t& paths) = 0;
};
}

#endif // MU_APPSHELL_IAPPSHELLCONFIGURATION_H
