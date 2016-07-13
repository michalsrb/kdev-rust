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

#ifndef RUSTPOINTERTYPE_H
#define RUSTPOINTERTYPE_H

#include <language/duchain/types/pointertype.h>

#include "kdevrustduchain_export.h"


namespace Rust {

typedef KDevelop::PointerTypeData RustPointerTypeData;

/*
 * Custom PointerType so we can override toString for Rust-like format of pointers.
 * TODO: Is there easier way to use standard type with different printing?
 */
class KDEVRUSTDUCHAIN_EXPORT RustPointerType : public KDevelop::PointerType
{
public:
    typedef KDevelop::TypePtr<RustPointerType> Ptr;

    /// Default constructor
    RustPointerType();
    /// Copy constructor. \param rhs type to copy
    RustPointerType(const RustPointerType& rhs);
    /// Constructor using raw data. \param data internal data.
    RustPointerType(RustPointerTypeData& data);

    virtual KDevelop::AbstractType* clone() const;

    virtual QString toString() const;

    // Inheriting equals and hash from PointerType

    enum {
        ///TODO: is that value OK?
        Identity = 82
    };

    typedef KDevelop::PointerTypeData Data;
    typedef KDevelop::PointerType BaseType;

protected:
    TYPE_DECLARE_DATA(RustPointerType);
};

}


namespace KDevelop
{

template<>
inline Rust::RustPointerType* fastCast<Rust::RustPointerType*>(AbstractType* from) {
    if (!from || from->whichType() != AbstractType::TypePointer) {
        return 0;
    } else {
        return dynamic_cast<Rust::RustPointerType*>(from);
    }
}

}

#endif // RUSTPOINTERTYPE_H
