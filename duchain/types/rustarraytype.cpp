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

#include "rustarraytype.h"

namespace Rust {

// TODO: Finnish this to be used for rust-like array printing
QString RustArrayType::toString() const
{
//   if (d_func()->m_dimension == 0) {
//     return QStringLiteral("%1[]").arg(elementType() ? elementType()->toString() : QStringLiteral("<notype>"));
//   }
//   return QStringLiteral("%1[%2]").arg(elementType() ? elementType()->toString() : QStringLiteral("<notype>")).arg(d_func()->m_dimension);
}

}
