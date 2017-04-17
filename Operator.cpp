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

#include "Operator.h"

#include "Interpreter.h"
#include "Object.h"
#include <algorithm>
#include <cassert>
#include <cctype>

namespace {

void setType(Data::Type type, Object &obj) {
    if (type != Data::Type::None) {
        obj.type = Object::Type::Data;
        if (type != Data::Type::Any)
            obj.data.type = type;
    }
}

bool isAlpha(char ch) {
    return ((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'));
}

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](char ch){
                       return isAlpha(ch) ? std::tolower(ch) : ch;
                   });
    return s;
}

std::string toUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](char ch){
                       return isAlpha(ch) ? std::toupper(ch) : ch;
                   });
    return s;
}

std::string removeString(std::string s, const std::string &toRemove) {
    if (s.empty() || toRemove.empty())
        return s;

    size_t pos;
    while((pos = s.find(toRemove)) != std::string::npos) {
        std::string sub = s.substr(0, pos);

        if((pos + toRemove.length()) < s.length())
            sub += s.substr(pos + toRemove.length());

        s = sub;
    }
    return s;
}

std::string repeatString(const std::string &s, Number n) {
    std::string result;

    if (n < 1)
        return "";
    else
        for (Number i = 0; i < n; ++i)
            result += s;

    return result;
}

std::string repeatString(const Object &a, const Object &b, const Object &c, bool &success) {
    success = true;

    if ((a.data.type == Data::Type::String) && (b.data.type == Data::Type::Number) &&
        (c.data.type == Data::Type::Number)) {
            return repeatString(a.data.string, b.data.number * c.data.number);
    } else if ((a.data.type == Data::Type::Number) && (b.data.type == Data::Type::String) &&
        (c.data.type == Data::Type::Number)) {
            return repeatString(b.data.string, a.data.number * c.data.number);
    } else if ((a.data.type == Data::Type::Number) && (b.data.type == Data::Type::Number) &&
        (c.data.type == Data::Type::String)) {
            return repeatString(c.data.string, a.data.number * b.data.number);
    }

    success = false;
    return "";
}

std::string repeatString(const Object &a, const Object &b, bool &success) {
    success = true;

    if ((a.data.type == Data::Type::String) && (b.data.type == Data::Type::Number)) {
        return repeatString(a.data.string, b.data.number);
    } else if ((a.data.type == Data::Type::Number) && (b.data.type == Data::Type::String)) {
        return repeatString(b.data.string, a.data.number);
    }

    success = false;
    return "";
}

std::string replaceString(std::string s, const std::string &from, const std::string &to) {
    if (from.empty())
        return s;

    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        s.replace(pos, from.length(), to);
        pos += to.length();
    }

    return s;
}

void cutString(std::string s, Number pos, std::string &first, std::string &last) {
    if (pos < 0)
        pos = s.length() + pos;

    if (pos <= 0) {
        first = "";
        last = s;
    } else if (pos >= static_cast<Number>(s.length())) {
        first = s;
        last = "";
    }

    first = s.substr(0, pos);

    last = s.substr(pos);
}

void cutString(std::string s, const std::string &delimiter, std::string &first, std::string &last) {
    if (delimiter.empty()) {
        first = s;
        last = "";
        return;
    }

    auto pos = s.find(delimiter);

    if (pos == std::string::npos) {
        first = s;
        last = "";
    } else {
        first = s.substr(0, pos);
        pos += delimiter.size();
        last = s.substr(pos);
    }
}

Number reverse(Number n) {
    Number result = 0;
    bool negative = false;

    if (n < 0) {
        n = -n;
        negative = true;
    }

    while (n != 0) {
        result = result * 10 + (n % 10);
        n /= 10;
    }

    if (negative) {
        result *= -1;
    }

    return result;
}

std::string reverse(std::string s) {
    std::reverse(s.begin(), s.end());
    return s;
}

/*Number abs(Number n) {
    if (n < 0)
        return -n;
    else
        return n;
}*/

Number sign(Number n) {
    if (n > 0)
        return 1;
    else if (n < 0)
        return -1;
    else
        return 0;
}

bool isPrime(Number n) {
    if (n < 0)
        n = abs(n);

    if (n < 2)
        return false;

    if ( n == 2)
        return true;

    if (n % 2 == 0)
        return false;

    for (Number i = 3; (i * i) <= n; i += 2) {
        if (n % i == 0)
            return false;
    }

    return true;
}

Number gcd(Number a, Number b) {
    a = abs(a);
    b = abs(b);

    while (b != 0) {
        Number t = b;
        b = a % b;
        a = t;
    }

    return a;
}

Number find(const std::string &s, const std::string &toFind) {
    if (s.empty() || toFind.empty())
        return -1;

    size_t pos = s.find(toFind);

    if (pos == std::string::npos)
        return -1;
    else
        return static_cast<Number>(pos);
}

}


Interpreter *Operator::interpreter = nullptr;

Result Operator::processAll(std::size_t n, Object &up, Object &left, Object &right, Object &/*down*/) {
    Result result;
    result.success = true;

    setType(io[n].out.down,  result.down);
    setType(io[n].out.right, result.right);
    setType(io[n].out.left,  result.left);
    setType(io[n].out.up,    result.up);

    switch (code) {
        case Operator::Code::W:
            switch (n) {
                case 0:
                    interpreter->output(interpreter->getInputString());
                    break;
                case 1:
                    interpreter->output(up.data.toS());
                    break;
                case 2:
                    result.down.data.string = interpreter->getInputString();
                    break;
            }
            break;
        case Operator::Code::w:
            switch (n) {
                case 0:
                    interpreter->output(interpreter->getInputNumber());
                    break;
                case 1:
                    interpreter->output(up.data.toS());
                    break;
                case 2:
                    result.down.data.number = interpreter->getInputNumber();
                    break;
            }
            break;
        case Operator::Code::C:
            if (n <= 3) {
                result.up = up;
                result.up.data.direction = util::invert(result.up.data.direction);
            }
            if ((n == 0) || (n == 1) || (n == 4) || (n == 5)) {
                result.left = left;
                if (result.left.data.direction == util::Dir::Right) {
                    result.left.data.direction = util::invert(result.left.data.direction);
                }
            }
            if ((n == 0) || (n == 2) || (n == 4) || (n == 6)) {
                result.right = right;
                if (result.right.data.direction == util::Dir::Left) {
                    result.right.data.direction = util::invert(result.right.data.direction);
                }
            }
            break;
        case Operator::Code::A:
            switch (n) {
                case 0:
                    result.down.data.number = up.data.number + left.data.number + right.data.number;
                    break;
                case 1:
                    result.down.data.string = up.data.toS() + left.data.toS() + right.data.toS();
                    break;
                case 2:
                    result.down.data.number = up.data.number + left.data.number;
                    break;
                case 3:
                    result.down.data.string = up.data.toS() + left.data.toS();
                    break;
                case 4:
                    result.down.data.number = up.data.number + right.data.number;
                    break;
                case 5:
                    result.down.data.string = up.data.toS() + right.data.toS();
                    break;
                case 6:
                    result.down.data.number = left.data.number + right.data.number;
                    break;
                case 7:
                    result.down.data.string = left.data.toS() + right.data.toS();
                    break;
            }
            break;
        case Operator::Code::S:
            switch (n) {
                case 0:
                    result.down.data.number = up.data.number - left.data.number - right.data.number;
                    break;
                case 1:
                    result.down.data.string = removeString(
                                                  removeString(up.data.toS(), left.data.toS()),
                                                  right.data.toS());
                    break;
                case 2:
                    result.down.data.number = up.data.number - left.data.number;
                    break;
                case 3:
                    result.down.data.string = removeString(up.data.toS(), left.data.toS());
                    break;
                case 4:
                    result.down.data.number = up.data.number - right.data.number;
                    break;
                case 5:
                    result.down.data.string = removeString(up.data.toS(), right.data.toS());
                    break;
                case 6:
                    result.down.data.number = left.data.number - right.data.number;
                    break;
                case 7:
                    result.down.data.string = removeString(left.data.toS(), right.data.toS());
                    break;
            }
            break;
        case Operator::Code::M:
            switch (n) {
                case 0:
                    result.down.data.number = up.data.number * left.data.number * right.data.number;
                    break;
                case 1:
                    result.down.data.string = repeatString(up, left, right, result.success);
                    break;
                case 2:
                    result.down.data.number = up.data.number * left.data.number;
                    break;
                case 3:
                    result.down.data.string = repeatString(up, left, result.success);
                    break;
                case 4:
                    result.down.data.number = up.data.number * right.data.number;
                    break;
                case 5:
                    result.down.data.string = repeatString(up, right, result.success);
                    break;
                case 6:
                    result.down.data.number = left.data.number * right.data.number;
                    break;
                case 7:
                    result.down.data.string = repeatString(left, right, result.success);
                    break;
            }
            break;
        case Operator::Code::D:
            switch (n) {
                case 0:
                    if ((left.data.number == 0) || (right.data.number == 0))
                        result.success = false;
                    else
                        result.down.data.number = up.data.number / left.data.number /
                                                  right.data.number;
                    break;
                case 1:
                    if (left.data.number == 0)
                        result.success = false;
                    else
                        result.down.data.number = up.data.number / left.data.number;
                    break;
                case 2:
                    if (right.data.number == 0)
                        result.success = false;
                    else
                        result.down.data.number = up.data.number / right.data.number;
                    break;
                case 3:
                    if (right.data.number == 0)
                        result.success = false;
                    else
                        result.down.data.number = left.data.number / right.data.number;
                    break;
                case 4:
                    cutString(up.data.string, left.data.number,
                              result.down.data.string, result.right.data.string);
                    break;
                case 5:
                    cutString(up.data.string, left.data.string,
                              result.down.data.string, result.right.data.string);
                    break;
            }
            break;
        case Operator::Code::R:
            switch (n) {
                case 0:
                    if ((left.data.number == 0) || (right.data.number == 0))
                        result.success = false;
                    else
                        result.down.data.number = up.data.number % left.data.number %
                                                  right.data.number;
                    break;
                case 1:
                    result.down.data.string = replaceString(up.data.toS(), left.data.toS(),
                                                            right.data.toS());
                    break;
                case 2:
                    if (left.data.number == 0)
                        result.success = false;
                    else
                        result.down.data.number = up.data.number % left.data.number;
                    break;
                case 3:
                    if (right.data.number == 0)
                        result.success = false;
                    else
                        result.down.data.number = up.data.number % right.data.number;
                    break;
                case 4:
                    if (right.data.number == 0)
                        result.success = false;
                    else
                        result.down.data.number = left.data.number % right.data.number;
                    break;
            }
            break;
        case Operator::Code::F:
            switch (n) {
                case 0:
                    result.down.data.number = gcd(up.data.number, left.data.number);
                    break;
                case 1:
                    result.down.data.number = find(up.data.toS(), left.data.toS());
                    break;
            }
            break;
        case Operator::Code::E:
            switch (n) {
                case 0:
                    result.down.data.number = (up.data == left.data) && (up.data == right.data);
                    break;
                case 1:
                    result.down.data.number = up.data == left.data;
                    break;
                case 2:
                    result.down.data.number = up.data == right.data;
                    break;
                case 3:
                    result.down.data.number = left.data == right.data;
                    break;
            }
            break;
        case Operator::Code::G:
            switch (n) {
                case 0:
                    result.down.data.number = (up.data.number > left.data.number) &&
                                              (left.data.number > right.data.number);
                    break;
                case 1:
                    result.down.data.number = (up.data.toS() > left.data.toS()) &&
                                              (left.data.toS() > right.data.toS());
                    break;
                case 2:
                    result.down.data.number = up.data.number > left.data.number;
                    break;
                case 3:
                    result.down.data.number = up.data.toS() > left.data.toS();
                    break;
                case 4:
                    result.down.data.number = up.data.number > right.data.number;
                    break;
                case 5:
                    result.down.data.number = up.data.toS() > right.data.toS();
                    break;
                case 6:
                    result.down.data.number = left.data.number > right.data.number;
                    break;
                case 7:
                    result.down.data.number = left.data.toS() > right.data.toS();
                    break;
            }
            break;
        case Operator::Code::V:
            result.down = up;
            break;
        case Operator::Code::T:
            if (up.data.empty()) {
                result.down = Object();
            } else {
                result.down.data.number = interpreter->tick;
            }
            break;
        case Operator::Code::H:
            if (!up.data.empty()) {
                interpreter->halt();
            }
            break;
        case Operator::Code::h:
            if (!up.data.empty()) {
                interpreter->halt(true);
            }
            break;
        default:
            assert(false);
    }

    return result;
}

Result Operator::processAxis(std::size_t n, Object &first, Object &second) {
    Result result;
    result.success = true;

    setType(io[n].out.down, result.second);
    setType(io[n].out.up,   result.first);

    switch (code) {
        case Operator::Code::c:
            result.first = second;
            result.second = first;
            break;
        case Operator::Code::L:
            switch (n) {
                case 0:
                    result.second.data.number = abs(first.data.number);
                    break;
                case 1:
                    result.second.data.string = toLower(first.data.string);
                    break;
            }
            break;
        case Operator::Code::U:
            switch (n) {
                case 0:
                    result.second.data.number = sign(first.data.number);
                    break;
                case 1:
                    result.second.data.string = toUpper(first.data.string);
                    break;
            }
            break;
        case Operator::Code::P:
            switch (n) {
                case 0:
                    result.second.data.number = isPrime(first.data.number);
                    break;
                case 1:
                    result.second.data.number = static_cast<Number>(first.data.string.size());
                    break;
            }
            break;
        case Operator::Code::Z:
            switch (n) {
                case 0:
                    result.second.data.number = reverse(first.data.number);
                    break;
                case 1:
                    result.second.data.string = reverse(first.data.string);
                    break;
            }
            break;
        case Operator::Code::N:
            result.second.data.number = first.data.empty();
            break;
        case Operator::Code::K:
            switch (n) {
                case 0:
                    result.second.data.number = interpreter->rand(first.data.number);
                    break;
                case 1:
                    if (!first.data.string.empty()) {
                        result.second.data.string +=
                            first.data.string[interpreter->rand(first.data.string.size())];
                    }
                    break;
            }
            break;
        case Operator::Code::Y:
            switch (n) {
                case 0:
                    result.second.data.string = toString(first.data.number);
                    break;
                case 1:
                    if (!fromString(first.data.string, result.second.data.number))
                        result.success = false;
                    break;
                case 2:
                case 3:
                    // nothing to do
                    break;
            }
            break;
        case Operator::Code::X:
            // nothing to do
            break;
        default:
            assert(false);
    }

    return result;
}

Result Operator::processFromPipe(const Object &obj) {
    Result result;
    result.success = true;

    switch (code) {
        case Operator::Code::V:
            result.down = obj;
            break;
        case Operator::Code::X:
            // nothing to do
            break;
        case Operator::Code::T:
            if (obj.data.empty()) {
                result.down = Object();
            } else {
                result.down.type = Object::Type::Data;
                result.down.data.type = Data::Type::Number;
                result.down.data.number = interpreter->tick;
            }
            break;
        case Operator::Code::H:
            if (!obj.data.empty()) {
                interpreter->halt();
            }
            break;
        case Operator::Code::h:
            if (!obj.data.empty()) {
                interpreter->halt(true);
            }
            break;
        default:
            assert(false);
    }

    return result;
}
