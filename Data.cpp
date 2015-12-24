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

#include "Data.h"

std::string Data::toS() const {
    if (type == Type::String) {
        return string;
    } else if (type == Type::Number){
        return toString(number);
    } else {
        return "";
    }
}

bool Data::empty() const {
    if (type == Type::String) {
        return string.empty();
    } else if (type == Type::Number){
        return number == 0;
    } else {
        return false;
    }
}

bool Data::operator==(const Data &other) {
    if (type != other.type)
        return false;

    if (type == Type::String) {
        return string == other.string;
    } else if (type == Type::Number){
        return number == other.number;
    } else {
        return true;
    }
}
