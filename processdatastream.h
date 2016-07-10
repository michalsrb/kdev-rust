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

#ifndef PROCESSDATASTREAM_H
#define PROCESSDATASTREAM_H

#include <exception>

#include <QtCore/QtGlobal>
#include <QtCore/QDataStream>
#include <QtCore/QProcess>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QByteArray>

#include <language/editor/rangeinrevision.h>

#include <serialization/indexedstring.h>


namespace Rust
{

class ProcessDataStream
{
public:
    class Exception : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

public:
    ProcessDataStream(QProcess &process);

    quint8 readUint8();
    quint32 readUint32();
    quint64 readUint64();

    KDevelop::IndexedString readIndexedString();
    QString readString();

    void writeUint32(quint32 value);
    void writeUint8(quint8 value);
    void writeByteArray(const QByteArray &byteArray);
    void writeIndexedString(KDevelop::IndexedString indexedString);
    void writeString(const QString &string);
    void writeStringList(const QStringList &stringList);

private:
    void waitForBytes(qint64 bytes);

private:
    QProcess &m_process;
    QDataStream m_dataStream;
    QByteArray m_scratchBuffer;
};

}

#endif // PROCESSDATASTREAM_H
