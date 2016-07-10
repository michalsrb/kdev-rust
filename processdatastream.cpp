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

#include "processdatastream.h"

namespace Rust
{

ProcessDataStream::ProcessDataStream(QProcess& process)
    : m_process(process)
    , m_dataStream(&process)
{
    m_dataStream.setByteOrder(QDataStream::LittleEndian);
}

void ProcessDataStream::waitForBytes(qint64 bytes)
{
    while (m_process.bytesAvailable() < bytes) {
        if (!m_process.waitForReadyRead(-1))
            throw Exception("Process quited unexpectedly.");
    }
}

quint8 ProcessDataStream::readUint8()
{
    waitForBytes(sizeof(quint8));
    quint8 value;
    m_dataStream >> value;
    return value;
}

quint32 ProcessDataStream::readUint32()
{
    waitForBytes(sizeof(quint32));
    quint32 value;
    m_dataStream >> value;
    return value;
}

quint64 ProcessDataStream::readUint64()
{
    waitForBytes(sizeof(quint64));
    quint64 value;
    m_dataStream >> value;
    return value;
}

QString ProcessDataStream::readString()
{
    quint32 str_len = readUint32();

    if (str_len == 0)
        return QString();

    m_scratchBuffer.resize(str_len); // TODO: Verify that this doesn't resize the buffer down if it is bigger.

    waitForBytes(str_len);
    if ((quint32) m_dataStream.readRawData(m_scratchBuffer.data(), str_len) != str_len)
        throw Exception("Data stream didn't contain whole name.");

    return QString::fromUtf8(m_scratchBuffer.data(), str_len);
}

KDevelop::IndexedString ProcessDataStream::readIndexedString()
{
    quint32 name_len = readUint32();

    if (name_len == 0)
        throw Exception("Got zero length name while expecting non-zero.");

    m_scratchBuffer.resize(name_len); // TODO: Verify that this doesn't resize the buffer down if it is bigger.

    waitForBytes(name_len);
    if ((quint32) m_dataStream.readRawData(m_scratchBuffer.data(), name_len) != name_len)
        throw Exception("Data stream didn't contain whole name.");

    return KDevelop::IndexedString(m_scratchBuffer.data(), name_len);
}

void ProcessDataStream::writeUint8(quint8 value)
{
    m_dataStream << value;
}

void ProcessDataStream::writeUint32(quint32 value)
{
    m_dataStream << value;
}

void ProcessDataStream::writeIndexedString(KDevelop::IndexedString indexedString)
{
    int length = indexedString.length();
    m_dataStream << (quint32) length;
    m_dataStream.writeRawData(indexedString.c_str(), length);
}

void ProcessDataStream::writeByteArray(const QByteArray& byteArray)
{
    m_dataStream << (quint32) byteArray.length();
    m_dataStream.writeRawData(byteArray.data(), byteArray.length());
}

void ProcessDataStream::writeString(const QString& string)
{
    writeByteArray(string.toUtf8());
}

void ProcessDataStream::writeStringList(const QStringList& stringList)
{
    m_dataStream << (quint32) stringList.length();
    for (const QString &string : stringList) {
        writeString(string);
    }
}

}
