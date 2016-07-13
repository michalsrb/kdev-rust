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

#ifndef RUSTFUNCTIONDECLARATION_H
#define RUSTFUNCTIONDECLARATION_H

#include <language/duchain/functiondeclaration.h>

#include "kdevrustduchain_export.h"

namespace Rust {

class KDEVRUSTDUCHAIN_EXPORT RustFunctionDeclarationData : public KDevelop::FunctionDeclarationData
{
public:
    RustFunctionDeclarationData() : KDevelop::FunctionDeclarationData()
    {
    }

    RustFunctionDeclarationData(const RustFunctionDeclarationData& rhs) : KDevelop::FunctionDeclarationData(rhs)
    {
    }
};

class KDEVRUSTDUCHAIN_EXPORT RustFunctionDeclaration : public KDevelop::FunctionDeclaration
{
public:

    RustFunctionDeclaration(const RustFunctionDeclaration& rhs);
    RustFunctionDeclaration(const KDevelop::RangeInRevision& range, KDevelop::DUContext* context);
    RustFunctionDeclaration(RustFunctionDeclarationData& data);
    RustFunctionDeclaration(RustFunctionDeclarationData& data, const KDevelop::RangeInRevision& range);

    virtual QString toString() const override;

    enum {
        Identity = 221
    };

    typedef KDevelop::FunctionDeclaration Base;

private:
    virtual KDevelop::Declaration* clonePrivate() const override;

    DUCHAIN_DECLARE_DATA(RustFunctionDeclaration);

};

}

#endif
