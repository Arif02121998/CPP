/*******************************************************************************
 * 06_arrays.cpp — Arrays: 1D, 2D, and C-Style Strings
 *******************************************************************************
 *
 * CONCEPT: Arrays store multiple elements of the same type contiguously.
 *
 *  Memory Layout of int arr[5] = {10, 20, 30, 40, 50}:
 *
 *  Index:    [0]      [1]      [2]      [3]      [4]
 *           ┌────────┬────────┬────────┬────────┬────────┐
 *  Value:   │   10   │   20   │   30   │   40   │   50   │
 *           └────────┴────────┴────────┴────────┴────────┘
 *  Address: 0x100    0x104    0x108    0x10C    0x110
 *                +4       +4       +4       +4
 *                    (each int = 4 bytes)
 *
 *  arr == &arr[0] == 0x100  (array name decays to pointer to first element)
 *
 * COMPILE: g++ -std=c++17 -o arrays 06_arrays.cpp
 ******************************************************************************/

#include <iostream>
#include <cstring>  // For C-string functions

int main() {

    // ═════════════════════════════════════════════════════════════════
    // 1. DECLARING AND INITIALIZING ARRAYS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "=== 1D ARRAYS ===\n";

    int arr1[5];                          // Uninitialized (garbage values!)
    int arr2[5] = {10, 20, 30, 40, 50};  // Fully initialized
    int arr3[5] = {10, 20};              // Partial: {10, 20, 0, 0, 0}
    int arr4[5] = {};                     // All zeros: {0, 0, 0, 0, 0}
    int arr5[] = {1, 2, 3, 4};           // Size deduced: 4 elements

    // Accessing elements (0-indexed!)
    std::cout << "arr2[0] = " << arr2[0] << '\n';  // 10
    std::cout << "arr2[4] = " << arr2[4] << '\n';  // 50

    // Modifying elements
    arr2[2] = 99;
    std::cout << "arr2[2] after modification = " << arr2[2] << '\n';  // 99

    // WARNING: No bounds checking! Accessing arr2[5] is UNDEFINED BEHAVIOR
    // arr2[100] = 42;  // Compiles but VERY DANGEROUS!

    // ═════════════════════════════════════════════════════════════════
    // 2. ARRAY SIZE
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== ARRAY SIZE ===\n";

    int nums[] = {1, 2, 3, 4, 5, 6};

    // Method 1: sizeof trick (only works in same scope as declaration!)
    int size = sizeof(nums) / sizeof(nums[0]);
    std::cout << "Array size: " << size << '\n';

    // Method 2: std::size() (C++17)
    // std::cout << "Array size (C++17): " << std::size(nums) << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 3. ITERATING OVER ARRAYS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== ITERATING ===\n";

    int data[] = {10, 20, 30, 40, 50};

    // Classic for loop
    std::cout << "Classic for: ";
    for (int i = 0; i < 5; i++) {
        std::cout << data[i] << " ";
    }
    std::cout << '\n';

    // Range-based for (C++11)
    std::cout << "Range-for:   ";
    for (int val : data) {
        std::cout << val << " ";
    }
    std::cout << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 4. MULTIDIMENSIONAL ARRAYS (2D)
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== 2D ARRAYS ===\n";

    /*
     *  2D Array: int matrix[3][4]
     *
     *              Col 0   Col 1   Col 2   Col 3
     *            ┌───────┬───────┬───────┬───────┐
     *   Row 0   │  1    │  2    │  3    │  4    │
     *            ├───────┼───────┼───────┼───────┤
     *   Row 1   │  5    │  6    │  7    │  8    │
     *            ├───────┼───────┼───────┼───────┤
     *   Row 2   │  9    │  10   │  11   │  12   │
     *            └───────┴───────┴───────┴───────┘
     *
     *  In memory (row-major order — rows stored contiguously):
     *  ┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┐
     *  │ 1│ 2│ 3│ 4│ 5│ 6│ 7│ 8│ 9│10│11│12│
     *  └──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┘
     *  ← Row 0 →← Row 1 →← Row 2 →
     */

    int matrix[3][4] = {
        {1,  2,  3,  4},   // Row 0
        {5,  6,  7,  8},   // Row 1
        {9,  10, 11, 12}   // Row 2
    };

    std::cout << "matrix[1][2] = " << matrix[1][2] << '\n';  // 7

    // Print entire matrix
    std::cout << "Full matrix:\n";
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
            std::cout << matrix[row][col] << "\t";
        }
        std::cout << '\n';
    }

    // ═════════════════════════════════════════════════════════════════
    // 5. ARRAYS AND FUNCTIONS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== ARRAYS AND FUNCTIONS ===\n";

    /*
     *  When passing an array to a function:
     *  - Array DECAYS to a pointer (loses size information!)
     *  - Must pass size separately
     *
     *  void func(int arr[], int size);   // arr is actually int*
     *  void func(int* arr, int size);    // Same thing!
     */

    // Example: Sum an array
    auto sumArray = [](const int arr[], int size) -> int {
        int sum = 0;
        for (int i = 0; i < size; i++) {
            sum += arr[i];
        }
        return sum;
    };

    int values[] = {1, 2, 3, 4, 5};
    std::cout << "Sum = " << sumArray(values, 5) << '\n';  // 15

    // ═════════════════════════════════════════════════════════════════
    // 6. C-STYLE STRINGS (char arrays)
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== C-STYLE STRINGS ===\n";

    /*
     *  C-string = char array terminated by '\0' (null character)
     *
     *  char name[] = "Hello";
     *  ┌─────┬─────┬─────┬─────┬─────┬─────┐
     *  │ 'H' │ 'e' │ 'l' │ 'l' │ 'o' │ '\0'│  ← 6 bytes (5 chars + null)
     *  └─────┴─────┴─────┴─────┴─────┴─────┘
     *  [0]   [1]   [2]   [3]   [4]   [5]
     */

    char greeting[] = "Hello";
    char name[20] = "World";  // Extra space for null terminator

    // C-string functions (from <cstring>)
    std::cout << "strlen(greeting) = " << strlen(greeting) << '\n';  // 5
    std::cout << "sizeof(greeting) = " << sizeof(greeting) << '\n';  // 6

    // strcmp: 0 if equal, <0 if first is less, >0 if first is greater
    std::cout << "strcmp: " << strcmp("abc", "abd") << '\n';  // negative

    // strcat: concatenate (must have enough space!)
    char dest[50] = "Hello, ";
    strcat(dest, "World!");
    std::cout << "strcat: " << dest << '\n';

    // strcpy: copy string
    char copy[20];
    strcpy(copy, "Copied!");
    std::cout << "strcpy: " << copy << '\n';

    // WARNING: C-strings are error-prone (buffer overflows, no bounds checking)
    // PREFER std::string in C++ (covered in 09_strings.cpp)

    // ═════════════════════════════════════════════════════════════════
    // 7. ARRAY LIMITATIONS
    // ═════════════════════════════════════════════════════════════════

    /*
     *  Raw Array Limitations:
     *  ┌─────────────────────────────────────────────┐
     *  │ ✗ Fixed size at compile time                │
     *  │ ✗ No bounds checking                        │
     *  │ ✗ Decays to pointer when passed to function │
     *  │ ✗ Cannot be returned from functions          │
     *  │ ✗ Cannot be assigned (arr1 = arr2)          │
     *  │ ✗ No built-in size information               │
     *  └─────────────────────────────────────────────┘
     *
     *  Solution: Use std::array (C++11) or std::vector!
     *  ┌─────────────────────────────────────────────┐
     *  │ ✓ std::array<int, 5> — fixed size, safe    │
     *  │ ✓ std::vector<int>   — dynamic size, safe  │
     *  │ ✓ Both have .size(), bounds checking (.at())│
     *  │ ✓ Both work with STL algorithms             │
     *  └─────────────────────────────────────────────┘
     */

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. Arrays are 0-indexed and stored contiguously in memory
 * 2. No bounds checking — out-of-bounds access is undefined behavior
 * 3. Arrays decay to pointers when passed to functions
 * 4. 2D arrays are stored in row-major order (row by row)
 * 5. C-strings end with '\0' — always account for null terminator
 * 6. Prefer std::array or std::vector over raw arrays
 */
