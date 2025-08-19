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
#ifndef MU_APPSHELL_APPSHELLCONFIGURATION_H
#define MU_APPSHELL_APPSHELLCONFIGURATION_H

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "global/iglobalconfiguration.h"
#include "global/iapplication.h"
#include "global/io/ifilesystem.h"
#include "multiinstances/imultiinstancesprovider.h"
#include "ui/iuiconfiguration.h"
#include "project/iprojectconfiguration.h"
#include "notation/inotationconfiguration.h"
#include "playback/iplaybackconfiguration.h"
#include "languages/ilanguagesconfiguration.h"
#include "settings.h" // krasko

#include "iappshellconfiguration.h"

namespace mu::appshell {
class AppShellConfiguration : public IAppShellConfiguration, public muse::Injectable, public muse::async::Asyncable
{
    muse::Inject<muse::IGlobalConfiguration> globalConfiguration = { this };
    muse::Inject<muse::IApplication> application = { this };
    muse::Inject<muse::io::IFileSystem> fileSystem = { this };
    muse::Inject<muse::mi::IMultiInstancesProvider> multiInstancesProvider = { this };
    muse::Inject<muse::ui::IUiConfiguration> uiConfiguration = { this };
    muse::Inject<project::IProjectConfiguration> projectConfiguration = { this };
    muse::Inject<notation::INotationConfiguration> notationConfiguration = { this };
    muse::Inject<playback::IPlaybackConfiguration> playbackConfiguration = { this };
    muse::Inject<muse::languages::ILanguagesConfiguration> languagesConfiguration = { this };

// krasko start: KRASKO'S SETTINGS

public:
    bool autoRestoreSessionOnStart() const override;

    bool focusExportButtonOnExportDialog() const override;
    muse::async::Channel<bool> focusExportButtonOnExportDialogChanged() const override;

    bool tabAndShiftTabNavigateBetweenControls() const override;

    bool changeActiveGripWithEditElementKey() const override;

    bool escKeyPreservesSelectionWhenEditing() const override;

    bool expandShowMoreSectionsInPropertiesPanel() const override;
    muse::async::Channel<bool> expandShowMoreSectionsInPropertiesPanelChanged() const override;

    bool doNotHighlightDisabledItemsOnHover() const override;
    muse::async::Channel<bool> doNotHighlightDisabledItemsOnHoverChanged() const override;

    bool showScrollbarOnScrollableDropDownLists() const override;
    muse::async::Channel<bool> showScrollbarOnScrollableDropDownListsChanged() const override;

    mu::engraving::Color activeGripColor() const override;
    muse::async::Channel<mu::engraving::Color> activeGripColorChanged() const override;

    mu::engraving::Color scrollbarColor() const override;
    muse::async::Channel<mu::engraving::Color> scrollbarColorChanged() const override;

    bool mainMenuFontFollowsPreferencesFont() const override;
    muse::async::Channel<bool> mainMenuFontFollowsPreferencesFontChanged() const override;

    double mainMenuFontSizeMultiplier() const override;
    muse::async::Channel<double> mainMenuFontSizeMultiplierChanged() const override;

    bool enableHighPrecisionNudging() const override;

    bool showSameColorCheckBoxOnSelectMoreDialog() const override;

    int verticalPanelsWidth() const override;
    muse::async::Channel<int> verticalPanelsWidthChanged() const override;

    int scrollDecelerationOfListsAndPanels() const override;
    muse::async::Channel<int> scrollDecelerationOfListsAndPanelsChanged() const override;

    double stepForSpinControlsOnAppearanceTab() const override;
    muse::async::Channel<double> stepForSpinControlsOnAppearanceTabChanged() const override;

    std::string textStylesToUseFullFontHeight() const override;
    muse::async::Channel<std::string> textStylesToUseFullFontHeightChanged() const override;

    bool fixFingeringTooCloseToStaffOnBeamedNotes() const override;
    muse::async::Channel<bool> fixFingeringTooCloseToStaffOnBeamedNotesChanged() const override;

    bool removeExtraSpacingOnMultilineFingering() const override;
    muse::async::Channel<bool> removeExtraSpacingOnMultilineFingeringChanged() const override;

public:
    bool isValueInCsvList(const std::string& s, const std::string& csvList) const override;

private:
    muse::async::Channel<bool> m_focusExportButtonOnExportDialogChanged;
    muse::async::Channel<bool> m_expandShowMoreSectionsInPropertiesPanelChanged;
    muse::async::Channel<bool> m_doNotHighlightDisabledItemsOnHoverChanged;
    muse::async::Channel<bool> m_showScrollbarOnScrollableDropDownListsChanged;
    muse::async::Channel<mu::engraving::Color> m_activeGripColorChanged;
    muse::async::Channel<mu::engraving::Color> m_scrollbarColorChanged;
    muse::async::Channel<bool> m_mainMenuFontFollowsPreferencesFontChanged;
    muse::async::Channel<double> m_mainMenuFontSizeMultiplierChanged;
    muse::async::Channel<int> m_verticalPanelsWidthChanged;
    muse::async::Channel<int> m_scrollDecelerationOfListsAndPanelsChanged;
    muse::async::Channel<double> m_stepForSpinControlsOnAppearanceTabChanged;
    muse::async::Channel<std::string> m_textStylesToUseFullFontHeightChanged;
    muse::async::Channel<bool> m_fixFingeringTooCloseToStaffOnBeamedNotesChanged;
    muse::async::Channel<bool> m_removeExtraSpacingOnMultilineFingeringChanged;

    void initKraskoSettings();
    void createKraskoSettings();
    void updateRenamedKraskoSettings();
    void deleteUnusedKraskoSettings();
    void notifyAboutChangedKraskoSettings();

    const muse::Settings::Key* findKraskoSettingKey(const std::string& keyName) const;
    bool existsKraskoSetting(const std::string& keyName) const;
    muse::Val kraskoSettingValue(const std::string& keyName) const;

// krasko end: KRASKO'S SETTINGS

public:
    AppShellConfiguration(const muse::modularity::ContextPtr& iocCtx)
        : muse::Injectable(iocCtx) {}

    ~AppShellConfiguration(); // krasko

    void init();

    bool hasCompletedFirstLaunchSetup() const override;
    void setHasCompletedFirstLaunchSetup(bool has) override;

    bool welcomeDialogShowOnStartup() const override;
    void setWelcomeDialogShowOnStartup(bool show) override;
    muse::async::Notification welcomeDialogShowOnStartupChanged() const override;

    std::string welcomeDialogLastShownVersion() const override;
    void setWelcomeDialogLastShownVersion(const std::string& version) override;

    int welcomeDialogLastShownIndex() const override;
    void setWelcomeDialogLastShownIndex(int index) override;

    StartupModeType startupModeType() const override;
    void setStartupModeType(StartupModeType type) override;
    muse::async::Notification startupModeTypeChanged() const override;

    muse::io::path_t startupScorePath() const override;
    void setStartupScorePath(const muse::io::path_t& scorePath) override;
    muse::async::Notification startupScorePathChanged() const override;

    muse::io::path_t userDataPath() const override;

    std::string handbookUrl() const override;
    std::string askForHelpUrl() const override;
    std::string museScoreUrl() const override;
    std::string museScoreForumUrl() const override;
    std::string museScoreContributionUrl() const override;
    std::string museHubFreeMuseSoundsUrl() const override;
    std::string musicXMLLicenseUrl() const override;
    std::string musicXMLLicenseDeedUrl() const override;

    std::string museScoreVersion() const override;
    std::string museScoreRevision() const override;

    bool isNotationNavigatorVisible() const override;
    void setIsNotationNavigatorVisible(bool visible) const override;
    muse::async::Notification isNotationNavigatorVisibleChanged() const override;

    bool needShowSplashScreen() const override;
    void setNeedShowSplashScreen(bool show) override;

    const QString& preferencesDialogLastOpenedPageId() const override;
    void setPreferencesDialogLastOpenedPageId(const QString& lastOpenedPageId) override;

    void startEditSettings() override;
    void applySettings() override;
    void rollbackSettings() override;

    void revertToFactorySettings(bool keepDefaultSettings = false, bool notifyAboutChanges = true,
                                 bool notifyOtherInstances = true) const override;

    muse::io::paths_t sessionProjectsPaths() const override;
    muse::Ret setSessionProjectsPaths(const muse::io::paths_t& paths) override;

private:
    muse::SettingsCreator* m_settingsCreator = nullptr;

    std::string utmParameters(const std::string& utmMedium) const;

    std::string currentLanguageCode() const;

    muse::io::path_t sessionDataPath() const;
    muse::io::path_t sessionFilePath() const;

    muse::RetVal<muse::ByteArray> readSessionState() const;
    muse::Ret writeSessionState(const QByteArray& data);

    muse::io::paths_t parseSessionProjectsPaths(const QByteArray& json) const;

    QString m_preferencesDialogCurrentPageId;

    muse::async::Notification m_welcomeDialogShowOnStartupChanged;

    muse::async::Notification m_startupModeTypeChanged;
    muse::async::Notification m_startupScorePathChanged;
};
}

#endif // MU_APPSHELL_APPSHELLCONFIGURATION_H
