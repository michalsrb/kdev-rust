/************************************************************************************
*  This file is based on file from kdev-go, original author:                        *
*  Copyright (C) 2014 by Pavel Petrushkov <onehundredof@gmail.com>                  *
*                                                                                   *
*  This file is part of kdev-rust.                                                  *
*  Copyright (C) 2016 Michal Srb <michalsrb@gmail.com>                              *
*                                                                                   *
*  This program is free software; you can redistribute it and/or                    *
*  modify it under the terms of the GNU General Public License                      *
*  as published by the Free Software Foundation; either version 2                   *
*  of the License, or (at your option) any later version.                           *
*                                                                                   *
*  This program is distributed in the hope that it will be useful,                  *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
*  GNU General Public License for more details.                                     *
*                                                                                   *
*  You should have received a copy of the GNU General Public License                *
*  along with this program; if not, write to the Free Software                      *
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
*************************************************************************************/

#include "rustintegraltype.h"

#include <language/duchain/types/typeregister.h>

using namespace KDevelop;

namespace Rust{

REGISTER_TYPE(RustIntegralType);
    
RustIntegralType::RustIntegralType(const RustIntegralType& rhs)
  : IntegralType(copyData<RustIntegralType>(*rhs.d_func()))
{
}

RustIntegralType::RustIntegralType(RustIntegralTypeData& data)
  : IntegralType(data)
{
}

RustIntegralType::RustIntegralType(uint type)
  : IntegralType(createData<RustIntegralType>())
{
  setDataType(type);
  setModifiers(ConstModifier);
}


QString RustIntegralType::toString() const
{
    
    TYPE_D(RustIntegralType);

    QString name;

    switch (d->m_dataType) {
    case TypeU8:
        name = "u8";
        break;
    case TypeU16:
        name = "u16";
        break;
    case TypeU32:
        name = "u32";
        break;
    case TypeU64:
        name = "u64";
        break;
    case TypeI8:
        name = "i8";
        break;
    case TypeI16:
        name = "i16";
        break;
    case TypeI32:
        name = "i32";
        break;
    case TypeI64:
        name = "i64";
        break;
    case TypeUsize:
        name = "usize";
        break;
    case TypeIsize:
        name = "isize";
        break;
    case TypeF32:
        name = "f32";
        break;
    case TypeF64:
        name = "f64";
        break;
    case TypeBool:
        name = "bool";
        break;
    case TypeChar:
        name = "char";
        break;
    case TypeStr:
        name = "str";
        break;
    }

    return name;
}

KDevelop::AbstractType* RustIntegralType::clone() const
{
    return new RustIntegralType(*this);
}

uint RustIntegralType::hash() const
{
    return 4 * KDevelop::IntegralType::hash();
}

bool RustIntegralType::equals(const KDevelop::AbstractType* rhs) const
{
    if( this == rhs ) {
        return true;
    }

    if ( !IntegralType::equals(rhs) ) {
        return false;
    }

    Q_ASSERT( fastCast<const RustIntegralType*>(rhs) );

    const RustIntegralType* type = static_cast<const RustIntegralType*>(rhs);

    return d_func()->m_dataType == type->d_func()->m_dataType;
}


}
