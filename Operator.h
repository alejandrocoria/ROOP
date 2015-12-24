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
#include <vector>

class Interpreter;
class Object;
struct Result;

class Operator {
public:
    struct DataRequirements {
        Data::Type up;
        Data::Type left;
        Data::Type right;
        Data::Type down;
        DataRequirements(Data::Type up = Data::Type::None, Data::Type left = Data::Type::None,
                    Data::Type right = Data::Type::None, Data::Type down = Data::Type::None):
            up(up), left(left), right(right), down(down) {}
    };
    struct DataOutcome {
        Data::Type down;
        Data::Type right;
        Data::Type left;
        Data::Type up;
        DataOutcome(Data::Type down = Data::Type::None, Data::Type right = Data::Type::None,
                    Data::Type left = Data::Type::None, Data::Type up = Data::Type::None):
            down(down), right(right), left(left), up(up) {}
    };
    struct DataPair {
        DataRequirements req;
        DataOutcome out;
    };
    using InAndOut = std::vector<DataPair>;
    enum class Code {W, w, C, c, L, U, A, S, M, D, R, F, P, Z,
                     N, E, G, K, Y,
                     V, X, T, H, h
                    };

    Result processAll(std::size_t n, Object &up, Object &left, Object &right, Object &down);
    Result processAxis(std::size_t n, Object &first, Object &second);
    Result processFromPipe(const Object &obj);

    static Interpreter *interpreter;

    Code code;
    bool noRemove;
    InAndOut io;
    bool separateAxes = false;
    bool replace = false;
private:

};
