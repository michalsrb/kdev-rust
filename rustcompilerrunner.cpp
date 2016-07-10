/*****************************************************************************
 * This file is part of kdev-rust                                            *
 * Copyright (c) 2016 Michal Srb <michalsrb@gmail.com                        *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
 *****************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QProcessEnvironment>
#include <QtCore/QStringRef>
#include <QtCore/QVector>

#include <QtCore/QDebug>

#include "settings/rustsettingsmanager.h"

#include "rustcompilerrunner.h"


namespace Rust
{

RustCompilerRunner::Result RustCompilerRunner::run(QProcess &process)
{
    // Try our best to find rustc2duchain and a way to run it.
    // If user configured path to rustc2duchain binary and to rustc
    // libraries, use them. Otherwise try to run rustc2duchain as-is
    // and with the use of rustup.

//     // Find rustc2duchain in PATH
//     QString exeFile = findInPATH();
//     if (exeFile.isEmpty()) {
//         return Result::NotFoundInPath;
//     }

    // Prepare the given QProcess
    process.setReadChannel(QProcess::StandardOutput);

    // Apply settings
    QString exeFile = "rustc2duchain";

    auto rustc2duchainSettings = RustSettingsManager::self()->rustc2duchainSettings();

    if (!rustc2duchainSettings.rustc2duchainBinary.isEmpty())
        exeFile = rustc2duchainSettings.rustc2duchainBinary;

    if (!rustc2duchainSettings.rustcLibraryPath.isEmpty()) {
        QStringList env = QProcess::systemEnvironment();
        env << "LD_LIBRARY_PATH=" + rustc2duchainSettings.rustcLibraryPath;
        process.setEnvironment(env);
    }

    // First try to just run it just as it is.
    process.setProgram(exeFile);
    if (tryRun(process))
        return Result::Success;

    // Second try to run it using rustup with nightly toolchain.
    process.setProgram("rustup");
    process.setArguments({ "run", "nightly", exeFile });
    if (tryRun(process))
        return Result::Success;

    return Result::FailedToStart;
}

bool RustCompilerRunner::tryRun(QProcess& process)
{
    process.start();
    process.waitForStarted(-1);
    if (process.state() != QProcess::ProcessState::Running)
        return false;

    while (!process.canReadLine()) {
        if (!process.waitForReadyRead(-1))
            return false;
    }

    QByteArray line = process.readLine(4);

    if (line != "OK\n")
        return false;

    return true;
}

// QString RustCompilerRunner::findInPATH()
// {
//     // Find if rustc2duchain exists in PATH.
//     // We want to know and we need the absolute path for rustup.
//
//     QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
//     for (QStringRef path : env.value("PATH").splitRef(':')) {
//         QDir dir(path.toString());
//         QString filepath = dir.filePath("rustc2duchain");
//         QFile file(filepath);
//         if (file.exists())
//             return filepath;
//     }
//
//     return QString();
// }

}
