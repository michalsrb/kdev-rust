/**
    This file is based on file from kdev-python, original author:
    Copyright (C) 2011 Sven Brauch <svenbrauch@googlemail.com>

    This file is part of kdev-rust.
    Copyright (C) 2016 Michal Srb <michalsrb@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/


#ifndef RUSTTUPLETYPE_H
#define RUSTTUPLETYPE_H

#include <language/duchain/types/structuretype.h>
#include <language/duchain/types/typesystemdata.h>

#include "rustduchainexport.h"

using namespace KDevelop;

namespace Rust {

KDEVRUSTDUCHAIN_EXPORT DECLARE_LIST_MEMBER_HASH(RustTupleTypeData, m_values, IndexedType)

class KDEVRUSTDUCHAIN_EXPORT RustTupleTypeData : public KDevelop::StructureTypeData
{
public:
    /// Constructor
    RustTupleTypeData()
        : KDevelop::StructureTypeData()
    {
        initializeAppendedLists(m_dynamic);
    }
    /// Copy constructor. \param rhs data to copy
    RustTupleTypeData( const RustTupleTypeData& rhs )
        : KDevelop::StructureTypeData(rhs)
    {
        initializeAppendedLists(m_dynamic);
        copyListsFrom(rhs);
    }
    
    ~RustTupleTypeData() {
        freeAppendedLists();
    };
    
    START_APPENDED_LISTS_BASE(RustTupleTypeData, StructureTypeData)
    APPENDED_LIST_FIRST(RustTupleTypeData, IndexedType, m_values)
    END_APPENDED_LISTS(RustTupleTypeData, m_values)
};


class KDEVRUSTDUCHAIN_EXPORT RustTupleType : public KDevelop::StructureType
{
public:
    typedef TypePtr<RustTupleType> Ptr;
    
    RustTupleType();
    RustTupleType(const RustTupleType& rhs);
    RustTupleType(RustTupleTypeData& data);
    void prependType(AbstractType::Ptr type);
    virtual AbstractType* clone() const;
    virtual uint hash() const;
    int typesCount() const;
    const IndexedType& typeAt(int index) const;
    void replaceType(int index, AbstractType::Ptr newType);
    virtual QString toString() const;

    virtual bool equals(const AbstractType* rhs) const;

    enum {
        Identity = 80
    };

    typedef RustTupleTypeData Data;
    typedef KDevelop::StructureType BaseType;

protected:
    TYPE_DECLARE_DATA(RustTupleType);
};

}

#endif // RUSTTUPLETYPE_H
