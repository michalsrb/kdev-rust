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

#include "rustfunctiondeclaration.h"

#include <language/duchain/duchainregister.h>
#include <language/duchain/types/functiontype.h>


using namespace KDevelop;

namespace Rust {

REGISTER_DUCHAIN_ITEM(RustFunctionDeclaration);

RustFunctionDeclaration::RustFunctionDeclaration(const RustFunctionDeclaration& rhs)
    : FunctionDeclaration(*new RustFunctionDeclarationData(*rhs.d_func()))
{

}

RustFunctionDeclaration::RustFunctionDeclaration(const RangeInRevision& range, DUContext* context)
    : FunctionDeclaration(*new RustFunctionDeclarationData, range)
{
    d_func_dynamic()->setClassId(this);
    if (context) {
        setContext(context);
    }
}

RustFunctionDeclaration::RustFunctionDeclaration(RustFunctionDeclarationData& data)
    : FunctionDeclaration(data)
{

}

RustFunctionDeclaration::RustFunctionDeclaration(RustFunctionDeclarationData& data, const RangeInRevision& range)
    : FunctionDeclaration(data, range)
{

}

Declaration* RustFunctionDeclaration::clonePrivate() const
{
    return new RustFunctionDeclaration(*this);
}


QString RustFunctionDeclaration::toString() const
{
    AbstractType::Ptr type = abstractType();
    if (!type)
        return Declaration::toString();

    TypePtr<FunctionType> function = type.cast<FunctionType>();
    if (function) {
        QString returnTypeString = function->partToString(FunctionType::SignatureReturn);
        QString str = QStringLiteral("%1 %2").arg(identifier().toString(), function->partToString(FunctionType::SignatureArguments));

        if (!returnTypeString.isEmpty())
            str += " -> " + returnTypeString;

        return str;
    } else {
        return Declaration::toString();
    }
}

}
