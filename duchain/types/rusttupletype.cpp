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

#include "rusttupletype.h"

#include <language/duchain/types/typeregister.h>
#include <language/duchain/duchain.h>
#include <language/duchain/duchainlock.h>

#include <KLocalizedString>

using namespace KDevelop;

namespace Rust {

DEFINE_LIST_MEMBER_HASH(RustTupleTypeData, m_values, IndexedType)
REGISTER_TYPE(RustTupleType);

RustTupleType::RustTupleType() : KDevelop::StructureType(createData<RustTupleType>())
{

}

RustTupleType::RustTupleType(const RustTupleType& rhs)
    : StructureType(copyData<RustTupleType>(*rhs.d_func()))
{

}

RustTupleType::RustTupleType(RustTupleTypeData& data)
    : StructureType(data)
{

}

void RustTupleType::prependType(AbstractType::Ptr type)
{
    Q_ASSERT(type && "trying to add a null type to tuple");
    d_func_dynamic()->m_valuesList().prepend(type->indexed());
}

const IndexedType& RustTupleType::typeAt(int index) const
{
    Q_ASSERT((uint) index < d_func()->m_valuesSize());
    return d_func()->m_values()[index];
}

void RustTupleType::replaceType(int index, AbstractType::Ptr newType)
{
    Q_ASSERT((uint) index < d_func()->m_valuesSize());
    d_func_dynamic()->m_valuesList()[index] = newType->indexed();
}

KDevelop::AbstractType* RustTupleType::clone() const
{
    RustTupleType* n = new RustTupleType(*this);
    return n;
}

QString RustTupleType::toString() const
{
    QStringList typesArray;
    for ( int i = 0; i < typesCount(); i++ ) {
        if ( i > 5 ) {
            // Don't print more than five types explicitly
            typesArray << "...";
            break;
        }
        typesArray << typeAt(i).abstractType()->toString();
    }
    return "(" + typesArray.join(", ") + ")";
}

int RustTupleType::typesCount() const
{
    return d_func()->m_valuesSize();
}

bool RustTupleType::equals(const AbstractType* rhs) const
{
    if ( this == rhs ) {
        return true;
    }
    if ( ! KDevelop::StructureType::equals(rhs) ) {
        return false;
    }
    const RustTupleType* c = dynamic_cast<const RustTupleType*>(rhs);
    if ( ! c ) {
        return false;
    }
    if ( typesCount() != c->typesCount() ) {
        return false;
    }
    for ( int i = 0; i < typesCount(); i++ ) {
        if ( c->typeAt(i) != typeAt(i) ) {
            return false;
        }
    }
    return true;
}

uint RustTupleType::hash() const
{
    uint h = StructureType::hash();
    for ( uint i = 0; i < d_func()->m_valuesSize(); i++ ) {
        h += i*d_func()->m_values()[i];
    }
    return h;
}

}
