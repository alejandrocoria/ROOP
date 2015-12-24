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

#include "TestManager.h"

#include <limits>

TestManager::TestManager(std::ifstream &file) {
    bool eof = false;

    while (!eof) {
        char ch;
        Test *test = new Test();

        file >> test->name;
        file >> test->tick;
        file >> test->x;
        file >> test->y;
        file >> ch;

        switch (ch) {
            case 'e':
                test->type = Data::Type::None;
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            case 'n':
                test->type = Data::Type::Number;
                file >> test->number;
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            case 's':
                test->type = Data::Type::String;
                file.ignore(1);
                std::getline(file, test->string);
                break;
            case 'i':
                test->type = Data::Type::Input;
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            case 'o':
                test->type = Data::Type::Output;
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            default:
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        while (true) {
            file.get(ch);

            if (file.eof())
                eof = true;

            if (eof || (ch == '@')) {
                tests.push_back(std::unique_ptr<Test>(test));
                break;
            } else {
                test->stream << ch;
            }
        }

        test->stream << '\n';
    }
}

std::size_t TestManager::getCount() const {
    return tests.size();
}

TestManager::Test *TestManager::getTest(std::size_t n) {
    return tests[n].get();
}
