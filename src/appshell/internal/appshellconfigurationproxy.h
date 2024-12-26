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
#ifndef MU_APPSHELL_APPSHELLCONFIGURATIONPROXY_H
#define MU_APPSHELL_APPSHELLCONFIGURATIONPROXY_H

// krasko start
#include "async/asyncable.h"
#include "modularity/ioc.h"

#include "../iappshellconfiguration.h"

namespace mu::appshell {
class AppShellConfigurationProxy : public QObject, muse::async::Asyncable
{
    Q_OBJECT

    Inject<mu::appshell::IAppShellConfiguration> appshellConfiguration;

    Q_PROPERTY(bool focusExportButtonOnExportDialog READ focusExportButtonOnExportDialog CONSTANT)
    bool focusExportButtonOnExportDialog() { return appshellConfiguration()->focusExportButtonOnExportDialog(); }

    Q_PROPERTY(int verticalPanelsWidth READ verticalPanelsWidth NOTIFY verticalPanelsWidthChanged)
    int verticalPanelsWidth() { return appshellConfiguration()->verticalPanelsWidth(); }

    Q_PROPERTY(bool expandShowMore READ expandShowMore CONSTANT)
    bool expandShowMore() { return appshellConfiguration()->expandShowMore(); }

    Q_PROPERTY(bool hoverDisabledItems READ hoverDisabledItems CONSTANT)
    bool hoverDisabledItems() { return appshellConfiguration()->hoverDisabledItems(); }

    Q_PROPERTY(bool showScrollbarOnDropDownLists READ showScrollbarOnDropDownLists CONSTANT)
    bool showScrollbarOnDropDownLists() { return appshellConfiguration()->showScrollbarOnDropDownLists(); }

public:
    void init();

signals:
    void verticalPanelsWidthChanged(int newValue);
};
}
// krasko end

#endif // APPSHELLCONFIGURATIONPROXY_H
