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

#include "Interpreter.h"

#include "Number.h"
#include "Util.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

namespace {

const std::string lowerLetters = "abcdefghijklmnopqrstuvwxyz";
const std::string upperLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string allLetters =   "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string digits =       "0123456789";
const std::string octalDigits =  "01234567";
const std::string lowHexDigits = "0123456789abcdef";
const std::string uppHexDigits = "0123456789ABCDEF";
const std::string allHexDigits = "0123456789abcdefABCDEF";
const std::string base64 =       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const std::string base58 =       "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
const std::string base36 =       "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string romanNumerals ="IVXLCDM";
const std::string literals =     "{([\"'/\\";
const std::string horLiterals =  "{(\"/";
const std::string pipes =        "-|+*><%!x";
const std::string operators =    "WwCcLlUuAaSsMmDdRrFfPpZz  NnEeGgKkYy  VvXTtHh";

bool contain(const std::string &s, char ch) {
    return s.find(ch) != std::string::npos;
}

char escapeCharacter(char ch) {
    switch (ch) {
        case 'a': return '\a';
        case 'b': return '\b';
        case 'f': return '\f';
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        case 'v': return '\v';
    }

    return ch;
}

Object getConstant(const std::string &s) {
    Object object;
    object.type = Object::Type::Data;
    object.data.type = Data::Type::String;

    if (s == "ll")  {object.data.string = lowerLetters;   return object;}
    if (s == "ul")  {object.data.string = upperLetters;   return object;}
    if (s == "al")  {object.data.string = allLetters;     return object;}
    if (s == "aa") {
        for (unsigned char c = 0;  c <= 127; ++c)
            object.data.string += c;
        return object;
    }
    if (s == "pa") {
        for (unsigned char c = 32; c <= 126; ++c)
            object.data.string += c;
        return object;
    }
    if (s == "dd")  {object.data.string = digits;         return object;}
    if (s == "od")  {object.data.string = octalDigits;    return object;}
    if (s == "lhd") {object.data.string = lowHexDigits;   return object;}
    if (s == "uhd") {object.data.string = uppHexDigits;   return object;}
    if (s == "ahd") {object.data.string = allHexDigits;   return object;}
    if (s == "b64") {object.data.string = base64;         return object;}
    if (s == "b58") {object.data.string = base58;         return object;}
    if (s == "b36") {object.data.string = base36;         return object;}
    if (s == "rn")  {object.data.string = romanNumerals;  return object;}

    object.data.type = Data::Type::Number;

    if (s == "min") {object.data.number = std::numeric_limits<Number>::min(); return object;}
    if (s == "max") {object.data.number = std::numeric_limits<Number>::max(); return object;}

    bool allDigits = true;
    Number n = 0;
    for (char ch : s) {
        if (contain(digits, ch)) {
            n = n * 10 + (ch - '0');
        } else {
            allDigits = false;
            break;
        }
    }

    if (allDigits) {
        n %= 128;
        object.data.type = Data::Type::String;
        object.data.string += static_cast<char>(n);
        return object;
    }

    return Object();
}

bool sendObject(Grid &, const Object &, std::size_t, std::size_t, util::Dir);

bool placeObject(Grid &grid, const Object &obj, std::size_t x, std::size_t y, util::Dir dir, bool force = false) {
    Object::Type type = grid[y][x].type;

    if (obj.type == Object::Type::Empty) {
        return true;
    }

    if (force || (type == Object::Type::Empty)) {
        grid[y][x] = obj;
        return true;
    } else if ((type == Object::Type::Pipe) || (type == Object::Type::SpecialOperator)) {
        return sendObject(grid, obj, x, y, dir);
    }

    return false;
}

bool sendObject(Grid &grid, const Object &obj, std::size_t x, std::size_t y, util::Dir dir) {
    Object::Type type = grid[y][x].type;

    if (type == Object::Type::SpecialOperator) {
        Result result = grid[y][x].op.processFromPipe(obj);
        if (result.success) {
            return placeObject(grid, result.down, x, y + 1, util::Dir::Down);
        } else {
            return false;
        }
    } else if (type == Object::Type::Pipe) {
        switch (grid[y][x].pipe.type) {
            case Pipe::Type::Horizontal:
                if (dir == util::Dir::Left) {
                    return sendObject(grid, obj, x - 1, y, dir);
                } else {
                    dir = util::Dir::Right;
                    return sendObject(grid, obj, x + 1, y, dir);
                }
            case Pipe::Type::Vertical:
                if (dir == util::Dir::Up) {
                    return sendObject(grid, obj, x, y - 1, dir);
                } else {
                    dir = util::Dir::Down;
                    return sendObject(grid, obj, x, y + 1, dir);
                }
            case Pipe::Type::Intersection:
                if (dir == util::Dir::Up)
                    return sendObject(grid, obj, x, y - 1, dir);
                else if (dir == util::Dir::Left)
                    return sendObject(grid, obj, x - 1, y, dir);
                else if (dir == util::Dir::Right)
                    return sendObject(grid, obj, x + 1, y, dir);
                else
                    return sendObject(grid, obj, x, y + 1, dir);
            case Pipe::Type::Bouncer:
                dir = util::invert(dir);
                if (dir == util::Dir::Up)
                    return sendObject(grid, obj, x, y - 1, dir);
                else if (dir == util::Dir::Left)
                    return sendObject(grid, obj, x - 1, y, dir);
                else if (dir == util::Dir::Right)
                    return sendObject(grid, obj, x + 1, y, dir);
                else
                    return sendObject(grid, obj, x, y + 1, dir);
            case Pipe::Type::TurnR:
                if (dir == util::Dir::Up)
                    return sendObject(grid, obj, x + 1, y, util::Dir::Right);
                else if (dir == util::Dir::Left)
                    return sendObject(grid, obj, x, y - 1, util::Dir::Up);
                else if (dir == util::Dir::Right)
                    return sendObject(grid, obj, x, y + 1, util::Dir::Down);
                else
                    return sendObject(grid, obj, x - 1, y, util::Dir::Left);
            case Pipe::Type::TurnL:
                if (dir == util::Dir::Up)
                    return sendObject(grid, obj, x - 1, y, util::Dir::Left);
                else if (dir == util::Dir::Left)
                    return sendObject(grid, obj, x, y + 1, util::Dir::Down);
                else if (dir == util::Dir::Right)
                    return sendObject(grid, obj, x, y - 1, util::Dir::Up);
                else
                    return sendObject(grid, obj, x + 1, y, util::Dir::Right);
            case Pipe::Type::Duplicator:
                if ((dir == util::Dir::Up) || (dir == util::Dir::Down)) {
                    bool success = sendObject(grid, obj, x + 1, y, util::Dir::Right);
                    success = sendObject(grid, obj, x - 1, y, util::Dir::Left) || success;
                    return success;
                } else {
                    bool success = sendObject(grid, obj, x, y - 1, util::Dir::Up);
                    success = sendObject(grid, obj, x, y + 1, util::Dir::Down) || success;
                    return success;
                }
            case Pipe::Type::Teleport: {
                bool found = false;
                while (true) {
                    if (dir == util::Dir::Up)
                        --y;
                    else if (dir == util::Dir::Left)
                        --x;
                    else if (dir == util::Dir::Right)
                        ++x;
                    else
                        ++y;

                    if (found) {
                        return sendObject(grid, obj, x, y, dir);
                    }

                    if ((y == 0) || (x == 0) || (y == grid.size() - 1) ||
                        (x == grid[y].size() - 1)) {
                            return false;
                    } else if (grid[y][x].type == Object::Type::Pipe){
                        if (grid[y][x].pipe.type == Pipe::Type::Teleport) {
                            found = true;
                        }
                    }
                }
            }
            case Pipe::Type::Eraser:
                return true;
        }
    }

    return false;
}

}



Interpreter::Interpreter(bool debugMode, TestManager::Test *test):
tick        (0),
ended       (false),
printAll    (false),
debugMode   (debugMode),
testMode    (test != nullptr),
test        (test),
randomEngine(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {
    Operator::interpreter = this;
}

std::string Interpreter::getInputString() {
    std::string result;
    std::cin >> result;
    return result;
}

Number Interpreter::getInputNumber() {
    Number result;
    std::string tmp;
    bool exit = false;
    do{
        std::getline(std::cin, tmp);
        std::stringstream ss(tmp);
        exit = ss >> result;
    } while (!exit);

    return result;
}

void Interpreter::output(const std::string &s) {
    std::cout << s;
}

void Interpreter::output(Number n) {
    std::cout << toString(n);
}

void Interpreter::halt(bool print) {
    ended = true;
    printAll = print;
}

Number Interpreter::rand(Number n){
    if (n > 0) {
        return std::uniform_int_distribution<Number>{0, n - 1}(randomEngine);
    } else if (n < 0) {
        return -std::uniform_int_distribution<Number>{0, -n - 1}(randomEngine);
    } else {
        Number mult = std::uniform_int_distribution<Number>{0, 1}(randomEngine) ? 1: -1;
        return mult * std::uniform_int_distribution<Number>{}(randomEngine);
    }
}

bool Interpreter::load(std::stringstream &stream) {
    gridSource.push_back(std::string());

	std::string line;
	std::size_t longer = 0;
	while (std::getline(stream, line)) {
        gridSource.push_back(std::string("#" + line));
        if (gridSource.back().size() > longer)
            longer = gridSource.back().size();
	}

	gridSource[0] = std::string(longer + 1, '#');

	for (std::size_t i = 1; i < gridSource.size(); ++i) {
        gridSource[i].resize(longer, ' ');
        gridSource[i].push_back('#');
	}

	gridSource.push_back(std::string(longer + 1, '#'));

    return parse();
}

bool Interpreter::execute() {
    while (!ended) {
        if (debugMode)
            printDebug();

        firstStage();

        if (testMode) {
            if (test->tick == tick) {
                if (test->type == grid[test->y][test->x].data.type) {
                    switch (test->type) {
                        case Data::Type::Number:
                            test->success = test->number == grid[test->y][test->x].data.number;
                            break;
                        case Data::Type::String:
                            test->success = test->string == grid[test->y][test->x].data.string;
                            break;
                        default:
                            test->success = true;
                            break;
                    }
                }
                ended = true;
            }
        }

        if (ended) {
            if (printAll)
                printAllObject();
            break;
        }

        secondStage();
        ++tick;
    }

    return true;
}

bool Interpreter::parse() {
    for (std::size_t y = 0; y < gridSource.size(); ++y) {
        std::string &line = gridSource[y];
        grid.push_back(std::vector<Object>());

        for (std::size_t x = 0; x < line.size(); ++x) {
            char ch = line[x];
            Object object;

            if (ch == ' ') {
                object.type = Object::Type::Empty;
            } else if (ch == '#') {
                object.type = Object::Type::Block;
            } else if (ch == 'I') {
                object.type = Object::Type::Data;
                object.data.type = Data::Type::Input;
            } else if (ch == 'O') {
                object.type = Object::Type::Data;
                object.data.type = Data::Type::Output;
            } else if (contain(literals, ch)) {
                object = parseLiteral(ch, x, y);
            } else if (contain(digits, ch)) {
                object.type = Object::Type::Data;
                object.data.type = Data::Type::Number;
                object.data.number = ch - '0';
            } else if (contain(pipes, ch)) {
                object = Object::CreatePipe(ch);
            } else if (contain(operators, ch)) {
                object = Object::CreateOperator(ch);
            } else {
                std::cout << "ERROR: character not valid (" << x << ", " << y << ")\n";
                return false;
            }
            if (ended) {
                return false;
            }
            grid.back().push_back(object);
        }
    }

    return true;
}

Object Interpreter::parseLiteral(char ch, std::size_t startX, std::size_t startY) {
    bool horizontal = contain(horLiterals, ch);

    enum class Type {Comment, Number, String, Constant} type;
    char chEnd;

    switch (ch) {
        case '{':   type = Type::Comment;   chEnd = '}';  break;
        case '(':   type = Type::Number;    chEnd = ')';  break;
        case '[':   type = Type::Number;    chEnd = ']';  break;
        case '"':   type = Type::String;    chEnd = '"';  break;
        case '\'':  type = Type::String;    chEnd = '\''; break;
        case '/':   type = Type::Constant;  chEnd = '/';  break;
        case '\\':  type = Type::Constant;  chEnd = '\\'; break;
    }

    Number theNumber = 0;
    std::string theString;

    bool first = true;
    bool negative = false;
    bool escaped = false;
    bool end = false;
    std::size_t x = startX;
    std::size_t y = startY;
    while (!end) {
        if (horizontal) {
            ++x;
        } else {
            ++y;
        }

        if ((y == gridSource.size() - 1) || (x == gridSource[y].size() - 1)) {
            std::cout << "ERROR: unclosed literal (" << startX << ", " << startY << ")\n";
            end = true;
            ended = true;
            continue;
        }

        char ch = gridSource[y][x];
        gridSource[y][x] = ' ';

        if (!escaped && (ch == chEnd)) {
            end = true;
            continue;
        }

        switch (type) {
            case Type::Number:
                if (contain(digits, ch)) {
                    theNumber = theNumber * 10 + (ch - '0');
                } else if (first && (ch == '-')) {
                    negative = true;
                } else {
                    end = true;
                    continue;
                }
                break;
            case Type::String:
                if (escaped) {
                    theString += escapeCharacter(ch);
                    escaped = false;
                } else {
                    if (ch == '\\') {
                        escaped = true;
                    } else {
                        theString += ch;
                    }
                }
                break;
            case Type::Constant:
                theString += ch;
                break;
            default:
                break;
        }

        first = false;
    }

    Object object;

    switch (type) {
        case Type::Comment:
            object.type = Object::Type::Empty;
            break;
        case Type::Number:
            if (negative)
                theNumber *= -1;
            object.type = Object::Type::Data;
            object.data.type = Data::Type::Number;
            object.data.number = theNumber;
            break;
        case Type::String:
            object.type = Object::Type::Data;
            object.data.type = Data::Type::String;
            object.data.string = theString;
            break;
        case Type::Constant:
            object = getConstant(theString);
            break;
    }

    return object;
}

void Interpreter::firstStage() {
    using PersistGrid = std::vector<std::vector<bool>>;
    PersistGrid persistGrid(grid.size(), std::vector<bool>(grid.back().size(), true));

    Grid newGrid = grid;

    auto conformsType = [](const Object &obj, Data::Type type) -> bool {
        if (type == Data::Type::None)
            return true;
        if (obj.type != Object::Type::Data)
            return false;
        if (check(type, obj.data.type))
            return true;

        return false;
    };

    auto RemovePersistentFlag = [&](Data::Type type, std::size_t x, std::size_t y) {
        if (type != Data::Type::None) {
            persistGrid[y][x] = false;
        }
    };

    auto allNone = [](Operator::DataOutcome &out) -> bool {
        return (out.up == Data::Type::None) && (out.left == Data::Type::None) &&
               (out.right == Data::Type::None) && (out.down == Data::Type::None);
    };

    for (std::size_t y = 1; y < grid.size() - 1; ++y) {
        auto &line = grid[y];
        for (std::size_t x = 1; x < line.size() - 1; ++x) {
            Object &object = line[x];
            if (check(object.type, Object::Type::Operator)) {
                bool verticalAlreadyChecked = false;
                bool horizontalAlreadyChecked = false;
                for (std::size_t n = 0; n < object.op.io.size(); ++n) {
                    Operator::DataRequirements &req = object.op.io[n].req;
                    Operator::DataOutcome &out = object.op.io[n].out;
                    Object &objU = grid[y - 1][x];
                    Object &objL = grid[y][x - 1];
                    Object &objR = grid[y][x + 1];
                    Object &objD = grid[y + 1][x];

                    bool conforms;
                    bool vertical = false;
                    bool horizontal = false;
                    if (object.op.separateAxes) {
                        if (!verticalAlreadyChecked)
                            vertical   = conformsType(objU, req.up) &&
                                         conformsType(objD, req.down);
                        if (!horizontalAlreadyChecked)
                            horizontal = conformsType(objL, req.up) &&
                                         conformsType(objR, req.down);
                        conforms = vertical || horizontal;
                    } else {
                        conforms = conformsType(objU, req.up) && conformsType(objL, req.left) &&
                            conformsType(objD, req.down) && conformsType(objR, req.right);
                    }

                    if (!conforms)
                        continue;

                    bool placed = false;
                    bool placedV = false;
                    bool placedH = false;

                    if (object.op.separateAxes) {
                        if (vertical) {
                            Result result = object.op.processAxis(n, objU, objD);
                            if (result.success) {
                                if (allNone(out)) {
                                    placedV = true;
                                } else {
                                    if ((out.up != Data::Type::None) && placeObject(newGrid, result.first, x, y - 1, util::Dir::Up, object.op.replace))
                                            placedV = true;
                                    if ((out.down != Data::Type::None) && placeObject(newGrid, result.second, x, y + 1, util::Dir::Down, object.op.replace) )
                                            placedV = true;
                                }
                            }
                        }
                        if (horizontal) {
                            Result result = object.op.processAxis(n, objL, objR);
                            if (result.success) {
                                if (allNone(out)) {
                                    placedH = true;
                                } else {
                                    if ((out.up != Data::Type::None) && placeObject(newGrid, result.first, x - 1, y, util::Dir::Left, object.op.replace))
                                            placedH = true;
                                    if ((out.down != Data::Type::None) && placeObject(newGrid, result.second, x + 1, y, util::Dir::Right, object.op.replace))
                                            placedH = true;
                                }
                            }
                        }
                    } else {
                        Result result = object.op.processAll(n, objU, objL, objR, objD);
                        if (result.success) {
                            if (allNone(out)) {
                                placed = true;
                            } else {
                                if ((out.up != Data::Type::None) && placeObject(newGrid, result.up, x, y - 1, util::Dir::Up, object.op.replace))
                                        placed = true;
                                if ((out.left != Data::Type::None) && placeObject(newGrid, result.left, x - 1, y, util::Dir::Left, object.op.replace))
                                        placed = true;
                                if ((out.right != Data::Type::None) && placeObject(newGrid, result.right, x + 1, y, util::Dir::Right, object.op.replace))
                                        placed = true;
                                if ((out.down != Data::Type::None) && placeObject(newGrid, result.down, x, y + 1, util::Dir::Down, object.op.replace))
                                        placed = true;
                            }
                        }
                    }

                    if (!object.op.noRemove) {
                        if (object.op.separateAxes) {
                            if (placedV) {
                                RemovePersistentFlag(req.up,    x,     y - 1);
                                RemovePersistentFlag(req.down,  x,     y + 1);
                            }
                            if (placedH) {
                                RemovePersistentFlag(req.up,    x - 1, y);
                                RemovePersistentFlag(req.down,  x + 1, y);
                            }
                        } else {
                            if (placed) {
                                RemovePersistentFlag(req.up,    x,     y - 1);
                                RemovePersistentFlag(req.down,  x,     y + 1);
                                RemovePersistentFlag(req.left,  x - 1, y);
                                RemovePersistentFlag(req.right, x + 1, y);
                            }
                        }
                    }

                    if (object.op.separateAxes) {
                        if (vertical)
                            verticalAlreadyChecked = true;
                        if (horizontal)
                            horizontalAlreadyChecked = true;
                        if (verticalAlreadyChecked && horizontalAlreadyChecked) {
                            break;
                        } else {
                            continue;
                        }
                    } else {
                        break;
                    }
                }
            }
        }
    }

    for (std::size_t y = 1; y < newGrid.size() - 1; ++y) {
        auto &line = newGrid[y];
        for (std::size_t x = 1; x < line.size() - 1; ++x) {
            Object &object = line[x];
            if (!persistGrid[y][x]) {
                object.clear();
            }
        }
    }

    grid = std::move(newGrid);
}

void Interpreter::secondStage() {
    struct State {
        bool fixed;
        bool empty;
        util::Dir direction;
    };

    std::vector<std::vector<State>> stateGrid(grid.size(), std::vector<State>(grid.back().size()));

    bool existData = false;

    for (std::size_t y = 0; y < grid.size(); ++y) {
        auto &line = grid[y];
        for (std::size_t x = 0; x < line.size(); ++x) {
            Object &object = line[x];
            State &state = stateGrid[y][x];
            if (object.type == Object::Type::Data) {
                existData = true;
                state.fixed = false;
                state.empty = false;
                state.direction = util::Dir::Down;
            } else if (object.type == Object::Type::Empty) {
                state.fixed = false;
                state.empty = true;
            } else {
                state.fixed = true;
                state.empty = false;
            }
        }
    }

    if (!existData) {
        ended = true;
        return;
    }

    auto moveObject = [&](std::size_t fromX, std::size_t fromY, std::size_t toX, std::size_t toY) {
        grid[toY][toX] = grid[fromY][fromX];
        stateGrid[toY][toX].fixed = true;
        stateGrid[toY][toX].empty = false;
        grid[fromY][fromX].clear();
        stateGrid[fromY][fromX].fixed = false;
        stateGrid[fromY][fromX].empty = true;
    };

    bool toMove = true;
    while (toMove) {
        start:
        toMove = false;

        for (std::size_t y = 1; y < grid.size() - 1; ++y) {
            auto &line = grid[y];
            for (std::size_t x = 1; x < line.size() - 1; ++x) {
                Object &object = line[x];
                State &state = stateGrid[y][x];

                if (state.fixed || state.empty)
                    continue;

                toMove = true;

                std::size_t newX, newY;
                switch (state.direction) {
                    case util::Dir::Up:     newX = x; newY = y - 1; break;
                    case util::Dir::Down:   newX = x; newY = y + 1; break;
                    case util::Dir::Left:   newX = x - 1; newY = y; break;
                    case util::Dir::Right:  newX = x + 1; newY = y; break;
                }

                State &newState = stateGrid[newY][newX];
                if (newState.empty) {
                    moveObject(x, y, newX, newY);
                    goto start;
                } else if (newState.fixed){
                    if (state.direction == util::Dir::Down)
                        state.direction = object.data.direction;
                    else
                        state.fixed = true;
                } else if (state.direction == util::invert(newState.direction)) {
                    state.fixed = true;
                }
            }
        }
    }
}

void Interpreter::printAllObject() {
    for (std::size_t y = 1; y < grid.size() - 1; ++y) {
        auto &line = grid[y];
        for (std::size_t x = 1; x < line.size() - 1; ++x) {
            Object &object = line[x];
            if (object.type == Object::Type::Data) {
                if (object.data.type == Data::Type::Number) {
                    output(object.data.number);
                } else if (object.data.type == Data::Type::String) {
                    output(object.data.string);
                } else {
                    continue;
                }
                std::cout << ' ';
            }
        }
    }
}

void Interpreter::printDebug() {
    for (std::size_t y = 0; y < grid.size(); ++y) {
        auto &line = grid[y];
        for (std::size_t x = 0; x < line.size(); ++x) {
            Object object = line[x];
            std::cout << object;
        }
        std::cout << "\n";
    }

    std::cout << "\n\n";

    for (std::size_t y = 0; y < grid.size(); ++y) {
        auto &line = grid[y];
        for (std::size_t x = 0; x < line.size(); ++x) {
            Object object = line[x];
            if (object.type == Object::Type::Data) {
                if (object.data.type == Data::Type::Number) {
                    std::cout << "(" << x << "," << y << ") " << object.data.number << "\n";
                } else if (object.data.type == Data::Type::String) {
                    std::cout << "(" << x << "," << y << ") \"" << object.data.string << "\"\n";
                }
            }
        }
    }

    std::cin.clear();
    while (std::cin.get() != '\n') {}
}
