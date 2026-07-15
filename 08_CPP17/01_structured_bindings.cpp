/*******************************************************************************
 * 01_structured_bindings.cpp — Auto-decompose pairs, tuples, structs
 *******************************************************************************
 *
 *  auto [x, y, z] = some_struct;   // Decompose into named variables!
 *
 *  Before C++17:                    C++17:
 *  auto p = getPoint();             auto [x, y] = getPoint();
 *  int x = p.first;
 *  int y = p.second;
 *
 * COMPILE: g++ -std=c++17 -o struct_bind 01_structured_bindings.cpp
 ******************************************************************************/

#include <iostream>
#include <map>
#include <tuple>
#include <string>
#include <vector>

struct Point {
    double x, y, z;
};

std::tuple<int, std::string, double> getStudent() {
    return {42, "Alice", 3.8};
}

int main() {
    // ─── From struct ───
    std::cout << "=== STRUCT ===\n";
    Point p{1.0, 2.0, 3.0};
    auto [x, y, z] = p;
    std::cout << "x=" << x << " y=" << y << " z=" << z << '\n';

    // ─── From tuple ───
    std::cout << "\n=== TUPLE ===\n";
    auto [id, name, gpa] = getStudent();
    std::cout << "ID: " << id << ", Name: " << name << ", GPA: " << gpa << '\n';

    // ─── From pair ───
    std::cout << "\n=== PAIR ===\n";
    std::pair<std::string, int> person{"Bob", 25};
    auto [pname, age] = person;
    std::cout << pname << " is " << age << " years old\n";

    // ─── With map iteration ── (HUGE improvement!)
    std::cout << "\n=== MAP ITERATION ===\n";
    std::map<std::string, int> scores = {{"Alice", 95}, {"Bob", 87}, {"Charlie", 92}};

    // Before C++17:
    // for (auto it = scores.begin(); it != scores.end(); ++it)
    //     std::cout << it->first << ": " << it->second;

    // C++17: Clean!
    for (const auto& [student, score] : scores) {
        std::cout << student << ": " << score << '\n';
    }

    // ─── With insert result ───
    std::cout << "\n=== INSERT RESULT ===\n";
    auto [it, inserted] = scores.insert({"Diana", 88});
    std::cout << "Inserted " << it->first << "? " << (inserted ? "yes" : "no") << '\n';

    auto [it2, inserted2] = scores.insert({"Alice", 100});
    std::cout << "Inserted Alice again? " << (inserted2 ? "yes" : "no") << '\n';

    // ─── If with initializer (C++17) ───
    std::cout << "\n=== IF WITH INIT ===\n";
    if (auto [iter, success] = scores.insert({"Eve", 91}); success) {
        std::cout << "Added " << iter->first << ": " << iter->second << '\n';
    }

    return 0;
}
