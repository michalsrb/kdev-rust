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

#ifndef RUST_RUSTHIGHLIGHTING_H
#define RUST_RUSTHIGHLIGHTING_H

#include <language/highlighting/codehighlighting.h>


namespace Rust {

class Highlighting : public KDevelop::CodeHighlighting
{
public:
    Highlighting(QObject* parent);
    virtual KDevelop::CodeHighlightingInstance* createInstance() const;
};

}

#endif // RUST_RUSTHIGHLIGHTING_H
