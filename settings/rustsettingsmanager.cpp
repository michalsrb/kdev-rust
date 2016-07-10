/*
 * This file is based on file from KDevelop, original author:
 * Copyright 2015 Sergey Kalinichev <kalinichev.so.0@gmail.com>
 *
 * Copyright 2016 Michal Srb <michalsrb@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "rustsettingsmanager.h"

#include <KConfigGroup>
#include <KConfig>

#include <QtCore/QThread>
#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>

#include <interfaces/icore.h>
#include <interfaces/isession.h>
#include <interfaces/iproject.h>
#include <util/path.h>

#include <project/projectmodel.h>

using namespace KDevelop;

namespace
{

const QString settingsGroup = QStringLiteral("Rust Settings");

const QString rustc2duchainBinary = QStringLiteral("rustc2duchainBinary");
const QString rustcLibraryPath = QStringLiteral("rustcLibraryPath");

Rustc2duchainSettings readRustc2DuchainSettings(KConfig* cfg)
{
    auto grp = cfg->group(settingsGroup);
    Rustc2duchainSettings settings;

    settings.rustc2duchainBinary = grp.readPathEntry(rustc2duchainBinary, QString());
    if (!settings.rustc2duchainBinary.isEmpty())
        settings.rustc2duchainBinary = QUrl(settings.rustc2duchainBinary).path();

    settings.rustcLibraryPath = grp.readPathEntry(rustcLibraryPath, QString());
    if (!settings.rustcLibraryPath.isEmpty())
        settings.rustcLibraryPath = QUrl(settings.rustcLibraryPath).path();

    return settings;
}

}

RustSettingsManager* RustSettingsManager::self()
{
    static RustSettingsManager manager;
    return &manager;
}

Rustc2duchainSettings RustSettingsManager::rustc2duchainSettings() const
{
    auto cfg = ICore::self()->activeSession()->config();
    return readRustc2DuchainSettings(cfg.data());
}

RustSettingsManager::RustSettingsManager()
{}
