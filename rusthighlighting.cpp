/*
    This file is based on file from kdev-php, original author:
    Copyright (C) 2010 Milian Wolff

    This file is part of kdev-rust

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

*/

#include "rusthighlighting.h"
#include <language/duchain/declaration.h>

using namespace KDevelop;

namespace Rust
{


class HighlightingInstance : public CodeHighlightingInstance {
public:
    HighlightingInstance(const CodeHighlighting* highlighting);
    virtual bool useRainbowColor(Declaration* dec) const;
};

HighlightingInstance::HighlightingInstance(const CodeHighlighting* highlighting)
    : CodeHighlightingInstance(highlighting)
{
}

bool HighlightingInstance::useRainbowColor(Declaration* dec) const
{
    return dec->context()->type() == DUContext::Function || dec->context()->type() == DUContext::Other;
}

Highlighting::Highlighting(QObject* parent)
    : CodeHighlighting(parent)
{

}

CodeHighlightingInstance* Highlighting::createInstance() const
{
    return new HighlightingInstance(this);
}

}
