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

    Q_PROPERTY(int verticalPanelDefaultWidth READ verticalPanelDefaultWidth NOTIFY verticalPanelDefaultWidthChanged)
    int verticalPanelDefaultWidth() { return appshellConfiguration()->verticalPanelDefaultWidth(); }

    Q_PROPERTY(bool expandShowMore READ expandShowMore CONSTANT)
    bool expandShowMore() { return appshellConfiguration()->expandShowMore(); }

    Q_PROPERTY(bool hoverDisabledItems READ hoverDisabledItems CONSTANT)
    bool hoverDisabledItems() { return appshellConfiguration()->hoverDisabledItems(); }

public:
    void init();

signals:
    void verticalPanelDefaultWidthChanged(int newValue);
};
}
// krasko end

#endif // APPSHELLCONFIGURATIONPROXY_H
