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

#include "Object.h"
#include "TestManager.h"

#include <map>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

typedef std::vector<std::vector<Object>> Grid;
typedef std::vector<std::string> GridSource;

class Interpreter {
public:
    Interpreter(bool debugMode, TestManager::Test *test = nullptr);

    std::string getInputString();
    Number getInputNumber();
    void output(const std::string &s);
    void output(Number n);
    void halt(bool print = false);
    Number rand(Number n = 0);

    bool load(std::stringstream &stream);
    bool execute();

    Number tick;
private:
    bool parse();
    Object parseLiteral(char ch, std::size_t x, std::size_t y);

    void firstStage();
    void secondStage();
    void printAllObject();

    void printDebug();

    Grid grid;
    GridSource gridSource;
    bool ended;
    bool printAll;
    bool debugMode;
    bool testMode;
    TestManager::Test *test;
    std::mt19937_64 randomEngine;
};
