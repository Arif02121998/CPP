/*******************************************************************************
 * 10_input_output.cpp — Console I/O and File I/O
 *******************************************************************************
 *
 * CONCEPT: C++ uses streams for I/O. Data flows between your program and
 *          external sources (keyboard, screen, files).
 *
 *  ┌─────────────────────────────────────────────────────────────┐
 *  │                  I/O STREAM HIERARCHY                        │
 *  │                                                              │
 *  │   Keyboard ──► std::cin  ──► Your Program                   │
 *  │                                    │                         │
 *  │   Your Program ──► std::cout ──► Screen                     │
 *  │   Your Program ──► std::cerr ──► Screen (errors, unbuffered)│
 *  │   Your Program ──► std::clog ──► Screen (log, buffered)     │
 *  │                                                              │
 *  │   File ──► std::ifstream ──► Your Program                   │
 *  │   Your Program ──► std::ofstream ──► File                   │
 *  │   Both dirs ◄──► std::fstream ◄──► File                     │
 *  └─────────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o io 10_input_output.cpp
 ******************************************************************************/

#include <iostream>   // cin, cout, cerr
#include <iomanip>    // setw, setprecision, fixed, etc.
#include <string>
#include <sstream>    // stringstream
#include <fstream>    // ifstream, ofstream, fstream

int main() {

    // ═════════════════════════════════════════════════════════════════
    // 1. CONSOLE OUTPUT (std::cout)
    // ═════════════════════════════════════════════════════════════════
    std::cout << "=== CONSOLE OUTPUT ===\n";

    int age = 25;
    double pi = 3.14159265;
    std::string name = "Alice";

    // Basic output
    std::cout << "Name: " << name << ", Age: " << age << '\n';

    // Formatting with <iomanip>
    std::cout << "\n--- Output Formatting ---\n";

    // Fixed precision
    std::cout << "Default pi: " << pi << '\n';
    std::cout << "Fixed 2:    " << std::fixed << std::setprecision(2) << pi << '\n';
    std::cout << "Fixed 6:    " << std::fixed << std::setprecision(6) << pi << '\n';
    std::cout << std::defaultfloat;  // Reset

    // Width and alignment
    std::cout << "\n--- Width & Alignment ---\n";
    std::cout << std::left  << std::setw(15) << "Name" << std::setw(10) << "Score" << '\n';
    std::cout << std::left  << std::setw(15) << "Alice" << std::setw(10) << 95 << '\n';
    std::cout << std::left  << std::setw(15) << "Bob" << std::setw(10) << 87 << '\n';
    std::cout << std::left  << std::setw(15) << "Charlie" << std::setw(10) << 92 << '\n';

    // Fill character
    std::cout << "\n--- Fill Character ---\n";
    std::cout << std::setfill('*') << std::setw(20) << "TITLE" << '\n';
    std::cout << std::setfill('.') << std::left << std::setw(20) << "Item" << "Done" << '\n';
    std::cout << std::setfill(' ');  // Reset

    // Number bases
    std::cout << "\n--- Number Bases ---\n";
    int num = 255;
    std::cout << "Decimal: " << std::dec << num << '\n';
    std::cout << "Octal:   " << std::oct << num << '\n';
    std::cout << "Hex:     " << std::hex << num << '\n';
    std::cout << "Hex(0x): " << std::showbase << std::hex << num << '\n';
    std::cout << std::dec << std::noshowbase;  // Reset

    // Boolean output
    std::cout << "\n--- Boolean ---\n";
    std::cout << "Default:  " << true << '\n';              // 1
    std::cout << "Alpha:    " << std::boolalpha << true << '\n';  // true
    std::cout << std::noboolalpha;

    // ═════════════════════════════════════════════════════════════════
    // 2. CONSOLE INPUT (std::cin)
    // ═════════════════════════════════════════════════════════════════
    /*
     *  std::cin reads from keyboard:
     *
     *                    ┌──────────────┐
     *  Keyboard ──────►  │  input buffer │ ──► cin >> variable
     *                    └──────────────┘
     *
     *  cin >> stops at whitespace (space, tab, newline)
     *  getline() reads entire line including spaces
     *
     *  UNCOMMENT the code below to test interactively:
     */

    // --- Reading integers ---
    // int user_age;
    // std::cout << "Enter your age: ";
    // std::cin >> user_age;
    // std::cout << "You entered: " << user_age << '\n';

    // --- Reading strings (single word) ---
    // std::string first_name;
    // std::cout << "Enter first name: ";
    // std::cin >> first_name;  // Stops at space!

    // --- Reading full line ---
    // std::string full_name;
    // std::cin.ignore();  // Clear leftover newline from previous cin >>
    // std::cout << "Enter full name: ";
    // std::getline(std::cin, full_name);  // Reads entire line

    // --- Reading multiple values ---
    // int x, y;
    // std::cout << "Enter two numbers: ";
    // std::cin >> x >> y;  // Reads space-separated values
    // std::cout << "Sum: " << (x + y) << '\n';

    /*
     *  cin >> vs getline:
     *
     *  Input: "John Doe 25"
     *
     *  cin >> str     → str = "John"    (stops at space)
     *  getline(cin,s) → s = "John Doe 25" (reads whole line)
     *
     *  PITFALL: Mixing cin >> and getline:
     *    cin >> x;          // Reads value, leaves '\n' in buffer
     *    getline(cin, s);   // Reads leftover '\n' → empty string!
     *    
     *    FIX: cin.ignore() between cin >> and getline
     */

    // ═════════════════════════════════════════════════════════════════
    // 3. STRING STREAMS (std::stringstream)
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== STRING STREAMS ===\n";

    // Building strings (like sprintf)
    std::ostringstream oss;
    oss << "Name: " << name << ", Age: " << age << ", PI: " << std::fixed
        << std::setprecision(2) << pi;
    std::string result = oss.str();
    std::cout << "Built string: " << result << '\n';

    // Parsing strings (like sscanf)
    std::string data = "42 3.14 Hello";
    std::istringstream iss(data);
    int n;
    double d;
    std::string s;
    iss >> n >> d >> s;
    std::cout << "Parsed: n=" << n << ", d=" << d << ", s=" << s << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 4. FILE I/O
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== FILE I/O ===\n";

    /*
     *  File Stream Classes:
     *
     *  ┌────────────────┬────────────────────────────────────┐
     *  │ std::ofstream  │ Output (write to file)             │
     *  │ std::ifstream  │ Input (read from file)             │
     *  │ std::fstream   │ Both input and output              │
     *  └────────────────┴────────────────────────────────────┘
     *
     *  File Open Modes:
     *  ┌──────────────┬──────────────────────────────────────┐
     *  │ ios::in      │ Open for reading                     │
     *  │ ios::out     │ Open for writing (creates/truncates) │
     *  │ ios::app     │ Append to end                        │
     *  │ ios::trunc   │ Truncate file on open                │
     *  │ ios::binary  │ Binary mode                          │
     *  │ ios::ate     │ Seek to end after open               │
     *  └──────────────┴──────────────────────────────────────┘
     */

    // --- Writing to a file ---
    std::ofstream outfile("/tmp/cpp_io_example.txt");
    if (outfile.is_open()) {
        outfile << "Line 1: Hello, File!" << '\n';
        outfile << "Line 2: Number = " << 42 << '\n';
        outfile << "Line 3: Pi = " << 3.14159 << '\n';
        outfile.close();
        std::cout << "File written successfully.\n";
    } else {
        std::cerr << "Error opening file for writing!\n";
    }

    // --- Reading from a file ---
    std::ifstream infile("/tmp/cpp_io_example.txt");
    if (infile.is_open()) {
        std::string line;
        std::cout << "File contents:\n";
        while (std::getline(infile, line)) {
            std::cout << "  " << line << '\n';
        }
        infile.close();
    } else {
        std::cerr << "Error opening file for reading!\n";
    }

    // --- Append to a file ---
    std::ofstream appendfile("/tmp/cpp_io_example.txt", std::ios::app);
    if (appendfile.is_open()) {
        appendfile << "Line 4: Appended line!\n";
        appendfile.close();
        std::cout << "Line appended.\n";
    }

    // ═════════════════════════════════════════════════════════════════
    // 5. ERROR STREAMS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== ERROR STREAMS ===\n";

    std::cout << "This goes to stdout (normal output)\n";
    std::cerr << "This goes to stderr (error output, unbuffered)\n";
    std::clog << "This goes to stderr (log output, buffered)\n";

    // Redirecting in terminal:
    // ./program > stdout.txt 2> stderr.txt
    // ./program > all.txt 2>&1  (both to same file)

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. cout for output, cin for input, cerr for errors
 * 2. Use <iomanip> for formatted output (setw, setprecision, etc.)
 * 3. cin >> stops at whitespace; getline() reads full line
 * 4. Always cin.ignore() between cin >> and getline()
 * 5. stringstream is great for building/parsing strings
 * 6. Always check if file opened successfully before reading/writing
 * 7. Close files when done (or use RAII — file closes on scope exit)
 */
