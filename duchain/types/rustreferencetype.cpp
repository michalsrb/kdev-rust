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

#include "rustreferencetype.h"

#include <language/duchain/types/typeregister.h>

using namespace KDevelop;

namespace Rust {

REGISTER_TYPE(RustReferenceType);

RustReferenceType::RustReferenceType()
  : ReferenceType(createData<RustReferenceType>())
{
}

RustReferenceType::RustReferenceType(const RustReferenceType& rhs)
  : ReferenceType(copyData<RustReferenceType>(*rhs.d_func()))
{
}

RustReferenceType::RustReferenceType(RustReferenceTypeData& data)
  : ReferenceType(data)
{
}

QString RustReferenceType::toString() const
{
  QString baseString = (baseType() ? baseType()->toString() : QStringLiteral("<notype>"));
  return QStringLiteral("&%1").arg(baseString) + AbstractType::toString(true);
}


KDevelop::AbstractType* RustReferenceType::clone() const
{
    return new RustReferenceType(*this);
}

}
