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

#include "Object.h"

#include <cctype>

void Object::clear() {
    type = Type::Empty;
    data.type = Data::Type::None;
    data.string.clear();
    data.direction = util::Dir::Right;
}

Object Object::CreateOperator(char ch) {
    Object obj;
    obj.type = Object::Type::NormalOperator;

    bool lower = false;
    if (std::islower(ch)) {
        ch = std::toupper(ch);
        lower = true;
    }
    obj.op.noRemove = lower;

    using DType = Data::Type;

    switch (ch) {
        case 'W':
            if (lower) {
                obj.op.code = Operator::Code::w;
                obj.op.io.push_back({{DType::Input, DType::None, DType::None, DType::Output},
                                    {}});
                obj.op.io.push_back({{DType::NorS, DType::None, DType::None, DType::Output},
                                    {}});
                obj.op.io.push_back({{DType::Input},
                                    {DType::Number}});
            } else {
                obj.op.code = Operator::Code::W;
                obj.op.io.push_back({{DType::Input, DType::None, DType::None, DType::Output},
                                    {}});
                obj.op.io.push_back({{DType::NorS, DType::None, DType::None, DType::Output},
                                    {}});
                obj.op.io.push_back({{DType::Input},
                                    {DType::String}});
                obj.op.noRemove = true;
            }
            break;
        case 'C':
            if (lower) {
                obj.op.code = Operator::Code::c;
                obj.op.io.push_back({{DType::Any,  DType::None, DType::None, DType::Any},
                                    {DType::Any,  DType::None, DType::None, DType::Any}});
                obj.op.separateAxes = true;
                obj.op.replace = true;
            } else {
                obj.op.code = Operator::Code::C;
                obj.op.noRemove = true;
                obj.op.io.push_back({{DType::Any,  DType::Any,  DType::Any,  DType::None},
                                    {DType::None,  DType::Any,  DType::Any,  DType::Any}});
                obj.op.io.push_back({{DType::Any,  DType::Any},
                                    {DType::None,  DType::None, DType::Any}});
                obj.op.io.push_back({{DType::Any,  DType::None, DType::Any},
                                    {DType::None,  DType::Any,  DType::None, DType::Any}});
                obj.op.io.push_back({{DType::Any},
                                    {DType::None,  DType::None, DType::None, DType::Any}});
                obj.op.io.push_back({{DType::None, DType::Any,  DType::Any},
                                    {DType::None,  DType::Any,  DType::Any}});
                obj.op.io.push_back({{DType::None, DType::Any,  DType::None},
                                    {DType::None,  DType::None, DType::Any}});
                obj.op.io.push_back({{DType::None, DType::None, DType::Any},
                                    {DType::None,  DType::Any}});
                obj.op.replace = true;
            }
            break;
        case 'L':
            obj.op.code = Operator::Code::L;
            obj.op.io.push_back({{DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::String},
                                {DType::String}});
            obj.op.separateAxes = true;
            break;
        case 'U':
            obj.op.code = Operator::Code::U;
            obj.op.io.push_back({{DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::String},
                                {DType::String}});
            obj.op.separateAxes = true;
            break;
        case 'A':
            obj.op.code = Operator::Code::A;
            obj.op.io.push_back({{DType::Number, DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::NorS,   DType::NorS},
                                {DType::String}});
            obj.op.io.push_back({{DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::NorS,   DType::None},
                                {DType::String}});
            obj.op.io.push_back({{DType::Number, DType::None,   DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::None,   DType::NorS},
                                {DType::String}});
            obj.op.io.push_back({{DType::None,   DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::None,   DType::NorS,   DType::NorS},
                                {DType::String}});
            break;
        case 'S':
            obj.op.code = Operator::Code::S;
            obj.op.io.push_back({{DType::Number, DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::NorS,   DType::NorS},
                                {DType::String}});
            obj.op.io.push_back({{DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::NorS,   DType::None},
                                {DType::String}});
            obj.op.io.push_back({{DType::Number, DType::None,   DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::None,   DType::NorS},
                                {DType::String}});
            obj.op.io.push_back({{DType::None,   DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::None,   DType::NorS,   DType::NorS},
                                {DType::String}});
            break;
        case 'M':
            obj.op.code = Operator::Code::M;
            obj.op.io.push_back({{DType::Number, DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::NorS,   DType::NorS},
                                {DType::String}});
            obj.op.io.push_back({{DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::NorS,   DType::None},
                                {DType::String}});
            obj.op.io.push_back({{DType::Number, DType::None,   DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::None,   DType::NorS},
                                {DType::String}});
            obj.op.io.push_back({{DType::None,   DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::None,   DType::NorS,   DType::NorS},
                                {DType::String}});
            break;
        case 'D':
            obj.op.code = Operator::Code::D;
            obj.op.io.push_back({{DType::Number, DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::Number, DType::None,   DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::None,   DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::String, DType::Number},
                                {DType::String, DType::String}});
            obj.op.io.push_back({{DType::String, DType::String},
                                {DType::String, DType::String}});
            break;
        case 'R':
            obj.op.code = Operator::Code::R;
            obj.op.io.push_back({{DType::Number, DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::NorS,   DType::NorS},
                                {DType::String}});
            obj.op.io.push_back({{DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::Number, DType::None,   DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::None,   DType::Number, DType::Number},
                                {DType::Number}});
            break;
        case 'F':
            obj.op.code = Operator::Code::F;
            obj.op.io.push_back({{DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::NorS},
                                {DType::Number}});
            break;
        case 'P':
            obj.op.code = Operator::Code::P;
            obj.op.io.push_back({{DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::String},
                                {DType::Number}});
            obj.op.separateAxes = true;
            break;
        case 'Z':
            obj.op.code = Operator::Code::Z;
            obj.op.io.push_back({{DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::String},
                                {DType::String}});
            obj.op.separateAxes = true;
            break;


        case 'N':
            obj.op.code = Operator::Code::N;
            obj.op.io.push_back({{DType::Any},
                                {DType::Number}});
            obj.op.separateAxes = true;
            break;
        case 'E':
            obj.op.code = Operator::Code::E;
            obj.op.io.push_back({{DType::Any,  DType::Any,  DType::Any},
                                {DType::Number}});
            obj.op.io.push_back({{DType::Any,  DType::Any,  DType::None},
                                {DType::Number}});
            obj.op.io.push_back({{DType::Any,  DType::None, DType::Any},
                                {DType::Number}});
            obj.op.io.push_back({{DType::None, DType::Any,  DType::Any},
                                {DType::Number}});
            break;
        case 'G':
            obj.op.code = Operator::Code::G;
            obj.op.io.push_back({{DType::Number, DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::NorS,   DType::NorS},
                                {DType::Number}});
            obj.op.io.push_back({{DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::NorS,   DType::None},
                                {DType::Number}});
            obj.op.io.push_back({{DType::Number, DType::None,   DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::NorS,   DType::None,   DType::NorS},
                                {DType::Number}});
            obj.op.io.push_back({{DType::None,   DType::Number, DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::None,   DType::NorS,   DType::NorS},
                                {DType::Number}});
            break;
        case 'K':
            obj.op.code = Operator::Code::K;
            obj.op.io.push_back({{DType::Number},
                                {DType::Number}});
            obj.op.io.push_back({{DType::String},
                                {DType::String}});
            obj.op.separateAxes = true;
            break;
        case 'Y':
            obj.op.code = Operator::Code::Y;
            obj.op.io.push_back({{DType::Number},
                                {DType::String}});
            obj.op.io.push_back({{DType::String},
                                {DType::Number}});
            obj.op.io.push_back({{DType::Input},
                                {DType::Output}});
            obj.op.io.push_back({{DType::Output},
                                {DType::Input}});
            obj.op.separateAxes = true;
            break;


        case 'V':
            obj.op.code = Operator::Code::V;
            obj.op.io.push_back({{DType::Any},
                                 {DType::Any}});
            obj.type = Object::Type::SpecialOperator;
            break;
        case 'X':
            obj.op.code = Operator::Code::X;
            obj.op.io.push_back({{DType::Any,  DType::None, DType::None, DType::Any},
                                 {}});
            obj.op.io.push_back({{DType::Any},
                                 {}});
            obj.op.io.push_back({{DType::None, DType::None, DType::None, DType::Any},
                                 {}});
            obj.op.separateAxes = true;
            obj.type = Object::Type::SpecialOperator;
            break;
        case 'T':
            obj.op.code = Operator::Code::T;
            obj.op.io.push_back({{DType::Any},
                                 {DType::Number}});
            obj.type = Object::Type::SpecialOperator;
            break;
        case 'H':
            if (lower) {
                obj.op.code = Operator::Code::h;
            } else {
                obj.op.code = Operator::Code::H;
            }
            obj.op.io.push_back({{DType::Any},
                                 {}});
            obj.type = Object::Type::SpecialOperator;
            break;
    }

    return obj;
}

Object Object::CreatePipe(char ch) {
    Object obj;
    obj.type = Object::Type::Pipe;

    switch (ch) {
        case '-': obj.pipe.type = Pipe::Type::Horizontal;   break;
        case '|': obj.pipe.type = Pipe::Type::Vertical;     break;
        case '+': obj.pipe.type = Pipe::Type::Intersection; break;
        case '*': obj.pipe.type = Pipe::Type::Bouncer;      break;
        case '>': obj.pipe.type = Pipe::Type::TurnR;        break;
        case '<': obj.pipe.type = Pipe::Type::TurnL;        break;
        case '%': obj.pipe.type = Pipe::Type::Duplicator;   break;
        case '!': obj.pipe.type = Pipe::Type::Teleport;     break;
        case 'x': obj.pipe.type = Pipe::Type::Eraser;       break;
    }

    return obj;
}

std::ostream &operator<<(std::ostream &os, const Object &obj) {
    switch (obj.type) {
        case Object::Type::Empty:
            os << ' ';
            break;
        case Object::Type::Data:
            switch (obj.data.type) {
                case Data::Type::Number: os << '$'; break;
                case Data::Type::String: os << '@'; break;
                case Data::Type::Input:  os << 'I'; break;
                case Data::Type::Output: os << 'O'; break;
                default:                 os << '?';
            }
            break;
        case Object::Type::NormalOperator:
        case Object::Type::SpecialOperator:
            switch (obj.op.code) {
                case Operator::Code::W: os << 'W'; break;
                case Operator::Code::w: os << 'w'; break;
                case Operator::Code::C: os << 'C'; break;
                case Operator::Code::c: os << 'c'; break;
                case Operator::Code::L: os << 'L'; break;
                case Operator::Code::U: os << 'U'; break;
                case Operator::Code::A: os << 'A'; break;
                case Operator::Code::S: os << 'S'; break;
                case Operator::Code::M: os << 'M'; break;
                case Operator::Code::D: os << 'D'; break;
                case Operator::Code::R: os << 'R'; break;
                case Operator::Code::F: os << 'F'; break;
                case Operator::Code::P: os << 'P'; break;
                case Operator::Code::Z: os << 'Z'; break;
                case Operator::Code::N: os << 'N'; break;
                case Operator::Code::E: os << 'E'; break;
                case Operator::Code::G: os << 'G'; break;
                case Operator::Code::K: os << 'K'; break;
                case Operator::Code::Y: os << 'Y'; break;
                case Operator::Code::V: os << 'V'; break;
                case Operator::Code::X: os << 'X'; break;
                case Operator::Code::T: os << 'T'; break;
                case Operator::Code::H: os << 'H'; break;
                case Operator::Code::h: os << 'h'; break;
            }
            break;
        case Object::Type::Pipe:
            switch (obj.pipe.type) {
                case Pipe::Type::Horizontal:    os << '-'; break;
                case Pipe::Type::Vertical:      os << '|'; break;
                case Pipe::Type::Intersection:  os << '+'; break;
                case Pipe::Type::Bouncer:       os << '*'; break;
                case Pipe::Type::TurnR:         os << '>'; break;
                case Pipe::Type::TurnL:         os << '<'; break;
                case Pipe::Type::Duplicator:    os << '%'; break;
                case Pipe::Type::Teleport:      os << '!'; break;
                case Pipe::Type::Eraser:        os << 'x'; break;
            }
            break;
        case Object::Type::Block:
            os << '#';
            break;
        default:
            os << '&';
    }
    return os;
}
