/*******************************************************************************
 * 02_variables_and_datatypes.cpp — Variables, Data Types, and Type Casting
 *******************************************************************************
 *
 * CONCEPT: Variables are named storage locations in memory.
 *          Each variable has a TYPE that determines its size and behavior.
 *
 *  ┌─────────────────────────────────────────────────────────────┐
 *  │              C++ FUNDAMENTAL DATA TYPES                     │
 *  ├──────────┬──────────┬──────────────────────────────────────┤
 *  │ Category │   Type   │  Size (typical)  │     Range         │
 *  ├──────────┼──────────┼──────────────────┼───────────────────┤
 *  │ Boolean  │ bool     │ 1 byte           │ true/false        │
 *  │ Char     │ char     │ 1 byte           │ -128 to 127       │
 *  │ Integer  │ short    │ 2 bytes          │ -32768 to 32767   │
 *  │          │ int      │ 4 bytes          │ ±2.1 billion      │
 *  │          │ long     │ 8 bytes (Linux)  │ ±9.2 quintillion  │
 *  │          │ long long│ 8 bytes          │ ±9.2 quintillion  │
 *  │ Float    │ float    │ 4 bytes          │ ±3.4e±38 (7 dig) │
 *  │          │ double   │ 8 bytes          │ ±1.7e±308 (15 dig)│
 *  │          │ long dbl │ 16 bytes         │ Even more precise │
 *  │ Void     │ void     │ 0 bytes          │ No value          │
 *  └──────────┴──────────┴──────────────────┴───────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o datatypes 02_variables_and_datatypes.cpp
 ******************************************************************************/

#include <iostream>
#include <climits>    // INT_MAX, INT_MIN, etc.
#include <cfloat>     // FLT_MAX, DBL_MAX, etc.
#include <cstdint>    // int8_t, int16_t, int32_t, int64_t (fixed-width)

int main() {

    // ═════════════════════════════════════════════════════════════════
    // 1. VARIABLE DECLARATION AND INITIALIZATION
    // ═════════════════════════════════════════════════════════════════

    /*
     *  Variable in Memory:
     *
     *    Variable name: age
     *    Type: int (4 bytes)
     *    Address: 0x7fff5a2c
     *
     *    Memory:
     *    ┌────────┬────────┬────────┬────────┐
     *    │  0x19  │  0x00  │  0x00  │  0x00  │  = 25 (little-endian)
     *    └────────┴────────┴────────┴────────┘
     *    0x7fff5a2c                0x7fff5a2f
     */

    // Declaration (variable exists but has garbage value)
    int x;

    // Initialization (giving a value at declaration)
    int age = 25;                  // C-style (copy initialization)
    int height(180);               // Direct initialization
    int weight{75};                // Brace/Uniform initialization (C++11) ← PREFERRED
    int score = {100};             // Copy-list initialization

    // Why prefer brace initialization?
    // It prevents "narrowing conversions" — catches errors at compile time!
    // int bad{3.14};  // ERROR! double → int would lose data
    int ok = 3.14;     // Silently truncates to 3 (dangerous!)

    std::cout << "age: " << age << ", height: " << height
              << ", weight: " << weight << ", score: " << score << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 2. BOOLEAN TYPE
    // ═════════════════════════════════════════════════════════════════
    bool is_alive = true;    // 1
    bool is_dead = false;    // 0
    std::cout << "is_alive: " << is_alive << " (1=true, 0=false)\n";

    // ═════════════════════════════════════════════════════════════════
    // 3. CHARACTER TYPE
    // ═════════════════════════════════════════════════════════════════
    char letter = 'A';           // Single character (1 byte)
    char newline = '\n';         // Escape character
    char ascii_65 = 65;          // Same as 'A' (ASCII value)
    std::cout << "letter: " << letter << ", ascii_65: " << ascii_65 << '\n';

    /*
     * ASCII Table (partial):
     *
     *  Char │ Dec │  Char │ Dec │  Char │ Dec
     *  ─────┼─────┼───────┼─────┼───────┼─────
     *   '0' │  48 │  'A'  │  65 │  'a'  │  97
     *   '1' │  49 │  'B'  │  66 │  'b'  │  98
     *   '9' │  57 │  'Z'  │  90 │  'z'  │ 122
     *   ' ' │  32 │  '\n' │  10 │  '\0' │   0
     */

    // ═════════════════════════════════════════════════════════════════
    // 4. INTEGER TYPES AND SIZES
    // ═════════════════════════════════════════════════════════════════

    short s = 32767;             // At least 16 bits
    int i = 2147483647;          // At least 16 bits (usually 32)
    long l = 2147483647L;        // At least 32 bits
    long long ll = 9223372036854775807LL;  // At least 64 bits

    // unsigned versions (no negative numbers, double positive range)
    unsigned int ui = 4294967295U;  // 0 to ~4.2 billion
    unsigned short us = 65535;

    std::cout << "\n--- Integer Sizes ---\n";
    std::cout << "short:      " << sizeof(short)     << " bytes, max: " << SHRT_MAX  << '\n';
    std::cout << "int:        " << sizeof(int)        << " bytes, max: " << INT_MAX   << '\n';
    std::cout << "long:       " << sizeof(long)       << " bytes, max: " << LONG_MAX  << '\n';
    std::cout << "long long:  " << sizeof(long long)  << " bytes, max: " << LLONG_MAX << '\n';

    /*
     * Integer Size Comparison:
     *
     *  short (2 bytes)      ██
     *  int (4 bytes)        ████
     *  long (8 bytes)       ████████
     *  long long (8 bytes)  ████████
     *
     *  Note: 'long' is 4 bytes on Windows, 8 bytes on Linux/Mac
     */

    // ═════════════════════════════════════════════════════════════════
    // 5. FIXED-WIDTH INTEGERS (C++11) — Guaranteed sizes!
    // ═════════════════════════════════════════════════════════════════
    int8_t   i8  = 127;           // Exactly 8 bits
    int16_t  i16 = 32767;         // Exactly 16 bits
    int32_t  i32 = 2147483647;    // Exactly 32 bits
    int64_t  i64 = 9223372036854775807LL; // Exactly 64 bits

    uint8_t  u8  = 255;           // Unsigned 8 bits
    uint16_t u16 = 65535;         // Unsigned 16 bits
    uint32_t u32 = 4294967295U;   // Unsigned 32 bits

    std::cout << "\n--- Fixed-width integers ---\n";
    std::cout << "int32_t: " << i32 << ", uint8_t: " << static_cast<int>(u8) << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 6. FLOATING POINT TYPES
    // ═════════════════════════════════════════════════════════════════
    float f = 3.14f;              // 'f' suffix for float literals
    double d = 3.141592653589793;
    long double ld = 3.141592653589793238L;

    std::cout << "\n--- Floating Point Sizes ---\n";
    std::cout << "float:       " << sizeof(float)       << " bytes, precision: ~7 digits\n";
    std::cout << "double:      " << sizeof(double)      << " bytes, precision: ~15 digits\n";
    std::cout << "long double: " << sizeof(long double)  << " bytes, precision: ~18 digits\n";

    /*
     * Float vs Double precision:
     *
     *  float:  3.1415927       (7 significant digits)
     *            ───────
     *  double: 3.141592653589793  (15 significant digits)
     *            ───────────────
     *
     *  Rule of thumb: Always use 'double' unless memory is critical
     */

    // ═════════════════════════════════════════════════════════════════
    // 7. CONST AND CONSTEXPR
    // ═════════════════════════════════════════════════════════════════

    const double PI = 3.14159265358979;   // Runtime constant — cannot be changed
    // PI = 3.0;  // ERROR! Cannot modify const

    constexpr int MAX_SIZE = 100;  // Compile-time constant (C++11)
    // Evaluated at compile time — can be used in array sizes, template args

    // Difference:
    // const     → value set at runtime or compile time, cannot change after
    // constexpr → MUST be evaluable at compile time

    std::cout << "\nPI = " << PI << ", MAX_SIZE = " << MAX_SIZE << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 8. TYPE CASTING
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n--- Type Casting ---\n";

    // a) Implicit conversion (automatic, may lose data!)
    int int_val = 42;
    double double_val = int_val;  // int → double (safe, no data loss)
    std::cout << "Implicit int→double: " << double_val << '\n';

    double pi = 3.14159;
    int truncated = pi;           // double → int (DANGEROUS! loses .14159)
    std::cout << "Implicit double→int: " << truncated << '\n';

    /*
     * Implicit Conversion Hierarchy (safe direction →):
     *
     *  bool → char → short → int → long → long long → float → double
     *                    ────────── widening (safe) ──────────►
     *                    ◄─────── narrowing (dangerous) ───────
     */

    // b) C-style cast (avoid in modern C++)
    double d2 = 9.99;
    int i2 = (int)d2;  // Old C-style — no compile-time safety checks
    std::cout << "C-style cast: " << i2 << '\n';

    // c) static_cast<> (PREFERRED in C++)
    int i3 = static_cast<int>(d2);  // Safe, checked at compile time
    std::cout << "static_cast: " << i3 << '\n';

    // d) Other casts (covered in advanced topics):
    // dynamic_cast<>    — for polymorphic types (OOP)
    // const_cast<>      — remove/add constness
    // reinterpret_cast<>— bit-level reinterpretation (dangerous)

    // ═════════════════════════════════════════════════════════════════
    // 9. AUTO TYPE DEDUCTION (C++11)
    // ═════════════════════════════════════════════════════════════════
    auto a = 42;          // int
    auto b = 3.14;        // double
    auto c = 'X';         // char
    auto e = true;        // bool
    auto str = "hello";   // const char* (NOT std::string!)

    std::cout << "\n--- auto deduction ---\n";
    std::cout << "a=" << a << " b=" << b << " c=" << c << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 10. SIZEOF OPERATOR
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n--- sizeof all types ---\n";
    std::cout << "bool:        " << sizeof(bool)        << " byte(s)\n";
    std::cout << "char:        " << sizeof(char)        << " byte(s)\n";
    std::cout << "short:       " << sizeof(short)       << " byte(s)\n";
    std::cout << "int:         " << sizeof(int)         << " byte(s)\n";
    std::cout << "long:        " << sizeof(long)        << " byte(s)\n";
    std::cout << "long long:   " << sizeof(long long)   << " byte(s)\n";
    std::cout << "float:       " << sizeof(float)       << " byte(s)\n";
    std::cout << "double:      " << sizeof(double)      << " byte(s)\n";
    std::cout << "long double: " << sizeof(long double)  << " byte(s)\n";
    std::cout << "pointer:     " << sizeof(int*)        << " byte(s)\n";

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. Use brace initialization {} — it prevents narrowing conversions
 * 2. Prefer 'double' over 'float' for decimal numbers
 * 3. Use fixed-width types (int32_t) when size matters
 * 4. Use static_cast<> instead of C-style casts
 * 5. 'const' = runtime constant, 'constexpr' = compile-time constant
 * 6. 'auto' lets the compiler deduce the type (C++11)
 * 7. sizeof() tells you the size of a type in bytes
 */
