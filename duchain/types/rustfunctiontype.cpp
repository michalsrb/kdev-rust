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

#include "rustfunctiontype.h"
#include "rusttupletype.h"

#include <language/duchain/types/typeregister.h>

using namespace KDevelop;

namespace Rust {

REGISTER_TYPE(RustFunctionType);

RustFunctionType::RustFunctionType()
  : FunctionType(createData<RustFunctionType>())
{
}

RustFunctionType::RustFunctionType(const RustFunctionType& rhs)
  : FunctionType(copyData<RustFunctionType>(*rhs.d_func()))
{
}

RustFunctionType::RustFunctionType(RustFunctionTypeData& data)
  : FunctionType(data)
{
}

KDevelop::AbstractType* RustFunctionType::clone() const
{
    return new RustFunctionType(*this);
}

QString RustFunctionType::partToString( SignaturePart sigPart ) const {
    QString args;
    TYPE_D(FunctionType);
    if (sigPart == SignatureArguments || sigPart == SignatureWhole) {
        args += '(';
        bool first = true;
        FOREACH_FUNCTION(const IndexedType& type, d->m_arguments) {
            if (first)
                first = false;
            else
                args.append(", ");

            args.append(type ? type.abstractType()->toString() : QStringLiteral("<notype>"));
        }
        args += ')';
    }

    bool hasReturnType = false;
    if (sigPart == SignatureWhole || sigPart == SignatureReturn) {
        if (returnType()) {
            RustTupleType::Ptr tuple = RustTupleType::Ptr::dynamicCast(returnType());
            if (!tuple || tuple->typesCount() > 0)
                hasReturnType = true;
        }
    }

    if (sigPart == SignatureArguments) {
        return args;
    } else if (sigPart == SignatureWhole) {
        if (hasReturnType)
            return QStringLiteral("fn%1 -> %2").arg(args, returnType()->toString());
        else
            return QStringLiteral("fn%1").arg(args);
    } else if (sigPart == SignatureReturn) {
        return hasReturnType ? returnType()->toString() : QString();
    }

    return QStringLiteral("ERROR");
}

}
