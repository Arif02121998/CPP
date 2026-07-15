/*******************************************************************************
 * 01_vector.cpp — std::vector: The Most Used Container
 *******************************************************************************
 *
 *  vector = Dynamic array that grows automatically
 *
 *  Memory layout:
 *  ┌─────────────────────────────────────────────────────┐
 *  │ vector<int> v = {10, 20, 30}                        │
 *  │                                                      │
 *  │ Stack:              Heap:                            │
 *  │ ┌──────────────┐   ┌────┬────┬────┬────┬────┐      │
 *  │ │ data ptr ────┼──►│ 10 │ 20 │ 30 │    │    │      │
 *  │ │ size: 3      │   └────┴────┴────┴────┴────┘      │
 *  │ │ capacity: 5  │   ◄── used ──►◄─ reserved ─►      │
 *  │ └──────────────┘                                     │
 *  │                                                      │
 *  │ When size == capacity → reallocate (usually 2x)     │
 *  └─────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o vector 01_vector.cpp
 ******************************************************************************/

#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    // ─── CREATION ───
    std::vector<int> v1;                     // Empty
    std::vector<int> v2 = {10, 20, 30};     // Initializer list
    std::vector<int> v3(5, 99);             // 5 elements, all 99
    std::vector<int> v4(v2);                // Copy of v2
    std::vector<int> v5(v2.begin(), v2.end()); // From iterators

    // ─── ADDING ELEMENTS ───
    std::cout << "=== ADDING ===\n";
    v1.push_back(10);              // Add to end: O(1) amortized
    v1.push_back(20);
    v1.emplace_back(30);           // Construct in-place (avoids copy!)
    v1.insert(v1.begin() + 1, 15); // Insert at index 1: O(n)

    for (int x : v1) std::cout << x << " ";
    std::cout << '\n';

    // ─── ACCESS ───
    std::cout << "\n=== ACCESS ===\n";
    std::cout << "v2[0] = " << v2[0] << '\n';      // No bounds check
    std::cout << "v2.at(1) = " << v2.at(1) << '\n'; // Throws if out of bounds
    std::cout << "front: " << v2.front() << '\n';
    std::cout << "back: " << v2.back() << '\n';

    // ─── SIZE & CAPACITY ───
    std::cout << "\n=== SIZE & CAPACITY ===\n";
    std::cout << "Size: " << v2.size() << '\n';
    std::cout << "Capacity: " << v2.capacity() << '\n';
    std::cout << "Empty? " << v2.empty() << '\n';

    v2.reserve(100);  // Pre-allocate to avoid reallocation
    std::cout << "After reserve(100): capacity = " << v2.capacity() << '\n';

    v2.shrink_to_fit();  // Release unused memory
    std::cout << "After shrink_to_fit: capacity = " << v2.capacity() << '\n';

    // ─── REMOVING ───
    std::cout << "\n=== REMOVING ===\n";
    std::vector<int> v = {1, 2, 3, 4, 5};
    v.pop_back();                          // Remove last: O(1)
    v.erase(v.begin() + 1);               // Remove at index: O(n)
    v.erase(v.begin(), v.begin() + 2);    // Remove range

    for (int x : v) std::cout << x << " ";
    std::cout << '\n';

    // ─── ITERATION ───
    std::cout << "\n=== ITERATION ===\n";
    std::vector<int> nums = {50, 10, 40, 20, 30};

    // Range-based for (preferred!)
    std::cout << "Range-for: ";
    for (const auto& n : nums) std::cout << n << " ";
    std::cout << '\n';

    // ─── ALGORITHMS ───
    std::sort(nums.begin(), nums.end());  // Sort ascending
    std::cout << "Sorted: ";
    for (int n : nums) std::cout << n << " ";
    std::cout << '\n';

    auto it = std::find(nums.begin(), nums.end(), 30);
    if (it != nums.end())
        std::cout << "Found 30 at index: " << (it - nums.begin()) << '\n';

    // ─── 2D VECTOR ───
    std::cout << "\n=== 2D VECTOR ===\n";
    std::vector<std::vector<int>> matrix = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    for (const auto& row : matrix) {
        for (int val : row) std::cout << val << " ";
        std::cout << '\n';
    }

    return 0;
}
