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

#include <limits>

#include <QtCore/QDebug>

#include <language/duchain/duchainlock.h>
#include <language/duchain/duchain.h>
#include <language/duchain/duchainutils.h>
#include <language/duchain/topducontext.h>
#include <language/duchain/declaration.h>
#include <language/duchain/types/abstracttype.h>
#include <language/duchain/types/delayedtype.h>
#include <language/duchain/types/structuretype.h>

#include "declarations/rustfunctiondeclaration.h"

#include "types/rustarraytype.h"
#include "types/rustfunctiontype.h"
#include "types/rustintegraltype.h"
#include "types/rustpointertype.h"
#include "types/rustreferencetype.h"
#include "types/rusttupletype.h"

#include "builder.h"


using namespace KDevelop;

namespace Rust
{


Builder::Builder(KDevelop::ReferencedTopDUContext topDUContext)
{
    m_topDUContext = topDUContext;
    m_contextStack.push(m_topDUContext);
}

AbstractType::Ptr Builder::popType()
{
    if (m_typeStack.size() == 0) {
        qWarning() << "Requesting type from stack, but it is empty!";

        DelayedType *type = new DelayedType();
        type->setKind(DelayedType::Unresolved);
        return AbstractType::Ptr(type);
    }

    return m_typeStack.pop();
}


void Builder::buildType(TypeKind kind, DefId defId, uint32_t size) {
    qWarning() << "Builder::buildType(kind:" << (int) kind << ", defId:" << defId << ", size:" << size << ")";

    AbstractType *type = nullptr;
    IdentifiedType *identified_type = nullptr;

    switch (kind) {
        case TypeKind::U8:
        case TypeKind::U16:
        case TypeKind::U32:
        case TypeKind::U64:
        case TypeKind::I8:
        case TypeKind::I16:
        case TypeKind::I32:
        case TypeKind::I64:
        case TypeKind::Usize:
        case TypeKind::Isize:
        case TypeKind::F32:
        case TypeKind::F64:
        case TypeKind::Bool:
        case TypeKind::Char:
        case TypeKind::Str:
        {
            RustIntegralType::RustIntegralTypes integralType;
            switch (kind) {
                case TypeKind::U8:    integralType = RustIntegralType::TypeU8;    break;
                case TypeKind::U16:   integralType = RustIntegralType::TypeU16;   break;
                case TypeKind::U32:   integralType = RustIntegralType::TypeU32;   break;
                case TypeKind::U64:   integralType = RustIntegralType::TypeU64;   break;
                case TypeKind::I8:    integralType = RustIntegralType::TypeI8;    break;
                case TypeKind::I16:   integralType = RustIntegralType::TypeI16;   break;
                case TypeKind::I32:   integralType = RustIntegralType::TypeI32;   break;
                case TypeKind::I64:   integralType = RustIntegralType::TypeI64;   break;
                case TypeKind::Usize: integralType = RustIntegralType::TypeUsize; break;
                case TypeKind::Isize: integralType = RustIntegralType::TypeIsize; break;
                case TypeKind::F32:   integralType = RustIntegralType::TypeF32;   break;
                case TypeKind::F64:   integralType = RustIntegralType::TypeF64;   break;
                case TypeKind::Bool:  integralType = RustIntegralType::TypeBool;  break;
                case TypeKind::Char:  integralType = RustIntegralType::TypeChar;  break;
                case TypeKind::Str:   integralType = RustIntegralType::TypeStr;   break;
                default: Q_ASSERT(!"Not reached");
            }
            type = new RustIntegralType(integralType);

            break;
        }

        case TypeKind::Array:
        {
            RustArrayType *array_type = new RustArrayType();
            array_type->setElementType(popType());
            array_type->setDimension(size);
            type = array_type;

            break;
        }

        case TypeKind::Ref:
        {
            RustReferenceType *reference_type = new RustReferenceType();
            reference_type->setBaseType(popType());
            type = reference_type;

            break;
        }

        case TypeKind::RawPtr:
        {
            RustPointerType *pointer_type = new RustPointerType();
            pointer_type->setBaseType(popType());
            pointer_type->setKind(PointerKind::Raw);
            type = pointer_type;

            break;
        }

        case TypeKind::BoxPtr:
        {
            RustPointerType *pointer_type = new RustPointerType();
            pointer_type->setBaseType(popType());
            pointer_type->setKind(PointerKind::Box);
            type = pointer_type;

            break;
        }

        case TypeKind::Struct:
        {
            StructureType *struct_type = new StructureType();
            identified_type = struct_type;
            type = struct_type;

            break;
        }

        case TypeKind::Tuple:
        {
            RustTupleType *tuple_type = new RustTupleType();
            for (uint32_t i = 0; i < size; i++) {
                tuple_type->prependType(popType());
            }
            type = tuple_type;

            break;
        }

        case TypeKind::Function:
        {
            RustFunctionType *function_type = new RustFunctionType();
            for (uint32_t i = 0; i < size; i++) {
                function_type->addArgument(popType(), 0);
            }
            function_type->setReturnType(popType());
            type = function_type;

            break;
        }
    }

    AbstractType::Ptr type_ptr(type);

    if (identified_type) {
        if (defId != INT64_MAX) {
            auto declarationIter = m_declarationMap.find(defId);
            if (declarationIter == m_declarationMap.end()) {
                auto nameIter = m_assignedNames.find(defId);
                if (nameIter != m_assignedNames.end()) {
                    identified_type->setDeclarationId(DeclarationId(QualifiedIdentifier(*nameIter)));
                }
            } else {
                identified_type->setDeclaration(declarationIter->data());
            }
        }
    }

    m_typeStack.push(type_ptr);
}

void Builder::buildDeclaration(DeclarationKind kind, DefId defId, const IndexedString &name, const RangeInRevision &span, bool isDefinition, bool useLastType)
{
    qWarning() << "Builder::buildDeclaration(kind:" << (int)kind
        << ", defId:" << defId
        << ", name:" << name
        << ", span:" << span.start.line << ":" << span.start.column << ".." << span.end.line << ":" << span.end.column
        << ", isDefinition:" << isDefinition
        << ", useLastType:" << useLastType
        << ")";

    Declaration *newDeclaration;

    switch (kind) {
        case DeclarationKind::Instance:
            newDeclaration = new Declaration(span, m_contextStack.top());
            newDeclaration->setKind(Declaration::Kind::Instance);
            break;

        case DeclarationKind::Function:
            newDeclaration = new RustFunctionDeclaration(span, m_contextStack.top());
            newDeclaration->setKind(Declaration::Kind::Instance);
            break;

        case DeclarationKind::Type:
            newDeclaration = new Declaration(span, m_contextStack.top());
            newDeclaration->setKind(Declaration::Kind::Type);
            break;

        case DeclarationKind::Namespace:
            newDeclaration = new Declaration(span, m_contextStack.top());
            newDeclaration->setKind(Declaration::Kind::Namespace);
            break;

        default:
            qWarning() << "Unknown declaration type " << (int) kind;
            return;
    }

    DeclarationPointer declaration(newDeclaration);

    declaration->setIdentifier(Identifier(name));

    declaration->setDeclarationIsDefinition(isDefinition);

    if (useLastType) {
        declaration->setAbstractType(popType());
    }

    if (defId != std::numeric_limits<quint64>::max()) {
        m_declarationMap.insert(defId, declaration);

        {
            auto indexForDeclaration = m_topDUContext->indexForUsedDeclaration(declaration.data());

            auto iter = m_usesMissingDeclaration.find(defId);
            while (iter != m_usesMissingDeclaration.end() && iter.key() == defId) {
                iter->first->createUse(indexForDeclaration, iter->second);
                iter = m_usesMissingDeclaration.erase(iter);
            }
        }
    }

    m_lastDeclaration = declaration;
}

void Builder::openContext(Rust::ContextKind kind, const QString name, const RangeInRevision &span, bool belongsToLastDeclaration)
{
    qWarning() << "Builder::openContext("
        << "name:" << (name.isEmpty() ? "anonymous" : name)
        << ", span:" << span.start.line << ":" << span.start.column << ".." << span.end.line << ":" << span.end.column
        << ", belongsToLastDeclaration:" << belongsToLastDeclaration
        << ")";

    DUContext *context = new DUContext(span, m_contextStack.top());

    if (!name.isEmpty())
        context->setLocalScopeIdentifier(QualifiedIdentifier(name));

    switch(kind) {
        case ContextKind::Namespace:
            context->setType(DUContext::ContextType::Namespace);
            break;
        case ContextKind::Class:
            context->setType(DUContext::ContextType::Class);
            break;
        case ContextKind::Function:
            context->setType(DUContext::ContextType::Function);
            break;
        case ContextKind::Other:
            context->setType(DUContext::ContextType::Other);
            break;
        default:
            qWarning() << "Unknown context type " << (int) kind;
            break;
    }

    if (belongsToLastDeclaration) {
        if (m_lastDeclaration) {
            context->setOwner(m_lastDeclaration.data());
        } else {
            qWarning() << "Creating context belonging to last declaration, but there is none!";
        }
    }

    m_contextStack.push(context);
}

void Builder::closeContext()
{
    qWarning() << "Builder::closeContext()";

    m_contextStack.pop();
}

void Builder::buildUse(DefId defId, const KDevelop::RangeInRevision &span)
{
    qWarning() << "Builder::buildUse("
        << "defId:" << defId
        << ", span:" << span.start.line << ":" << span.start.column << ".." << span.end.line << ":" << span.end.column
        << ")";

    auto declaration = m_declarationMap.find(defId);

    if (declaration != m_declarationMap.end()) {
        m_contextStack.top()->createUse(m_topDUContext->indexForUsedDeclaration(declaration->data()), span);
    } else {
        m_usesMissingDeclaration.insert(defId, qMakePair(m_contextStack.top(), span));
    }
}

void Rust::Builder::assignName(DefId defId, QString qualifiedName)
{
    m_assignedNames.insert(defId, qualifiedName);
}



}
