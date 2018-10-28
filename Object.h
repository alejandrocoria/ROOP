/*
ROOP - Interpreter for the esoteric programming language ROOP.
Copyright (C) 2015 Alejandro O. Coria Bayer

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#pragma once

#include "Data.h"
#include "Operator.h"
#include "Pipe.h"
#include <array>
#include <iostream>

struct Result;

class Object {
public:
    enum class Type {
        Empty           = 0b00000,
        Data            = 0b00001,
        NormalOperator  = 0b00010,
        SpecialOperator = 0b00100,
        Pipe            = 0b01000,
        Block           = 0b10000,
        Operator        = NormalOperator | SpecialOperator
    };

    void clear();

    static Object CreateOperator(char ch);
    static Object CreatePipe(char ch);

    Type type = Type::Empty;
    Data data;
    Operator op;
    Pipe pipe;
private:

};

struct Result {
    bool success;
    Object up;
    Object left;
    Object right;
    Object down;
    Object &first = up;
    Object &second = down;
};

std::ostream& operator<<(std::ostream &os, const Object &obj);

inline bool check(Object::Type a, Object::Type b) {
    using uType = std::underlying_type<Object::Type>::type;
    return (static_cast<uType>(a) & static_cast<uType>(b)) != 0;
}
