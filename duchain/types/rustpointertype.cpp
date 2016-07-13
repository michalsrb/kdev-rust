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

#include "rustpointertype.h"

#include <language/duchain/types/typeregister.h>

using namespace KDevelop;

namespace Rust {

REGISTER_TYPE(RustPointerType);

RustPointerType::RustPointerType()
  : PointerType(createData<RustPointerType>())
{
}

RustPointerType::RustPointerType(const RustPointerType& rhs)
  : PointerType(copyData<RustPointerType>(*rhs.d_func()))
{
}

RustPointerType::RustPointerType(RustPointerTypeData& data)
  : PointerType(data)
{
}

QString RustPointerType::toString() const
{
  QString baseString = (baseType() ? baseType()->toString() : QStringLiteral("<notype>"));
  return QStringLiteral("*%1").arg(baseString) + AbstractType::toString(true);
}


KDevelop::AbstractType* RustPointerType::clone() const
{
    return new RustPointerType(*this);
}

}
