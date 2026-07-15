/*******************************************************************************
 * 03_iterators.cpp — Iterators: The Bridge Between Containers and Algorithms
 *******************************************************************************
 *
 *  Iterator = Generalized pointer that traverses a container
 *
 *  ┌────────────────────────────────────────────────────────────┐
 *  │  Iterator Categories (weakest → strongest):                │
 *  │                                                            │
 *  │  Input ──► Forward ──► Bidirectional ──► Random Access     │
 *  │  (read)   (multi-pass) (also --)       (also +n, [], <)  │
 *  │                                                            │
 *  │  Container     │ Iterator Type                             │
 *  │  ──────────────┼──────────────────                        │
 *  │  vector, deque │ Random Access                             │
 *  │  list          │ Bidirectional                             │
 *  │  forward_list  │ Forward                                   │
 *  │  map, set      │ Bidirectional                             │
 *  │  unordered_*   │ Forward                                   │
 *  └────────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o iterators 03_iterators.cpp
 ******************************************************************************/

#include <iostream>
#include <vector>
#include <list>
#include <map>

int main() {
    std::vector<int> vec = {10, 20, 30, 40, 50};

    // ─── BASIC ITERATION ───
    std::cout << "=== ITERATOR TYPES ===\n";

    // Forward iteration
    std::cout << "Forward:  ";
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << '\n';

    // Reverse iteration
    std::cout << "Reverse:  ";
    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << '\n';

    // Const iteration (read-only)
    std::cout << "Const:    ";
    for (auto it = vec.cbegin(); it != vec.cend(); ++it) {
        // *it = 99;  // ERROR! const iterator
        std::cout << *it << " ";
    }
    std::cout << '\n';

    // ─── RANDOM ACCESS ───
    std::cout << "\n=== RANDOM ACCESS (vector) ===\n";
    auto it = vec.begin();
    std::cout << "*(begin + 2) = " << *(it + 2) << '\n';  // 30
    std::cout << "Distance: " << std::distance(vec.begin(), vec.end()) << '\n';

    // ─── MODIFY THROUGH ITERATOR ───
    std::cout << "\n=== MODIFY ===\n";
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        *it *= 2;
    }
    for (int v : vec) std::cout << v << " ";
    std::cout << '\n';

    // ─── MAP ITERATORS ───
    std::cout << "\n=== MAP ITERATOR ===\n";
    std::map<std::string, int> m = {{"a", 1}, {"b", 2}, {"c", 3}};
    for (auto it = m.begin(); it != m.end(); ++it) {
        std::cout << it->first << ":" << it->second << " ";
    }
    std::cout << '\n';

    return 0;
}
