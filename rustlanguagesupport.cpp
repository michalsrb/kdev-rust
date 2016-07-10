/*****************************************************************************
 * This file is based on file from kdev-php, original authors:               *
 * Copyright (c) 2007 Piyush verma <piyush.verma@gmail.com>                  *
 * Copyright (c) 2009 Niko Sams <niko.sams@gmail.com>                        *
 * Copyright (c) 2010 Milian Wolff <mail@milianw.de>                         *
 *                                                                           *
 * This file is part of kdev-rust                                            *
 * Copyright (c) 2016 Michal Srb <michalsrb@gmail.com>                       *
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

#include "rustlanguagesupport.h"

#include <QAction>
#include <QMutexLocker>
#include <QReadWriteLock>

#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <KTextEditor/Document>
#include <KActionCollection>

#include <interfaces/icore.h>
#include <interfaces/ilanguagecontroller.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/idocument.h>
#include <interfaces/iproject.h>
#include <language/backgroundparser/backgroundparser.h>
#include <language/duchain/duchain.h>
#include <language/duchain/duchaindumper.h>
#include <language/duchain/duchainlock.h>
#include <language/duchain/duchainutils.h>
#include <interfaces/idocumentcontroller.h>
#include <interfaces/contextmenuextension.h>
#include <language/interfaces/editorcontext.h>

#include "rustparsejob.h"
#include "rusthighlighting.h"
#include "codegen/refactoring.h"

#include "settings/rustsettings/rustsettings.h"

#include <language/codecompletion/codecompletion.h>
#include <language/codecompletion/codecompletionmodel.h>

#include <language/duchain/parsingenvironment.h>


using namespace KTextEditor;
using namespace KDevelop;

K_PLUGIN_FACTORY_WITH_JSON(KDevRustSupportFactory, "kdevrustsupport.json", registerPlugin<Rust::LanguageSupport>(); )

namespace Rust
{

LanguageSupport::LanguageSupport(QObject* parent, const QVariantList& /*args*/)
        : KDevelop::IPlugin(QStringLiteral("kdevrustsupport"), parent),
        KDevelop::ILanguageSupport()
{
    KDEV_USE_EXTENSION_INTERFACE(KDevelop::ILanguageSupport)
    setXMLFile(QStringLiteral("kdevrustsupport.rc"));

    m_highlighting = new Rust::Highlighting(this);
    m_refactoring = new Rust::Refactoring(this);
}

LanguageSupport::~LanguageSupport()
{
    parseLock()->lockForWrite();
    //By locking the parse-mutexes, we make sure that parse- and preprocess-jobs
    //get a chance to finish in a good state
    parseLock()->unlock();
}

KDevelop::ParseJob *LanguageSupport::createParseJob(const IndexedString &url)
{
    return new ParseJob(url, this);
}

QString LanguageSupport::name() const
{
    return QStringLiteral("Rust");
}

KDevelop::ICodeHighlighting* LanguageSupport::codeHighlighting() const
{
    return m_highlighting;
}

KDevelop::ContextMenuExtension LanguageSupport::contextMenuExtension(Context* context)
{
    ContextMenuExtension cm;
    EditorContext *ed = dynamic_cast<KDevelop::EditorContext *>(context);

    if (ed && ICore::self()->languageController()->languagesForUrl(ed->url()).contains(this)) {
        // It's safe to add our own ContextMenuExtension.
        m_refactoring->fillContextMenu(cm, context);
    }
    return cm;
}

void LanguageSupport::createActionsForMainWindow (Sublime::MainWindow* /*window*/, QString& _xmlFile, KActionCollection& actions)
{
    _xmlFile = xmlFile();

    QAction* renameDeclarationAction = actions.addAction(QStringLiteral("dump_duchain"));
    renameDeclarationAction->setText( i18n("Dump Duchain") );
    renameDeclarationAction->setIcon(QIcon::fromTheme(QStringLiteral("edit-rename")));
    actions.setDefaultShortcut(renameDeclarationAction, Qt::CTRL | Qt::SHIFT | Qt::Key_D);
    connect(renameDeclarationAction, &QAction::triggered, this, &LanguageSupport::dumpDuchainAction);
}

void LanguageSupport::dumpDuchainAction()
{
    KDevelop::DUChainReadLocker readLock(KDevelop::DUChain::lock());

    auto url = ICore::self()->documentController()->activeDocument()->url();
    KDevelop::TopDUContext *topContext = KDevelop::DUChainUtils::standardContextForUrl(url);

    if(!topContext) {
        qWarning() << "No valid top context";
        return;
    }

    DUChainDumper dumper(DUChainDumper::Feature::DumpContext);
    dumper.dump(topContext, 100);
}

KDevelop::ConfigPage* LanguageSupport::configPage(int number, QWidget* parent)
{
    return number == 0 ? new RustSettings(parent) : nullptr;
}

int LanguageSupport::configPages() const
{
    return 1;
}

}

#include "rustlanguagesupport.moc"
