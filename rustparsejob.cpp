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

#include "rustparsejob.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
// #include <QReadWriteLock>

#include <ktexteditor/document.h>
#include <threadweaver/thread.h>

#include <language/duchain/duchainlock.h>
#include <language/duchain/duchain.h>
#include <language/duchain/topducontext.h>
#include <language/duchain/declaration.h>
#include <language/duchain/problem.h>
#include <language/duchain/types/abstracttype.h>
#include <language/duchain/types/arraytype.h>
#include <language/duchain/types/delayedtype.h>
#include <language/duchain/types/pointertype.h>
#include <language/duchain/types/referencetype.h>
#include <language/duchain/types/structuretype.h>
#include <interfaces/icore.h>
#include <interfaces/idocumentcontroller.h>
#include <interfaces/ilanguagecontroller.h>
#include <interfaces/iuicontroller.h>
#include <language/backgroundparser/backgroundparser.h>
#include <language/backgroundparser/urlparselock.h>

#include "processdatastream.h"
#include "rustlanguagesupport.h"
#include "duchain/builder.h"

#include "rustcompilerrunner.h"
#include "settings/rustsettingsmanager.h"

#include <QtCore/QReadLocker>
#include <QtCore/QThread>
#include <QtCore/QByteArray>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <language/duchain/duchainutils.h>


using namespace KDevelop;

namespace Rust
{

enum MessageIn : quint8 {
    StartMark = 0,
    StopMark = 1,
    BuildType = 2,
    BuildDeclaration = 3,
    OpenContext = 4,
    CloseContext = 5,
    BuildUse = 6,
    AssignName = 7
};

enum MessageOut : quint8 {
    SetFile = 0,
    Analyze = 1
};

static const KDevelop::IndexedString rustLangString("Rust");


ParseJob::ParseJob(const IndexedString& url, ILanguageSupport* languageSupport)
    : KDevelop::ParseJob(url, languageSupport)
{
}

ParseJob::~ParseJob()
{
}

void ParseJob::run(ThreadWeaver::JobPointer /*self*/, ThreadWeaver::Thread */*thread*/)
{
    UrlParseLock urlLock(document());

    ProblemPointer p = readContents();
    if (p) {
        //TODO: associate problem with topducontext
        return abortJob();
    }

    QUrl documentUrl = document().toUrl();

    ReferencedTopDUContext topDUContext = DUChainUtils::standardContextForUrl(documentUrl);

    if (topDUContext) {
        // XXX: Never updating, always recreating from scratch. So clean the old one.
        // XXX: Not sure what exactly should I delete and what not. It seems like creating fresh ducontext would be better, but every way I tried was causing problems.

        topDUContext->clearImportedParentContexts();
        topDUContext->clearProblems();
        topDUContext->deleteChildContextsRecursively();
        topDUContext->deleteLocalDeclarations();
        topDUContext->deleteUsesRecursively();

        translateDUChainToRevision(topDUContext);
    } else {
        ParsingEnvironmentFile *file = new ParsingEnvironmentFile(document()); // TODO: This should be probably set up more
        file->setLanguage(rustLangString);
        topDUContext = new TopDUContext(document(), RangeInRevision(0, 0, INT_MAX, INT_MAX), file);
        DUChain::self()->addDocumentChain(topDUContext);
    }

    QProcess process;
    auto result = RustCompilerRunner::run(process);
    if (result != RustCompilerRunner::Result::Success) {
        QString errorMessage;

        switch (result) {
        case RustCompilerRunner::Result::NotFoundInPath:
            errorMessage = "The rustc2duchain tool was not found in PATH. You can configure custom location in Rust plugin configuration.";
            break;

        case RustCompilerRunner::Result::FailedToStart:
            errorMessage = "The rustc2duchain tool failed to start. If you updated your Rust compiler, you may need to rebuild rustc2duchain.";
            break;
        }

        // TODO: Better way to report problem with rustc2duchain?

        ProblemPointer p(new Problem());
        p->setSource(IProblem::Plugin);
        p->setDescription(errorMessage);
        p->setFinalLocation(DocumentRange(document(), KTextEditor::Range()));
        topDUContext->addProblem(p);

        abortJob();
        return;
    }

    try {
        ProcessDataStream dataStream(process);

        // Copy the content of the open rs files
        foreach(auto document, ICore::self()->documentController()->openDocuments()) {
            auto textDocument = document->textDocument();
            if (textDocument && textDocument->isModified() && textDocument->url().isLocalFile() && textDocument->mimeType() == "text/rust") {
                dataStream.writeUint8(MessageOut::SetFile);
                dataStream.writeString(textDocument->url().toLocalFile());
                dataStream.writeString(textDocument->text());
            }
        }

        // Run the analysis
        dataStream.writeUint8(MessageOut::Analyze);

        {
            // Heuristics to find the main file of the crate.
            // TODO: Use proper data from cargo.

            QString mainFileName = document().str();

            if (documentUrl.fileName() != "lib.rs" && documentUrl.fileName() != "main.rs") {
                QFileInfo documentFileInfo(document().str());
                QDir dir = documentFileInfo.dir();

                do {
                    if (dir.exists("lib.rs")) {
                        mainFileName = dir.filePath("lib.rs");
                        break;
                    } else if (dir.exists("main.rs")) {
                        mainFileName = dir.filePath("main.rs");
                        break;
                    }
                } while (dir.cdUp());
            }

            dataStream.writeString(mainFileName);
        }

        {
            // Heuristics to find library search directories.
            // TODO: Use proper data from cargo.

            QStringList librarySearchDirectories;

            QFileInfo documentFileInfo(document().str());
            QDir depsDir = documentFileInfo.dir();
            if (depsDir.cdUp() && depsDir.cd("target") && depsDir.cd("debug") && depsDir.cd("deps")) {
                librarySearchDirectories << depsDir.absolutePath();
            }

            auto rustc2duchainSettings = RustSettingsManager::self()->rustc2duchainSettings();
            if (!rustc2duchainSettings.rustcLibraryPath.isEmpty()) {
                librarySearchDirectories << rustc2duchainSettings.rustcLibraryPath;
            }

            dataStream.writeStringList(librarySearchDirectories);
        }

        waitForStartMark(dataStream);

        {
            // The DUChain is locked for the whole time we are building.
            // TODO: Should it be more granual?
            KDevelop::DUChainWriteLocker locker;

            Builder builder(topDUContext);
            while(dispatchMessage(dataStream, builder));
        }

    } catch (ProcessDataStream::Exception exception) {
        qWarning() << "Exception in data stream:" << exception.what();
    } catch (ProtocolException exception) {
        qWarning() << "Exception in protocol:" << exception.what();
    }

    process.waitForFinished(5000);

    parseProblems(process.readAllStandardError(), topDUContext);

    topDUContext->setFeatures(TopDUContext::Features::AllDeclarationsContextsAndUses); // ?

    setDuChain(topDUContext);
    highlightDUChain();
}

void ParseJob::waitForStartMark(ProcessDataStream &dataStream)
{
    MessageIn message = (MessageIn) dataStream.readUint8();
    if (message != StartMark)
        throw ProtocolException("No start mark received.");
}

QString ParseJob::readFileName(ProcessDataStream &dataStream)
{
    // Read the file id
    quint32 file_id = dataStream.readUint32();

    // Check if we already have it. If yes, return it from there.
    auto filename = m_idToFileMap.find(file_id);
    if (filename != m_idToFileMap.end())
        return *filename;

    QString newFilename = dataStream.readString();
    m_idToFileMap.insert(file_id, newFilename);

    return newFilename;
}

RangeInRevision ParseJob::readSpan(ProcessDataStream &dataStream)
{
    quint32 lo_line = dataStream.readUint32();
    quint32 lo_col = dataStream.readUint32();
    quint32 hi_line = dataStream.readUint32();
    quint32 hi_col = dataStream.readUint32();

    return RangeInRevision(lo_line - 1, lo_col, hi_line - 1, hi_col);
}

bool ParseJob::dispatchMessage(ProcessDataStream &dataStream, Builder &builder)
{
    MessageIn message = (MessageIn) dataStream.readUint8();

    switch (message) {
    case StopMark:
    {
        return false;
    }

    case BuildType:
    {
        quint8 kind = dataStream.readUint8();
        quint64 defId = dataStream.readUint64();
        quint32 size = dataStream.readUint32();

        builder.buildType((TypeKind) kind, defId, size);

        return true;
    }

    case BuildDeclaration:
    {
        quint8 kind = dataStream.readUint8();
        quint8 flags = dataStream.readUint8();
        quint64 defId = dataStream.readUint64();

        QString filename = readFileName(dataStream);
        RangeInRevision span = readSpan(dataStream);
        IndexedString name = dataStream.readIndexedString();

        if (filename != document().str()) // XXX: Throwing away everything that doesn't belong to this file.
            return true;

        builder.buildDeclaration((DeclarationKind) kind, defId, name, span, flags & 0x1, flags & 0x2);

        return true;
    }

    case OpenContext:
    {
        quint8 kind = dataStream.readUint8();
        quint8 flags = dataStream.readUint8();

        QString filename = readFileName(dataStream);
        RangeInRevision span = readSpan(dataStream);
        QString name = dataStream.readString();

        if (filename != document().str()) { // XXX: Throwing away everything that doesn't belong to this file.
            m_contextToSkip++;
            return true;
        }

        builder.openContext((ContextKind) kind, name, span, flags & 0x1);

        return true;
    }

    case CloseContext:
    {
        if (m_contextToSkip > 0) {
            m_contextToSkip--;
            return true;
        }

        builder.closeContext();
        return true;
    }

    case BuildUse:
    {
        quint64 defId = dataStream.readUint64();

        QString filename = readFileName(dataStream);
        RangeInRevision span = readSpan(dataStream);

        if (filename != document().str()) // XXX: Throwing away everything that doesn't belong to this file.
            return true;

        builder.buildUse(defId, span);

        return true;
    }

    case AssignName:
    {
        quint64 defId = dataStream.readUint64();
        QString qualifiedName = dataStream.readString();

        builder.assignName(defId, qualifiedName);

        return true;
    }

    default:
        throw ProtocolException("Got unknown message.");
    }
}

void ParseJob::parseProblems(const QByteArray stderr, ReferencedTopDUContext topDUContext)
{
    DUChainWriteLocker locker;

    int start = 0;
    while (true) {
        int end = stderr.indexOf('\n', start);
        if (end == -1)
            break; // There should be always new line after the last error, so if we didn't find new line, we are done.

        const QByteArray lineSlice = QByteArray::fromRawData(stderr.data() + start, end - start);
        start = end + 1;

        QJsonParseError parseError;
        QJsonDocument errorJsonDocument = QJsonDocument::fromJson(lineSlice, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "Failed to parse error from rustc:" << parseError.errorString();
            continue;
        }

        if (!errorJsonDocument.isObject())
            continue;

        QJsonObject errorJsonObject = errorJsonDocument.object();

        // First check that all required parameters are present
        auto messageIter = errorJsonObject.find("message");
        if (messageIter == errorJsonObject.end()) continue;

        auto spansIter = errorJsonObject.find("spans");
        if (spansIter == errorJsonObject.end()) continue;

        // Get span(s)
        QJsonArray spans = spansIter.value().toArray();
        if (spans.size() == 0) continue;

        QJsonObject firstSpan = spans.first().toObject(); // TODO: Process the other spans too.
        auto lineStartIter   = firstSpan.find("line_start");
        auto columnStartIter = firstSpan.find("column_start");
        auto lineEndIter     = firstSpan.find("line_end");
        auto columnEndIter   = firstSpan.find("column_end");
        auto fileNameIter    = firstSpan.find("file_name");

        if (lineStartIter == firstSpan.end() ||
            columnStartIter == firstSpan.end() ||
            lineEndIter == firstSpan.end() ||
            columnEndIter == firstSpan.end() ||
            fileNameIter == firstSpan.end()) continue;

        if (fileNameIter.value().toString() != document().str()) // XXX: Skipping everything that doesn't belong to this file.
            continue;

        RangeInRevision range(
            lineStartIter.value().toInt() - 1,
            columnStartIter.value().toInt() - 1,
            lineEndIter.value().toInt() - 1,
            columnEndIter.value().toInt() - 1
        );

        // Create the problem
        ProblemPointer p(new Problem());
        p->setSource(IProblem::SemanticAnalysis);

        // Set description
        p->setDescription(messageIter.value().toString());

        // Set location
        p->setFinalLocation(DocumentRange(document(), range.castToSimpleRange())); // TODO: setFinalLocation has warning that it should be called only in foreground. But other plugin seem to be calling it in ParseJob as well. Verify if it is ok.

        // Set level
        IProblem::Severity severity = IProblem::Severity::Hint;
        auto levelIter = errorJsonObject.find("level");
        if (levelIter != errorJsonObject.end()) {
            QString level = levelIter.value().toString();
            if (level == "warning")    severity = IProblem::Severity::Warning;
            else if (level == "error") severity = IProblem::Severity::Error;
        }
        p->setSeverity(severity);

        // Set explanation
        auto codeIter = errorJsonObject.find("code");
        if (codeIter != errorJsonObject.end()) {
            QJsonObject codeJsonObject = codeIter.value().toObject();
            auto codeNameIter = codeJsonObject.find("code");
            auto explanationIter = codeJsonObject.find("explanation");

            if (codeNameIter != codeJsonObject.end() && explanationIter != codeJsonObject.end()) {
                // Note: Taking only the first line of explanation. The full explanation should be available in different place and ideally linked to this shortened one.
                QString explanation = explanationIter.value().toString();
                QVector<QStringRef> explanationLines = explanation.splitRef('\n', QString::SplitBehavior::SkipEmptyParts);
                if (explanationLines.size() > 0)
                    p->setExplanation(codeNameIter.value().toString() + ": " + explanationLines.first().toString());
            }
        }

        // Add it to the top du chain
        topDUContext->addProblem(p);
    }
}


}
