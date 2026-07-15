/*******************************************************************************
 * 02_map_set.cpp — Associative Containers: map, set, unordered variants
 *******************************************************************************
 *
 *  map: Key → Value pairs (sorted by key, uses red-black tree)
 *  set: Unique values (sorted, uses red-black tree)
 *  unordered_map/set: Hash table (O(1) average, unsorted)
 *
 *  ┌──────────────────┬──────────────────┐
 *  │   std::map       │ unordered_map    │
 *  │  (red-black tree)│ (hash table)     │
 *  │   O(log n)       │ O(1) average     │
 *  │   Sorted keys    │ Unsorted         │
 *  └──────────────────┴──────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o map_set 02_map_set.cpp
 ******************************************************************************/

#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <string>

int main() {

    // ═══════════════ MAP ═══════════════
    std::cout << "=== std::map ===\n";
    std::map<std::string, int> ages;

    // Insert
    ages["Alice"] = 30;
    ages["Bob"] = 25;
    ages.insert({"Charlie", 35});
    ages.emplace("Diana", 28);

    // Access
    std::cout << "Alice: " << ages["Alice"] << '\n';
    std::cout << "Bob:   " << ages.at("Bob") << '\n';

    // Check existence
    if (ages.count("Eve") == 0) {
        std::cout << "Eve not found\n";
    }

    if (auto it = ages.find("Charlie"); it != ages.end()) {
        std::cout << "Charlie: " << it->second << '\n';
    }

    // Iterate (sorted by key!)
    std::cout << "All (sorted):\n";
    for (const auto& [name, age] : ages) {  // Structured bindings (C++17)
        std::cout << "  " << name << ": " << age << '\n';
    }

    // Erase
    ages.erase("Bob");
    std::cout << "Size after erase: " << ages.size() << '\n';

    // ═══════════════ UNORDERED_MAP ═══════════════
    std::cout << "\n=== std::unordered_map ===\n";
    std::unordered_map<std::string, double> prices = {
        {"apple", 1.50}, {"banana", 0.75}, {"cherry", 2.00}
    };

    for (const auto& [item, price] : prices) {
        std::cout << "  " << item << ": $" << price << '\n';
    }

    // ═══════════════ SET ═══════════════
    std::cout << "\n=== std::set ===\n";
    std::set<int> numbers = {5, 3, 8, 1, 3, 5};  // Duplicates removed!

    numbers.insert(10);
    numbers.insert(3);  // Already exists — ignored

    std::cout << "Set (sorted, unique): ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << '\n';

    std::cout << "Contains 8? " << (numbers.count(8) ? "yes" : "no") << '\n';

    // ═══════════════ UNORDERED_SET ═══════════════
    std::cout << "\n=== std::unordered_set ===\n";
    std::unordered_set<std::string> words = {"hello", "world", "hello"};
    words.insert("foo");

    std::cout << "Words: ";
    for (const auto& w : words) std::cout << w << " ";
    std::cout << '\n';
    std::cout << "Size: " << words.size() << '\n';

    // ═══════════════ MULTIMAP ═══════════════
    std::cout << "\n=== std::multimap (duplicate keys) ===\n";
    std::multimap<std::string, int> scores;
    scores.insert({"Alice", 90});
    scores.insert({"Alice", 85});  // Duplicate key allowed!
    scores.insert({"Bob", 92});

    for (const auto& [name, score] : scores) {
        std::cout << "  " << name << ": " << score << '\n';
    }

    return 0;
}
