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
#include "appshellconfiguration.h"

#include <QJsonArray>
#include <QJsonDocument>

#include "settings.h"

#include "multiinstances/resourcelockguard.h"

#include "log.h"

using namespace mu::appshell;
using namespace mu::notation;
using namespace mu::framework;

static const std::string module_name("appshell");

// KRASKO {START} Settings
// "NavNextPrevPanelGoesToNextPrevControl" - determines whether pressing the shortcuts keys for "nav-next-panel" and "nav-prev-panel"
// (TAB and SHIFT+TAB) by default) will go to the next/prev control (value: true) or next/prev panel (value: false - the default).
static const Settings::Key NAV_NEXT_PREV_PANEL_GOES_TO_NEXT_PREV_CONTROL(module_name, "krasko/NavNextPrevPanelGoesToNextPrevControl");
// "UseArrowKeysForNavigation" - when false, pressing the arrow keys (or nav-up, nav-down, nav-left, nav-right shortcut keys)
// will navigate between the controls in the current panel (value: true - default) or not (value: false).
static const Settings::Key USE_ARROW_KEYS_FOR_NAVIGATION(module_name, "krasko/UseArrowKeysForNavigation");
// KRASKO {END}

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

static const mu::io::path_t SESSION_FILE("/session.json");
static const std::string SESSION_RESOURCE_NAME("SESSION");

void AppShellConfiguration::init()
{
    // KRASKO {START}
    // Set all default values to Val() so that these settings are always present
    // in the settings file even when set to their default values.
    settings()->setDefaultValue(NAV_NEXT_PREV_PANEL_GOES_TO_NEXT_PREV_CONTROL, Val());
    settings()->setDefaultValue(USE_ARROW_KEYS_FOR_NAVIGATION, Val());
    // KRASKO {END}

    settings()->setDefaultValue(HAS_COMPLETED_FIRST_LAUNCH_SETUP, Val(false));

    settings()->setDefaultValue(STARTUP_MODE_TYPE, Val(StartupModeType::StartEmpty));
    settings()->setDefaultValue(STARTUP_SCORE_PATH, Val(projectConfiguration()->myFirstProjectPath().toStdString()));

    fileSystem()->makePath(sessionDataPath());
}

bool AppShellConfiguration::hasCompletedFirstLaunchSetup() const
{
    return settings()->value(HAS_COMPLETED_FIRST_LAUNCH_SETUP).toBool();
}

void AppShellConfiguration::setHasCompletedFirstLaunchSetup(bool has)
{
    settings()->setSharedValue(HAS_COMPLETED_FIRST_LAUNCH_SETUP, Val(has));
}

// KRASKO {START} Settings
bool AppShellConfiguration::navNextPrevPanelGoesToNextPrevControl() const
{
    return settings()->value(NAV_NEXT_PREV_PANEL_GOES_TO_NEXT_PREV_CONTROL).toBool();
}

void AppShellConfiguration::setNavNextPrevPanelGoesToNextPrevControl(bool value)
{
    settings()->setSharedValue(NAV_NEXT_PREV_PANEL_GOES_TO_NEXT_PREV_CONTROL, Val(value));
}

bool AppShellConfiguration::useArrowKeysForNavigation() const
{
    return settings()->value(USE_ARROW_KEYS_FOR_NAVIGATION).toBool();
}

void AppShellConfiguration::setUseArrowKeysForNavigation(bool value)
{
    settings()->setSharedValue(USE_ARROW_KEYS_FOR_NAVIGATION, Val(value));
}
// KRASKO {END}

StartupModeType AppShellConfiguration::startupModeType() const
{
    return settings()->value(STARTUP_MODE_TYPE).toEnum<StartupModeType>();
}

void AppShellConfiguration::setStartupModeType(StartupModeType type)
{
    settings()->setSharedValue(STARTUP_MODE_TYPE, Val(type));
}

mu::io::path_t AppShellConfiguration::startupScorePath() const
{
    return settings()->value(STARTUP_SCORE_PATH).toString();
}

void AppShellConfiguration::setStartupScorePath(const io::path_t& scorePath)
{
    settings()->setSharedValue(STARTUP_SCORE_PATH, Val(scorePath.toStdString()));
}

mu::io::path_t AppShellConfiguration::userDataPath() const
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
    return MUSESCORE_VERSION + std::string(".") + MUSESCORE_BUILD_NUMBER;
}

std::string AppShellConfiguration::museScoreRevision() const
{
    return MUSESCORE_REVISION;
}

bool AppShellConfiguration::isNotationNavigatorVisible() const
{
    return uiConfiguration()->isVisible(NOTATION_NAVIGATOR_VISIBLE_KEY, false);
}

void AppShellConfiguration::setIsNotationNavigatorVisible(bool visible) const
{
    uiConfiguration()->setIsVisible(NOTATION_NAVIGATOR_VISIBLE_KEY, visible);
}

mu::async::Notification AppShellConfiguration::isNotationNavigatorVisibleChanged() const
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

mu::io::paths_t AppShellConfiguration::sessionProjectsPaths() const
{
    RetVal<ByteArray> retVal = readSessionState();
    if (!retVal.ret) {
        LOGE() << retVal.ret.toString();
        return {};
    }

    return parseSessionProjectsPaths(retVal.val.toQByteArrayNoCopy());
}

mu::Ret AppShellConfiguration::setSessionProjectsPaths(const mu::io::paths_t& paths)
{
    QJsonArray jsonArray;
    for (const io::path_t& path : paths) {
        jsonArray << QJsonValue(path.toQString());
    }

    QByteArray data = QJsonDocument(jsonArray).toJson();
    return writeSessionState(data);
}

std::string AppShellConfiguration::utmParameters(const std::string& utmMedium) const
{
    return "utm_source=desktop&utm_medium=" + utmMedium
           + "&utm_content=" + MUSESCORE_REVISION
           + "&utm_campaign=MuseScore" + MUSESCORE_VERSION;
}

std::string AppShellConfiguration::currentLanguageCode() const
{
    QString languageCode = languagesConfiguration()->currentLanguageCode().val;
    QLocale locale(languageCode);

    return locale.bcp47Name().toStdString();
}

mu::io::path_t AppShellConfiguration::sessionDataPath() const
{
    return globalConfiguration()->userAppDataPath() + "/session";
}

mu::io::path_t AppShellConfiguration::sessionFilePath() const
{
    return sessionDataPath() + SESSION_FILE;
}

mu::RetVal<mu::ByteArray> AppShellConfiguration::readSessionState() const
{
    mi::ReadResourceLockGuard lock_guard(multiInstancesProvider(), SESSION_RESOURCE_NAME);
    return fileSystem()->readFile(sessionFilePath());
}

mu::Ret AppShellConfiguration::writeSessionState(const QByteArray& data)
{
    mi::WriteResourceLockGuard lock_guard(multiInstancesProvider(), SESSION_RESOURCE_NAME);
    return fileSystem()->writeFile(sessionFilePath(), ByteArray::fromQByteArrayNoCopy(data));
}

mu::io::paths_t AppShellConfiguration::parseSessionProjectsPaths(const QByteArray& json) const
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
        io::path_t path = pathVal.toString().toStdString();
        if (!path.empty()) {
            result.push_back(path);
        }
    }

    return result;
}
