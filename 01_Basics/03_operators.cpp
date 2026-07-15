/*******************************************************************************
 * 03_operators.cpp — All C++ Operators Explained
 *******************************************************************************
 *
 * CONCEPT: Operators are symbols that perform operations on operands.
 *
 *  ┌──────────────────────────────────────────────────┐
 *  │              OPERATOR CATEGORIES                  │
 *  ├──────────────────────────────────────────────────┤
 *  │  1. Arithmetic      +  -  *  /  %  ++  --       │
 *  │  2. Assignment       =  +=  -=  *=  /=  %=      │
 *  │  3. Comparison       ==  !=  <  >  <=  >=       │
 *  │  4. Logical          &&  ||  !                   │
 *  │  5. Bitwise          &  |  ^  ~  <<  >>         │
 *  │  6. Ternary          ? :                         │
 *  │  7. Comma            ,                           │
 *  │  8. Scope Resolution ::                          │
 *  │  9. Sizeof           sizeof()                    │
 *  └──────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o operators 03_operators.cpp
 ******************************************************************************/

#include <iostream>
#include <iomanip>  // For std::boolalpha

int main() {

    // ═════════════════════════════════════════════════════════════════
    // 1. ARITHMETIC OPERATORS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "=== ARITHMETIC OPERATORS ===\n";

    int a = 17, b = 5;

    std::cout << "a = " << a << ", b = " << b << '\n';
    std::cout << "a + b = " << (a + b) << '\n';   // 22  Addition
    std::cout << "a - b = " << (a - b) << '\n';   // 12  Subtraction
    std::cout << "a * b = " << (a * b) << '\n';   // 85  Multiplication
    std::cout << "a / b = " << (a / b) << '\n';   // 3   Integer division (truncates!)
    std::cout << "a % b = " << (a % b) << '\n';   // 2   Modulus (remainder)

    /*
     * Integer Division vs Float Division:
     *
     *   17 / 5   = 3       ← Integer division (both operands int)
     *   17.0 / 5 = 3.4     ← Float division (at least one operand is double)
     *   17 / 5.0 = 3.4     ← Float division
     */

    std::cout << "17.0 / 5 = " << (17.0 / 5) << '\n';  // 3.4

    // ─────────────────────────────────────────────────────────────────
    // INCREMENT (++) and DECREMENT (--)
    // ─────────────────────────────────────────────────────────────────
    int x = 10;

    /*
     * Pre-increment vs Post-increment:
     *
     *   ++x (pre):   Increment FIRST, then use the value
     *   x++ (post):  Use the value FIRST, then increment
     *
     *   Timeline for x = 10:
     *
     *   Expression │ Value Returned │ x After
     *   ───────────┼────────────────┼────────
     *   ++x        │ 11             │ 11
     *   x++        │ 11             │ 12
     */

    std::cout << "\nx = " << x << '\n';
    std::cout << "++x = " << (++x) << '\n';   // 11 (increment then return)
    std::cout << "x++ = " << (x++) << '\n';   // 11 (return then increment)
    std::cout << "x now = " << x << '\n';     // 12

    // ═════════════════════════════════════════════════════════════════
    // 2. ASSIGNMENT OPERATORS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== ASSIGNMENT OPERATORS ===\n";

    int n = 20;
    std::cout << "n = " << n << '\n';

    n += 5;   // n = n + 5  → 25
    std::cout << "n += 5  → " << n << '\n';

    n -= 3;   // n = n - 3  → 22
    std::cout << "n -= 3  → " << n << '\n';

    n *= 2;   // n = n * 2  → 44
    std::cout << "n *= 2  → " << n << '\n';

    n /= 4;   // n = n / 4  → 11
    std::cout << "n /= 4  → " << n << '\n';

    n %= 3;   // n = n % 3  → 2
    std::cout << "n %= 3  → " << n << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 3. COMPARISON (RELATIONAL) OPERATORS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== COMPARISON OPERATORS ===\n";
    std::cout << std::boolalpha;  // Print true/false instead of 1/0

    int p = 10, q = 20;
    std::cout << "p = " << p << ", q = " << q << '\n';
    std::cout << "p == q: " << (p == q) << '\n';  // false
    std::cout << "p != q: " << (p != q) << '\n';  // true
    std::cout << "p <  q: " << (p <  q) << '\n';  // true
    std::cout << "p >  q: " << (p >  q) << '\n';  // false
    std::cout << "p <= q: " << (p <= q) << '\n';  // true
    std::cout << "p >= q: " << (p >= q) << '\n';  // false

    // ═════════════════════════════════════════════════════════════════
    // 4. LOGICAL OPERATORS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== LOGICAL OPERATORS ===\n";

    /*
     * Truth Table:
     *
     *   A     │  B     │  A && B  │  A || B  │  !A
     *   ──────┼────────┼──────────┼──────────┼──────
     *   true  │ true   │  true    │  true    │ false
     *   true  │ false  │  false   │  true    │ false
     *   false │ true   │  false   │  true    │ true
     *   false │ false  │  false   │  false   │ true
     *
     * SHORT-CIRCUIT EVALUATION:
     *   && : If left is false, right is NOT evaluated
     *   || : If left is true, right is NOT evaluated
     */

    bool t = true, f = false;

    std::cout << "true  && true  = " << (t && t) << '\n';  // true
    std::cout << "true  && false = " << (t && f) << '\n';  // false
    std::cout << "false || true  = " << (f || t) << '\n';  // true
    std::cout << "false || false = " << (f || f) << '\n';  // false
    std::cout << "!true          = " << (!t)     << '\n';  // false
    std::cout << "!false         = " << (!f)     << '\n';  // true

    // ═════════════════════════════════════════════════════════════════
    // 5. BITWISE OPERATORS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== BITWISE OPERATORS ===\n";
    std::cout << std::noboolalpha;

    /*
     * Bitwise operations on 8-bit values:
     *
     *   a = 12  →  0000 1100
     *   b = 10  →  0000 1010
     *   ─────────────────────
     *   a & b   →  0000 1000  = 8   (AND: both bits must be 1)
     *   a | b   →  0000 1110  = 14  (OR: at least one bit is 1)
     *   a ^ b   →  0000 0110  = 6   (XOR: bits must differ)
     *   ~a      →  1111 0011  = -13 (NOT: flip all bits)
     *   a << 2  →  0011 0000  = 48  (Left shift: multiply by 2^n)
     *   a >> 2  →  0000 0011  = 3   (Right shift: divide by 2^n)
     */

    int ba = 12, bb = 10;
    std::cout << "a = " << ba << " (0b1100), b = " << bb << " (0b1010)\n";
    std::cout << "a & b  = " << (ba & bb)  << '\n';   // 8
    std::cout << "a | b  = " << (ba | bb)  << '\n';   // 14
    std::cout << "a ^ b  = " << (ba ^ bb)  << '\n';   // 6
    std::cout << "~a     = " << (~ba)      << '\n';   // -13
    std::cout << "a << 2 = " << (ba << 2)  << '\n';   // 48
    std::cout << "a >> 2 = " << (ba >> 2)  << '\n';   // 3

    // Common bitwise tricks:
    // Check if even:    (n & 1) == 0
    // Swap without temp: a ^= b; b ^= a; a ^= b;
    // Set bit i:         n |= (1 << i)
    // Clear bit i:       n &= ~(1 << i)
    // Toggle bit i:      n ^= (1 << i)
    // Check bit i:       (n >> i) & 1

    // ═════════════════════════════════════════════════════════════════
    // 6. TERNARY OPERATOR (? :)
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== TERNARY OPERATOR ===\n";

    /*
     *  condition ? value_if_true : value_if_false
     *
     *       ┌──── condition ────┐
     *       │                   │
     *     true?               false?
     *       │                   │
     *       ▼                   ▼
     *   value_if_true      value_if_false
     */

    int age = 20;
    std::string status = (age >= 18) ? "adult" : "minor";
    std::cout << "Age " << age << " → " << status << '\n';

    // Nested ternary (avoid — hard to read)
    int score = 85;
    std::string grade = (score >= 90) ? "A" :
                        (score >= 80) ? "B" :
                        (score >= 70) ? "C" : "F";
    std::cout << "Score " << score << " → Grade " << grade << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 7. OPERATOR PRECEDENCE (highest to lowest)
    // ═════════════════════════════════════════════════════════════════

    /*
     *  Precedence │ Operator          │ Associativity
     *  ───────────┼───────────────────┼──────────────
     *       1     │ ::                │ Left to right
     *       2     │ () [] -> .        │ Left to right
     *       3     │ ++ -- (post)      │ Left to right
     *       4     │ ++ -- (pre) ! ~   │ Right to left
     *       5     │ * / %             │ Left to right
     *       6     │ + -               │ Left to right
     *       7     │ << >>             │ Left to right
     *       8     │ < <= > >=         │ Left to right
     *       9     │ == !=             │ Left to right
     *      10     │ &                 │ Left to right
     *      11     │ ^                 │ Left to right
     *      12     │ |                 │ Left to right
     *      13     │ &&                │ Left to right
     *      14     │ ||                │ Left to right
     *      15     │ ? :               │ Right to left
     *      16     │ = += -= etc.      │ Right to left
     *      17     │ ,                 │ Left to right
     *
     *  TIP: When in doubt, use parentheses () to make intent clear!
     */

    // Example showing precedence matters:
    int result = 2 + 3 * 4;      // = 14 (not 20), because * before +
    int forced = (2 + 3) * 4;    // = 20, parentheses override

    std::cout << "\n2 + 3 * 4 = " << result << '\n';
    std::cout << "(2 + 3) * 4 = " << forced << '\n';

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. Integer division truncates — use floating point for decimals
 * 2. Pre-increment (++x) increments then returns; post (x++) returns then increments
 * 3. && and || use short-circuit evaluation
 * 4. Bitwise operators work on individual bits — useful for flags & optimization
 * 5. Ternary operator is a compact if-else for simple conditions
 * 6. When in doubt about precedence, use parentheses
 */
