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

#ifndef RUST_PARSEJOB_H
#define RUST_PARSEJOB_H

#include <language/backgroundparser/parsejob.h>
#include <language/duchain/problem.h>

#include <QMap>

#include <ktexteditor/range.h>

#include <language/duchain/types/identifiedtype.h>


namespace KDevelop
{
    class AbstractType;
    class DUContext;
    class Declaration;
    class TopDUContext;
};

namespace Rust
{

class LanguageSupport;

class Builder;
class ProcessDataStream;

class ParseJob : public KDevelop::ParseJob
{
    Q_OBJECT

public:
    explicit ParseJob(const KDevelop::IndexedString& url, KDevelop::ILanguageSupport* LanguageSupport);
    virtual ~ParseJob();

protected:
    virtual void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread);

private:
    class ProtocolException : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

private:
    void waitForStartMark(ProcessDataStream &dataStream);

    bool dispatchMessage(ProcessDataStream &dataStream, Builder &builder);

    void parseProblems(const QByteArray stderr, KDevelop::ReferencedTopDUContext topducontext);

    QString readFileName(ProcessDataStream &dataStream);
    KDevelop::RangeInRevision readSpan(ProcessDataStream &dataStream);

private:
    QMap<quint32, QString> m_idToFileMap;

    int m_contextToSkip = 0; // Temporary, can be removed once we know how to process multiple files at once.
};

}

#endif
