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

#ifndef RUSTARRAYTYPE_H
#define RUSTARRAYTYPE_H

#include <language/duchain/types/arraytype.h>

#include "kdevrustduchain_export.h"


namespace Rust {

typedef KDevelop::ArrayTypeData RustArrayTypeData;

/*
 * Custom ArrayType so we can override toString for Rust-like format of arrays.
 * TODO: Is there easier way to use standard type with different printing?
 */
class KDEVRUSTDUCHAIN_EXPORT RustArrayType : public KDevelop::ArrayType
{
public:
    typedef KDevelop::TypePtr<RustArrayType> Ptr;

    /// Default constructor
    RustArrayType();
    /// Copy constructor. \param rhs type to copy
    RustArrayType(const RustArrayType& rhs);
    /// Constructor using raw data. \param data internal data.
    RustArrayType(RustArrayTypeData& data);

    virtual KDevelop::AbstractType* clone() const;

    virtual QString toString() const;

    // Inheriting equals and hash from ArrayType

    enum {
        Identity = 201
    };

    typedef KDevelop::ArrayTypeData Data;
    typedef KDevelop::ArrayType BaseType;

protected:
    TYPE_DECLARE_DATA(RustArrayType);
};

}


namespace KDevelop
{

template<>
inline Rust::RustArrayType* fastCast<Rust::RustArrayType*>(AbstractType* from) {
    if (!from || from->whichType() != AbstractType::TypeArray) {
        return 0;
    } else {
        return dynamic_cast<Rust::RustArrayType*>(from);
    }
}

}

#endif // RUSTARRAYTYPE_H
