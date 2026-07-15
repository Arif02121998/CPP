/*******************************************************************************
 * 04_control_flow.cpp — Decision Making and Loops
 *******************************************************************************
 *
 * CONCEPT: Control flow determines the order in which statements execute.
 *
 *  ┌─────────────────────────────────────────────────────────┐
 *  │                 CONTROL FLOW TYPES                       │
 *  ├─────────────────────────────────────────────────────────┤
 *  │                                                         │
 *  │  BRANCHING          LOOPS              JUMP              │
 *  │  ┌─────────┐       ┌──────────┐      ┌──────────┐      │
 *  │  │ if/else │       │ for      │      │ break    │      │
 *  │  │ switch  │       │ while    │      │ continue │      │
 *  │  │ ternary │       │ do-while │      │ goto     │      │
 *  │  └─────────┘       │ range-for│      │ return   │      │
 *  │                    └──────────┘      └──────────┘      │
 *  └─────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o control 04_control_flow.cpp
 ******************************************************************************/

#include <iostream>
#include <vector>
#include <string>

int main() {

    // ═════════════════════════════════════════════════════════════════
    // 1. IF / ELSE IF / ELSE
    // ═════════════════════════════════════════════════════════════════
    std::cout << "=== IF / ELSE ===\n";

    /*
     *  Flow Diagram:
     *
     *        ┌─────────┐
     *        │condition │
     *        └────┬─────┘
     *          ┌──┴──┐
     *        true   false
     *          │      │
     *          ▼      ▼
     *       ┌─────┐ ┌──────┐
     *       │ if  │ │ else │
     *       │block│ │block │
     *       └──┬──┘ └──┬───┘
     *          │      │
     *          └──┬───┘
     *             ▼
     *         (continue)
     */

    int temperature = 35;

    if (temperature > 40) {
        std::cout << "It's extremely hot!\n";
    } else if (temperature > 30) {
        std::cout << "It's hot! (temp=" << temperature << ")\n";
    } else if (temperature > 20) {
        std::cout << "It's warm.\n";
    } else if (temperature > 10) {
        std::cout << "It's cool.\n";
    } else {
        std::cout << "It's cold!\n";
    }

    // C++17: if with initializer
    // Variable 'it' only exists inside the if/else scope
    std::vector<int> vec = {10, 20, 30};
    if (auto size = vec.size(); size > 2) {
        std::cout << "Vector has " << size << " elements (> 2)\n";
    }

    // ═════════════════════════════════════════════════════════════════
    // 2. SWITCH STATEMENT
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== SWITCH ===\n";

    /*
     *  Flow Diagram:
     *
     *        ┌──────────┐
     *        │expression│
     *        └────┬─────┘
     *             │
     *      ┌──────┼──────┬──────┐
     *      ▼      ▼      ▼      ▼
     *   case 1  case 2  case 3  default
     *      │      │      │      │
     *    break  break  break    │
     *      │      │      │      │
     *      └──────┴──────┴──────┘
     *             │
     *             ▼
     *         (continue)
     *
     *  WARNING: Without 'break', execution "falls through" to next case!
     */

    int day = 3;
    switch (day) {
        case 1:
            std::cout << "Monday\n";
            break;
        case 2:
            std::cout << "Tuesday\n";
            break;
        case 3:
            std::cout << "Wednesday\n";
            break;
        case 4:
            std::cout << "Thursday\n";
            break;
        case 5:
            std::cout << "Friday\n";
            break;
        case 6:
        case 7:   // Fall-through: both 6 and 7 reach here
            std::cout << "Weekend!\n";
            break;
        default:
            std::cout << "Invalid day\n";
            break;
    }

    // C++17: switch with initializer
    switch (int val = 42; val % 3) {
        case 0: std::cout << val << " is divisible by 3\n"; break;
        case 1: std::cout << val << " remainder 1\n"; break;
        case 2: std::cout << val << " remainder 2\n"; break;
    }

    // ═════════════════════════════════════════════════════════════════
    // 3. FOR LOOP
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== FOR LOOP ===\n";

    /*
     *  for (init; condition; update) { body }
     *
     *       ┌──────┐
     *       │ init │ (runs once)
     *       └──┬───┘
     *          │
     *          ▼
     *    ┌───────────┐  false
     *    │ condition? │────────► EXIT
     *    └─────┬─────┘
     *        true
     *          │
     *          ▼
     *     ┌────────┐
     *     │  body  │
     *     └───┬────┘
     *         │
     *         ▼
     *    ┌────────┐
     *    │ update │
     *    └───┬────┘
     *        │
     *        └──────► (back to condition)
     */

    // Basic for loop
    for (int i = 0; i < 5; i++) {
        std::cout << "i = " << i << " ";
    }
    std::cout << '\n';

    // Multiple variables in for loop
    for (int i = 0, j = 10; i < j; i++, j--) {
        std::cout << "i=" << i << " j=" << j << "  ";
    }
    std::cout << '\n';

    // Infinite loop (be careful!)
    // for (;;) { /* runs forever */ }

    // ═════════════════════════════════════════════════════════════════
    // 4. RANGE-BASED FOR LOOP (C++11)
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== RANGE-BASED FOR (C++11) ===\n";

    std::vector<int> numbers = {10, 20, 30, 40, 50};

    // By value (copy each element)
    std::cout << "By value: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << '\n';

    // By reference (modify in place)
    for (int& num : numbers) {
        num *= 2;  // Double each element
    }

    // By const reference (read-only, no copy)
    std::cout << "Doubled:  ";
    for (const int& num : numbers) {
        std::cout << num << " ";
    }
    std::cout << '\n';

    // Using auto (let compiler deduce type)
    std::cout << "Auto:     ";
    for (const auto& num : numbers) {
        std::cout << num << " ";
    }
    std::cout << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 5. WHILE LOOP
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== WHILE LOOP ===\n";

    /*
     *    ┌───────────┐  false
     *    │ condition? │────────► EXIT
     *    └─────┬─────┘
     *        true
     *          │
     *          ▼
     *     ┌────────┐
     *     │  body  │
     *     └───┬────┘
     *         │
     *         └──────► (back to condition)
     *
     *  NOTE: Condition checked BEFORE body → may execute 0 times
     */

    int count = 5;
    while (count > 0) {
        std::cout << "Countdown: " << count << '\n';
        count--;
    }

    // ═════════════════════════════════════════════════════════════════
    // 6. DO-WHILE LOOP
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== DO-WHILE LOOP ===\n";

    /*
     *     ┌────────┐
     *     │  body  │ ◄──── Always executes at LEAST once!
     *     └───┬────┘
     *         │
     *         ▼
     *    ┌───────────┐  true
     *    │ condition? │────────► (back to body)
     *    └─────┬─────┘
     *        false
     *          │
     *          ▼
     *         EXIT
     */

    int num = 0;
    do {
        std::cout << "This runs at least once: num = " << num << '\n';
        num++;
    } while (num < 3);

    // ═════════════════════════════════════════════════════════════════
    // 7. BREAK AND CONTINUE
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== BREAK AND CONTINUE ===\n";

    // break — exits the loop entirely
    std::cout << "Break example: ";
    for (int i = 0; i < 10; i++) {
        if (i == 5) break;  // Stop at 5
        std::cout << i << " ";
    }
    std::cout << '\n';

    // continue — skip current iteration, jump to next
    std::cout << "Continue (skip odds): ";
    for (int i = 0; i < 10; i++) {
        if (i % 2 != 0) continue;  // Skip odd numbers
        std::cout << i << " ";
    }
    std::cout << '\n';

    /*
     *  Break vs Continue:
     *
     *  for (...)      for (...)
     *  {              {
     *    ...            ...
     *    break; ──►     continue; ──┐
     *    ...            ...         │
     *  }              }  ◄──────────┘
     *  │              │
     *  ▼              ▼
     *  (here)         (next iteration)
     */

    // ═════════════════════════════════════════════════════════════════
    // 8. NESTED LOOPS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== NESTED LOOPS (Multiplication Table) ===\n";

    for (int i = 1; i <= 5; i++) {
        for (int j = 1; j <= 5; j++) {
            std::cout << (i * j) << "\t";
        }
        std::cout << '\n';
    }

    // ═════════════════════════════════════════════════════════════════
    // 9. PRACTICAL PATTERN: Star Pyramid
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== STAR PYRAMID ===\n";

    int rows = 5;
    for (int i = 1; i <= rows; i++) {
        // Print spaces
        for (int j = rows - i; j > 0; j--) {
            std::cout << " ";
        }
        // Print stars
        for (int k = 0; k < 2 * i - 1; k++) {
            std::cout << "*";
        }
        std::cout << '\n';
    }

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. Use if/else for complex conditions, switch for discrete values
 * 2. Range-based for (C++11) is cleaner than index-based for
 * 3. Use const auto& in range-for to avoid copies
 * 4. while checks first (may skip body), do-while runs at least once
 * 5. break exits the loop; continue skips to next iteration
 * 6. C++17 adds initializers to if and switch statements
 */
