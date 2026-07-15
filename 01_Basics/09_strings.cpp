/*******************************************************************************
 * 09_strings.cpp — C-Strings vs std::string
 *******************************************************************************
 *
 * CONCEPT: C++ has two string types:
 *   1. C-style strings: char arrays ending with '\0'
 *   2. std::string: Safe, dynamic, feature-rich string class (PREFERRED)
 *
 *  ┌─────────────────────────────────────────────────────────────┐
 *  │  C-string: char s[] = "Hi"                                  │
 *  │  ┌─────┬─────┬─────┐                                       │
 *  │  │ 'H' │ 'i' │ '\0'│   Fixed size, manual management       │
 *  │  └─────┴─────┴─────┘                                       │
 *  │                                                              │
 *  │  std::string: string s = "Hi"                               │
 *  │  ┌──────────────────┐                                       │
 *  │  │ size: 2          │   Dynamic, auto-managed               │
 *  │  │ capacity: 15     │   Rich API (find, substr, append...)  │
 *  │  │ data ──────────► │ "Hi"                                  │
 *  │  └──────────────────┘                                       │
 *  └─────────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o strings 09_strings.cpp
 ******************************************************************************/

#include <iostream>
#include <string>      // std::string
#include <algorithm>   // std::transform, std::sort

int main() {

    // ═════════════════════════════════════════════════════════════════
    // 1. CREATING STRINGS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "=== CREATING STRINGS ===\n";

    std::string s1;                     // Empty string ""
    std::string s2 = "Hello";           // Copy initialization
    std::string s3("World");            // Direct initialization
    std::string s4{"C++"};              // Brace initialization (C++11)
    std::string s5(5, 'X');             // "XXXXX" (repeat char)
    std::string s6 = s2;               // Copy of s2
    std::string s7 = s2.substr(1, 3);  // "ell" (from index 1, length 3)

    std::cout << "s1 (empty): '" << s1 << "'\n";
    std::cout << "s2: " << s2 << '\n';
    std::cout << "s3: " << s3 << '\n';
    std::cout << "s5 (repeat): " << s5 << '\n';
    std::cout << "s7 (substr): " << s7 << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 2. STRING OPERATIONS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== STRING OPERATIONS ===\n";

    std::string a = "Hello";
    std::string b = "World";

    // Concatenation
    std::string c = a + " " + b;          // "Hello World"
    std::cout << "Concatenation: " << c << '\n';

    // Append
    a += " there";                         // "Hello there"
    a.append("!");                         // "Hello there!"
    std::cout << "Append: " << a << '\n';

    // Length / Size
    std::cout << "Length: " << a.length() << '\n';   // Same as .size()
    std::cout << "Size:   " << a.size()   << '\n';
    std::cout << "Empty?  " << a.empty()  << '\n';   // 0 (false)

    // Accessing characters
    std::cout << "a[0]: " << a[0] << '\n';          // 'H' (no bounds check)
    std::cout << "a.at(1): " << a.at(1) << '\n';    // 'e' (throws if out of bounds)
    std::cout << "a.front(): " << a.front() << '\n'; // 'H'
    std::cout << "a.back(): " << a.back() << '\n';   // '!'

    // ═════════════════════════════════════════════════════════════════
    // 3. SEARCHING
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== SEARCHING ===\n";

    std::string text = "The quick brown fox jumps over the lazy dog";

    // find — returns index of first occurrence (or string::npos if not found)
    size_t pos = text.find("fox");
    if (pos != std::string::npos) {
        std::cout << "'fox' found at index: " << pos << '\n';  // 16
    }

    // rfind — search from the end
    size_t rpos = text.rfind("the");
    std::cout << "Last 'the' at index: " << rpos << '\n';

    // find_first_of — find first occurrence of any char in set
    size_t vowel = text.find_first_of("aeiou");
    std::cout << "First vowel at index: " << vowel << '\n';

    /*
     *  String Search Summary:
     *
     *  ┌──────────────────────────────────────────────┐
     *  │ Method          │ Searches for               │
     *  ├──────────────────┼────────────────────────────┤
     *  │ find()           │ Substring (left to right)  │
     *  │ rfind()          │ Substring (right to left)  │
     *  │ find_first_of()  │ Any char from set          │
     *  │ find_last_of()   │ Any char from set (reverse)│
     *  │ find_first_not_of│ First char NOT in set      │
     *  └──────────────────┴────────────────────────────┘
     *  All return string::npos if not found
     */

    // ═════════════════════════════════════════════════════════════════
    // 4. MODIFYING STRINGS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== MODIFYING ===\n";

    std::string str = "Hello World";

    // Replace
    str.replace(6, 5, "C++");   // "Hello C++"
    std::cout << "Replace: " << str << '\n';

    // Insert
    str.insert(5, ",");          // "Hello, C++"
    std::cout << "Insert: " << str << '\n';

    // Erase
    str.erase(5, 1);             // "Hello C++"
    std::cout << "Erase: " << str << '\n';

    // Clear
    std::string temp = "something";
    temp.clear();                 // ""
    std::cout << "After clear: '" << temp << "' (empty=" << temp.empty() << ")\n";

    // ═════════════════════════════════════════════════════════════════
    // 5. SUBSTRING AND COMPARISON
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== SUBSTRING & COMPARISON ===\n";

    std::string full = "Hello World";
    std::string sub = full.substr(0, 5);  // "Hello"
    std::cout << "Substr: " << sub << '\n';

    // Comparison
    std::string x1 = "apple";
    std::string x2 = "banana";

    // Operators: ==, !=, <, >, <=, >= (lexicographic comparison)
    std::cout << "apple == banana? " << (x1 == x2) << '\n';  // 0
    std::cout << "apple < banana?  " << (x1 < x2)  << '\n';  // 1

    // compare(): returns 0 if equal, <0 if less, >0 if greater
    int cmp = x1.compare(x2);
    std::cout << "compare: " << cmp << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 6. STRING ITERATION
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== ITERATION ===\n";

    std::string word = "Hello";

    // Index-based
    std::cout << "Index: ";
    for (size_t i = 0; i < word.size(); i++) {
        std::cout << word[i] << ' ';
    }
    std::cout << '\n';

    // Range-based for
    std::cout << "Range: ";
    for (char ch : word) {
        std::cout << ch << ' ';
    }
    std::cout << '\n';

    // Iterator-based
    std::cout << "Iter:  ";
    for (auto it = word.begin(); it != word.end(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 7. STRING CONVERSIONS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== CONVERSIONS ===\n";

    // Number to string
    int num = 42;
    double pi = 3.14159;
    std::string num_str = std::to_string(num);     // "42"
    std::string pi_str = std::to_string(pi);       // "3.141590"
    std::cout << "to_string(42): " << num_str << '\n';
    std::cout << "to_string(pi): " << pi_str << '\n';

    // String to number
    std::string s_int = "123";
    std::string s_dbl = "3.14";
    int n = std::stoi(s_int);         // String to int
    double d = std::stod(s_dbl);      // String to double
    std::cout << "stoi(\"123\"): " << n << '\n';
    std::cout << "stod(\"3.14\"): " << d << '\n';

    // Also: stol(), stoll(), stof(), stoul(), stoull()

    // std::string ↔ C-string
    std::string cpp_str = "C++ String";
    const char* c_str = cpp_str.c_str();   // Get C-string pointer
    std::cout << "c_str(): " << c_str << '\n';

    std::string from_c = std::string(c_str);  // C-string to std::string
    std::cout << "From C: " << from_c << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 8. USEFUL STRING ALGORITHMS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== STRING ALGORITHMS ===\n";

    // To uppercase
    std::string upper = "hello world";
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    std::cout << "Upper: " << upper << '\n';

    // To lowercase
    std::transform(upper.begin(), upper.end(), upper.begin(), ::tolower);
    std::cout << "Lower: " << upper << '\n';

    // Reverse
    std::string rev = "abcdef";
    std::reverse(rev.begin(), rev.end());
    std::cout << "Reversed: " << rev << '\n';

    // Sort characters
    std::string sorted = "dcba";
    std::sort(sorted.begin(), sorted.end());
    std::cout << "Sorted: " << sorted << '\n';

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. Always prefer std::string over C-strings (char arrays)
 * 2. Use .at() for bounds-checked access, [] for fast unchecked access
 * 3. find() returns string::npos when substring not found
 * 4. std::to_string() and std::stoi/stod() for conversions
 * 5. .c_str() converts std::string to C-style string
 * 6. std::string is dynamically sized — no buffer overflow worries
 * 7. String comparison operators work lexicographically
 */
