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

#ifndef RUSTFUNCTIONTYPE_H
#define RUSTFUNCTIONTYPE_H

#include <language/duchain/types/functiontype.h>

#include "kdevrustduchain_export.h"


namespace Rust {

typedef KDevelop::FunctionTypeData RustFunctionTypeData;

/*
 * Custom FunctionType so we can override partToString for Rust-like format of functions.
 * TODO: Is there easier way to use standard type with different printing?
 */
class KDEVRUSTDUCHAIN_EXPORT RustFunctionType : public KDevelop::FunctionType
{
public:
    typedef KDevelop::TypePtr<RustFunctionType> Ptr;

    /// Default constructor
    RustFunctionType();
    /// Copy constructor. \param rhs type to copy
    RustFunctionType(const RustFunctionType& rhs);
    /// Constructor using raw data. \param data internal data.
    RustFunctionType(RustFunctionTypeData& data);

    virtual KDevelop::AbstractType* clone() const;

    virtual QString partToString( SignaturePart sigPart ) const;

    // Inheriting equals and hash from FunctionType

    enum {
        ///TODO: is that value OK?
        Identity = 84
    };

    typedef KDevelop::FunctionTypeData Data;
    typedef KDevelop::FunctionType BaseType;

protected:
    TYPE_DECLARE_DATA(RustFunctionType);
};

}


namespace KDevelop
{

template<>
inline Rust::RustFunctionType* fastCast<Rust::RustFunctionType*>(AbstractType* from) {
    if (!from || from->whichType() != AbstractType::TypeFunction) {
        return 0;
    } else {
        return dynamic_cast<Rust::RustFunctionType*>(from);
    }
}

}

#endif // RUSTFUNCTIONTYPE_H
