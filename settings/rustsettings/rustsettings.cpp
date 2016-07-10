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

#include "rustsettings.h"

#include <QVBoxLayout>

#include "rustconfig.h"
#include "ui_rustsettings.h"

RustSettings::RustSettings(QWidget* parent)
    : ConfigPage(nullptr, RustConfig::self(), parent)
    , m_settings(new Ui::RustSettings)
{
    auto l = new QVBoxLayout(this);
    auto w = new QWidget(this);

    m_settings->setupUi(w);

    l->addWidget(w);
}

void RustSettings::reset()
{
    ConfigPage::reset();

    Q_ASSERT(ICore::self()->activeSession());
}

RustSettings::~RustSettings( )
{}

void RustSettings::apply()
{
    ConfigPage::apply();
}

QString RustSettings::name() const
{
    return i18n("Rust Language Support");
}

QString RustSettings::fullName() const
{
    return i18n("Rust Language Support");
}

QIcon RustSettings::icon() const
{
    return QIcon::fromTheme(QStringLiteral("kdevelop"));
}

KDevelop::ConfigPage::ConfigPageType RustSettings::configPageType() const
{
    return ConfigPage::LanguageConfigPage;
}
