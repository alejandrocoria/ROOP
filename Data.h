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

#include "Number.h"
#include "Util.h"
#include <string>
#include <type_traits>

class Data {
public:
    enum class Type {
        None    = 0b0000,
        Number  = 0b0001,
        String  = 0b0010,
        Input   = 0b0100,
        Output  = 0b1000,
        NorS    = Number | String,
        Any     = Number | String | Input | Output
    };

    std::string toS() const;
    bool empty() const;

    bool operator==(const Data &other);

    Type type = Type::None;
    Number number;
    std::string string;
    util::Dir direction = util::Dir::Right;
private:

};

inline bool check(Data::Type a, Data::Type b) {
    using uType = std::underlying_type<Data::Type>::type;
    return (static_cast<uType>(a) & static_cast<uType>(b)) != 0;
}
