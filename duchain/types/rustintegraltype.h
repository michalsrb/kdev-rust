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

#ifndef RUSTLANGINTTYPE_H
#define RUSTLANGINTTYPE_H

#include <language/duchain/types/integraltype.h>

#include "kdevrustduchain_export.h"

namespace Rust {
    
typedef KDevelop::IntegralTypeData RustIntegralTypeData;

class KDEVRUSTDUCHAIN_EXPORT RustIntegralType : public KDevelop::IntegralType
{
public:
    typedef KDevelop::TypePtr<RustIntegralType> Ptr;
    
    /// Default constructor
    RustIntegralType(uint type = TypeNone);
    /// Copy constructor. \param rhs type to copy
    RustIntegralType(const RustIntegralType& rhs);
    /// Constructor using raw data. \param data internal data.
    RustIntegralType(RustIntegralTypeData& data);

    virtual KDevelop::AbstractType* clone() const;
    
    virtual QString toString() const;

    virtual bool equals(const KDevelop::AbstractType* rhs) const;

    virtual uint hash() const;
    
    enum RustIntegralTypes {
       TypeU8=201,
       TypeU16,
       TypeU32,
       TypeU64,
       TypeI8,
       TypeI16,
       TypeI32,
       TypeI64,
       TypeUsize,
       TypeIsize,
       TypeF32,
       TypeF64,
       TypeBool,
       TypeChar,
       TypeStr
   };
   
    enum {
        Identity = 203
    };
    
    typedef KDevelop::IntegralTypeData Data;
    typedef KDevelop::IntegralType BaseType;
   
protected:
    TYPE_DECLARE_DATA(RustIntegralType);

};

}


namespace KDevelop
{

template<>
inline Rust::RustIntegralType* fastCast<Rust::RustIntegralType*>(AbstractType* from) {
    if (!from || from->whichType() != AbstractType::TypeIntegral) {
        return 0;
    } else {
        return dynamic_cast<Rust::RustIntegralType*>(from);
    }
}

}


#endif
