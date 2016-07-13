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

#ifndef RUSTREFERENCETYPE_H
#define RUSTREFERENCETYPE_H

#include <language/duchain/types/referencetype.h>

#include "kdevrustduchain_export.h"


namespace Rust {

typedef KDevelop::ReferenceTypeData RustReferenceTypeData;

/*
 * Custom ReferenceType so we can override toString for Rust-like format of references.
 * TODO: Is there easier way to use standard type with different printing?
 */
class KDEVRUSTDUCHAIN_EXPORT RustReferenceType : public KDevelop::ReferenceType
{
public:
    typedef KDevelop::TypePtr<RustReferenceType> Ptr;

    /// Default constructor
    RustReferenceType();
    /// Copy constructor. \param rhs type to copy
    RustReferenceType(const RustReferenceType& rhs);
    /// Constructor using raw data. \param data internal data.
    RustReferenceType(RustReferenceTypeData& data);

    virtual KDevelop::AbstractType* clone() const;

    virtual QString toString() const;

    // Inheriting equals and hash from ReferenceType

    enum {
        ///TODO: is that value OK?
        Identity = 83
    };

    typedef KDevelop::ReferenceTypeData Data;
    typedef KDevelop::ReferenceType BaseType;

protected:
    TYPE_DECLARE_DATA(RustReferenceType);
};

}


namespace KDevelop
{

template<>
inline Rust::RustReferenceType* fastCast<Rust::RustReferenceType*>(AbstractType* from) {
    if (!from || from->whichType() != AbstractType::TypeReference) {
        return 0;
    } else {
        return dynamic_cast<Rust::RustReferenceType*>(from);
    }
}

}

#endif // RUSTREFERENCETYPE_H
