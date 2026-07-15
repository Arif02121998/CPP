/*******************************************************************************
 * 04_algorithms.cpp — STL Algorithms: sort, find, transform, and more
 *******************************************************************************
 *
 *  STL algorithms work on ITERATORS, not containers directly.
 *  This means they work with ANY container!
 *
 *  Pattern: algorithm(begin, end, ...)
 *
 * COMPILE: g++ -std=c++17 -o algorithms 04_algorithms.cpp
 ******************************************************************************/

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>    // accumulate, iota
#include <functional> // greater, less

void print(const std::vector<int>& v, const std::string& label = "") {
    if (!label.empty()) std::cout << label << ": ";
    for (int x : v) std::cout << x << " ";
    std::cout << '\n';
}

int main() {
    std::vector<int> v = {5, 3, 8, 1, 9, 2, 7, 4, 6};

    // ─── SORTING ───
    std::cout << "=== SORTING ===\n";
    print(v, "Original");

    std::sort(v.begin(), v.end());                          // Ascending
    print(v, "Sorted");

    std::sort(v.begin(), v.end(), std::greater<int>());     // Descending
    print(v, "Descending");

    std::sort(v.begin(), v.end(), [](int a, int b) {        // Custom
        return (a % 2) < (b % 2);  // Even numbers first
    });
    print(v, "Evens first");

    // ─── SEARCHING ───
    std::cout << "\n=== SEARCHING ===\n";
    std::sort(v.begin(), v.end());

    auto it = std::find(v.begin(), v.end(), 5);
    if (it != v.end())
        std::cout << "Found 5 at index " << std::distance(v.begin(), it) << '\n';

    auto it2 = std::find_if(v.begin(), v.end(), [](int x) { return x > 6; });
    if (it2 != v.end())
        std::cout << "First > 6: " << *it2 << '\n';

    bool has7 = std::binary_search(v.begin(), v.end(), 7);  // O(log n), requires sorted
    std::cout << "Contains 7? " << (has7 ? "yes" : "no") << '\n';

    int cnt = std::count_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    std::cout << "Even count: " << cnt << '\n';

    // ─── TRANSFORM ───
    std::cout << "\n=== TRANSFORM ===\n";
    std::vector<int> doubled(v.size());
    std::transform(v.begin(), v.end(), doubled.begin(), [](int x) { return x * 2; });
    print(doubled, "Doubled");

    // ─── ACCUMULATE ───
    std::cout << "\n=== ACCUMULATE ===\n";
    int sum = std::accumulate(v.begin(), v.end(), 0);
    std::cout << "Sum: " << sum << '\n';

    int product = std::accumulate(v.begin(), v.end(), 1, std::multiplies<int>());
    std::cout << "Product: " << product << '\n';

    // ─── MIN / MAX ───
    std::cout << "\n=== MIN / MAX ===\n";
    auto [minIt, maxIt] = std::minmax_element(v.begin(), v.end());
    std::cout << "Min: " << *minIt << ", Max: " << *maxIt << '\n';

    // ─── REMOVE / ERASE (Erase-Remove idiom) ───
    std::cout << "\n=== ERASE-REMOVE ===\n";
    std::vector<int> data = {1, 2, 3, 2, 4, 2, 5};
    data.erase(std::remove(data.begin(), data.end(), 2), data.end());
    print(data, "After removing 2s");

    // ─── UNIQUE ───
    std::cout << "\n=== UNIQUE ===\n";
    std::vector<int> dups = {1, 1, 2, 3, 3, 3, 4, 5, 5};
    dups.erase(std::unique(dups.begin(), dups.end()), dups.end());
    print(dups, "Unique");

    // ─── FOR_EACH ───
    std::cout << "\n=== FOR_EACH ===\n";
    std::cout << "Squares: ";
    std::for_each(v.begin(), v.end(), [](int x) {
        std::cout << x * x << " ";
    });
    std::cout << '\n';

    // ─── ALL_OF / ANY_OF / NONE_OF ───
    std::cout << "\n=== PREDICATES ===\n";
    std::cout << "All positive? " << std::all_of(v.begin(), v.end(), [](int x) { return x > 0; }) << '\n';
    std::cout << "Any even? " << std::any_of(v.begin(), v.end(), [](int x) { return x % 2 == 0; }) << '\n';

    // ─── IOTA (fill with incrementing values) ───
    std::cout << "\n=== IOTA ===\n";
    std::vector<int> seq(10);
    std::iota(seq.begin(), seq.end(), 1);
    print(seq, "iota(1)");

    return 0;
}
