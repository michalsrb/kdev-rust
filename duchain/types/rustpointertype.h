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
#include <language/duchain/types/typesystemdata.h>

#include "kdevrustduchain_export.h"


namespace Rust {

enum PointerKind {
    Raw,
    Box
};

class KDEVRUSTDUCHAIN_EXPORT RustPointerTypeData : public KDevelop::PointerTypeData
{
public:
    /// Constructor
    RustPointerTypeData() : KDevelop::PointerTypeData() {
        m_kind = PointerKind::Raw;
    }

    /// Copy constructor. \param rhs data to copy
    RustPointerTypeData(const RustPointerTypeData& rhs) : KDevelop::PointerTypeData(rhs) {
        m_kind = rhs.m_kind;
    }

    /// Pointer type
    PointerKind m_kind;
};

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

    PointerKind kind() const;

    void setKind(PointerKind kind);

    virtual KDevelop::AbstractType* clone() const;

    virtual QString toString() const;

    virtual bool equals(const AbstractType* rhs) const override;

    virtual uint hash() const override;

    enum {
        Identity = 204
    };

    typedef RustPointerTypeData Data;
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
