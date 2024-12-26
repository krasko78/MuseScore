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

// --- KRASKO'S SETTINGS START ---
public:
    // General/Miscellaneous
    bool autoRestoreSessionOnStart() const override;
    bool focusExportButtonOnExportDialog() const override;

    // Navigation
    bool tabAndShiftTabNavigateBetweenControls() const override;

    // Editing
    bool editElementKeyCyclesThroughGrips() const override;
    bool escKeyKeepsSelectionWhenEditing() const override;
    bool showSameColorCheckBoxOnSelectMoreDialog() const override;
    bool enableAltModifierKeyForNudging() const override;
    bool enableHighPrecisionNudging() const override;
    double stepForSpinControlsOnAppearanceTab() const override;

    // Engraving / Layout
    bool textStylesToUseFontHeight(const std::string csvTextStyles) const override;
    mu::engraving::Color invisibleElementsColor() const override;
    bool fixBeamedNotesFingeringTooCloseToStaff() const override;
    bool fixExtraSpacingOnMultilineFingering() const override;

    // UI
    mu::engraving::Color scrollbarColor() const override;
    mu::engraving::Color activeGripColor() const override;
    int flickDeceleration() const override;
    int verticalPanelsWidth() const override;
    bool expandShowMore() const override;
    bool hoverDisabledItems() const override;
    bool menuFontFollowsPreferencesFont() const override;
    std::string menuFontSizeRatio() const override;
    bool showScrollbarOnDropDownLists() const override;

    // Notifications
    muse::async::Channel<mu::engraving::Color> invisibleElementsColorChanged() const override;
    muse::async::Channel<mu::engraving::Color> scrollbarColorChanged() const override;
    muse::async::Channel<mu::engraving::Color> activeGripColorChanged() const override;
    muse::async::Channel<int> verticalPanelsWidthChanged() const override;
    muse::async::Channel<bool> menuFontFollowsPreferencesFontChanged() const override;
    muse::async::Channel<std::string> menuFontSizeRatioChanged() const override;

private:
    muse::async::Channel<mu::engraving::Color> m_invisibleElementsColorChanged;
    muse::async::Channel<mu::engraving::Color> m_scrollbarColorChanged;
    muse::async::Channel<mu::engraving::Color> m_activeGripColorChanged;
    muse::async::Channel<int> m_verticalPanelsWidthChanged;
    muse::async::Channel<bool> m_menuFontFollowsPreferencesFontChanged;
    muse::async::Channel<std::string> m_menuFontSizeRatioChanged;

    void initKraskoSettings();
    void createKraskoSettings();
    void updateRenamedKraskoSettings();
    void deleteUnusedKraskoSettings();

    const muse::Settings::Key* findKraskoSettingKey(const std::string& keyName) const;
    bool existsKraskoSetting(const std::string& keyName) const;
    muse::Val kraskoSettingValue(const std::string& keyName) const;
    bool isStrInCSVString(std::string s, std::string csvStr) const;
// --- KRASKO'S SETTINGS END ---

public:
    AppShellConfiguration(const muse::modularity::ContextPtr& iocCtx)
        : muse::Injectable(iocCtx) {}

    ~AppShellConfiguration(); // krasko

    void init();

    bool hasCompletedFirstLaunchSetup() const override;
    void setHasCompletedFirstLaunchSetup(bool has) override;

    StartupModeType startupModeType() const override;
    void setStartupModeType(StartupModeType type) override;

    muse::io::path_t startupScorePath() const override;
    void setStartupScorePath(const muse::io::path_t& scorePath) override;

    muse::io::path_t userDataPath() const override;

    std::string handbookUrl() const override;
    std::string askForHelpUrl() const override;
    std::string museScoreUrl() const override;
    std::string museScoreForumUrl() const override;
    std::string museScoreContributionUrl() const override;
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

    void revertToFactorySettings(bool keepDefaultSettings = false, bool notifyAboutChanges = true) const override;

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
};
}

#endif // MU_APPSHELL_APPSHELLCONFIGURATION_H
