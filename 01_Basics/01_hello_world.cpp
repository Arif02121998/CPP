/*******************************************************************************
 * 01_hello_world.cpp — Your First C++ Program
 *******************************************************************************
 *
 * CONCEPT: Every C++ program starts with the main() function.
 *          The preprocessor directive #include brings in library headers.
 *
 *  ┌────────────────────────────────────────────────────┐
 *  │  #include <iostream>    ◄── Include I/O library    │
 *  │                                                    │
 *  │  int main() {           ◄── Entry point            │
 *  │      std::cout << "Hi"; ◄── Print to console       │
 *  │      return 0;          ◄── Exit code (0 = success)│
 *  │  }                                                 │
 *  └────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o hello 01_hello_world.cpp
 * RUN:     ./hello
 ******************************************************************************/

#include <iostream>  // For std::cout, std::cin, std::endl

// ─────────────────────────────────────────────────────────────────────────────
// The main() function: Every C++ program must have exactly one main()
// The OS calls this function when your program starts.
// Return type 'int' — returning 0 means success, non-zero means error.
// ─────────────────────────────────────────────────────────────────────────────
int main() {

    // ═══════════════════════════════════════════════════════════════
    // 1. BASIC OUTPUT
    // ═══════════════════════════════════════════════════════════════

    // std::cout  — "character output" stream (prints to console)
    // <<         — insertion operator (sends data to the stream)
    // std::endl  — inserts newline AND flushes the buffer
    // '\n'       — just inserts newline (faster, no flush)

    std::cout << "Hello, World!" << std::endl;
    std::cout << "Hello again!" << '\n';

    /*
     * Stream Flow Diagram:
     *
     *   Your Program          Output Stream          Console
     *  ┌──────────┐         ┌──────────────┐       ┌─────────┐
     *  │ "Hello"  │──<< ──►│   std::cout   │─────►│ Terminal │
     *  └──────────┘         │   (buffer)    │       └─────────┘
     *                       └──────────────┘
     *                             │
     *                        std::endl or
     *                        flush() empties
     *                        the buffer
     */

    // ═══════════════════════════════════════════════════════════════
    // 2. CHAINING OUTPUT
    // ═══════════════════════════════════════════════════════════════
    // You can chain multiple << operators
    int age = 25;
    std::cout << "I am " << age << " years old." << '\n';

    // ═══════════════════════════════════════════════════════════════
    // 3. USING NAMESPACE (convenience, but not recommended in headers)
    // ═══════════════════════════════════════════════════════════════
    // Instead of typing std:: every time, you CAN do:
    //     using namespace std;
    // But this pollutes the global namespace and can cause conflicts.
    // Best practice: use std:: explicitly or selective using declarations:
    //     using std::cout;
    //     using std::endl;

    // ═══════════════════════════════════════════════════════════════
    // 4. COMMENTS
    // ═══════════════════════════════════════════════════════════════
    // Single-line comment

    /* Multi-line
       comment */

    // ═══════════════════════════════════════════════════════════════
    // 5. RETURN VALUE
    // ═══════════════════════════════════════════════════════════════
    // 'return 0' indicates successful execution
    // The OS receives this value (check with: echo $?)
    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. #include <iostream> brings in I/O functionality
 * 2. main() is the entry point — exactly one per program
 * 3. std::cout << prints output
 * 4. std::endl flushes buffer + newline; '\n' is just newline
 * 5. return 0 means success
 * 6. Prefer std:: prefix over "using namespace std;"
 */
