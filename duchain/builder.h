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

#ifndef BUILDER_H
#define BUILDER_H

#include <stack>

#include <QtCore/QMap>
#include <QtCore/QMultiMap>
#include <QtCore/QPair>
#include <QtCore/QStack>
#include <QtCore/QString>
#include <QtCore/QVector>

#include <language/duchain/types/identifiedtype.h>

#include <language/duchain/declaration.h>
#include <language/duchain/duchainlock.h>
#include <language/duchain/topducontext.h>


namespace KDevelop {
    class AbstractType;
    class DUContext;
    class Declaration;
    class TopDUContext;
    class RangeInRevision;
    class IndexedString;
};

namespace Rust
{

typedef quint64 DefId;

enum class DeclarationKind : quint8 {
    Instance = 0,
    Function = 1,
    Type = 2, // TODO: Replace with specific ones?
    Namespace = 3,
    Struct = 4,
    Trait = 5
};

enum class ContextKind : quint8 {
    Namespace = 0,
    Class = 1,
    Function = 2,
    Other = 3
};

enum class TypeKind : quint8 {
    U8 = 0,
    U16 = 1,
    U32 = 2,
    U64 = 3,
    I8 = 4,
    I16 = 5,
    I32 = 6,
    I64 = 7,
    Usize = 8,
    Isize = 9,
    F32 = 10,
    F64 = 11,
    Bool = 12,
    Char = 13,
    Str = 14,
    Array = 15,
    Ref = 16,
    RawPtr = 17,
    BoxPtr = 18,
    Struct = 19,
    Tuple = 20,
    Function = 21
};

class Builder
{
private:
    struct CurrentContext
    {
        CurrentContext(KDevelop::DUContext* context);

        CurrentContext(const CurrentContext& other) = delete;
        CurrentContext(CurrentContext&& other);

        ~CurrentContext();

        KDevelop::DUContext* context;
        // when updating, this contains child contexts of the current parent context
        QVector<KDevelop::DUContext*> previousChildContexts;
        // when updating, this contains child declarations of the current parent context
        QVector<KDevelop::Declaration*> previousChildDeclarations;

        bool resortChildContexts = false;
        bool resortLocalDeclarations = false;
    };

public:
    Builder(KDevelop::ReferencedTopDUContext topDUContext, bool update);

    void buildType(TypeKind kind, DefId defId, quint32 size);
    void buildDeclaration(DeclarationKind kind, DefId defId, const KDevelop::IndexedString &name, const KDevelop::RangeInRevision &span, bool isDefinition, bool useLastType);
    void openContext(Rust::ContextKind kind, const QString name, const KDevelop::RangeInRevision &span, bool belongsToLastDeclaration);
    void closeContext();
    void buildUse(DefId defId, const KDevelop::RangeInRevision &span);
    void assignName(DefId defId, QString qualifiedName);

    KDevelop::ReferencedTopDUContext topDUContext() const { return m_topDUContext; }

private:
    KDevelop::AbstractType::Ptr popType();

    template <class T>
    KDevelop::Declaration *createDeclaration(const KDevelop::RangeInRevision &range, const KDevelop::Identifier &identifier) {
        auto &parentContext = m_contextStack.top();

        if (m_update) {
            const KDevelop::IndexedIdentifier indexed_identifier(identifier);
            for (auto it = parentContext.previousChildDeclarations.begin(); it != parentContext.previousChildDeclarations.end(); ++it) {
                KDevelop::Declaration *decl = dynamic_cast<T*>(*it);
                if (!decl || decl->indexedIdentifier() != indexed_identifier)
                    continue;

                decl->setRange(range);
                parentContext.resortLocalDeclarations = true;
                parentContext.previousChildDeclarations.erase(it);
                return decl;
            }
        }

        KDevelop::Declaration *decl = new T(range, parentContext.context);
        decl->setIdentifier(identifier);
        return decl;
    }

    KDevelop::DUContext *createContext(const KDevelop::RangeInRevision &range, KDevelop::DUContext::ContextType type, const KDevelop::QualifiedIdentifier& scopeId);

private:
    QStack<KDevelop::AbstractType::Ptr> m_typeStack;

    QMap<DefId, QString> m_assignedNames;
    QMap<DefId, KDevelop::DeclarationPointer> m_declarationMap;
    QMultiMap<DefId, QPair<KDevelop::DUContext*, KDevelop::RangeInRevision>> m_usesMissingDeclaration;
    KDevelop::DeclarationPointer m_lastDeclaration;

    KDevelop::ReferencedTopDUContext m_topDUContext;
    std::stack<CurrentContext> m_contextStack;

    bool m_update;
};

}

#endif // BUILDER_H
