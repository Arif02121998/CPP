/*******************************************************************************
 * 01_exception_handling.cpp — Exception safety, custom exceptions, noexcept
 *******************************************************************************
 *
 *  Exception Safety Guarantees:
 *
 *   ┌──────────────────────────────────────────────────────────────────┐
 *   │  Guarantee        │  Description                                 │
 *   ├───────────────────┼─────────────────────────────────────────────┤
 *   │  No-throw          │  Never throws. Use noexcept.                │
 *   │  (nothrow)         │  e.g.: destructors, move, swap              │
 *   ├───────────────────┼─────────────────────────────────────────────┤
 *   │  Strong            │  If exception occurs, state is rolled       │
 *   │  (commit-or-rollbk)│  back as if operation never happened.       │
 *   │                    │  Achieved via copy-and-swap idiom.          │
 *   ├───────────────────┼─────────────────────────────────────────────┤
 *   │  Basic             │  If exception occurs, no resources leak,    │
 *   │                    │  objects remain in a valid (but unspecified) │
 *   │                    │  state.                                     │
 *   ├───────────────────┼─────────────────────────────────────────────┤
 *   │  No guarantee      │  Anything can happen. AVOID THIS.           │
 *   └───────────────────┴─────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++20 -o exceptions 01_exception_handling.cpp
 ******************************************************************************/

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <cstring>

// ═══════════════════════════════════════════════════════════════
//  1. Basic try/catch/throw
// ═══════════════════════════════════════════════════════════════

double safeDivide(double a, double b) {
    if (b == 0.0) {
        throw std::invalid_argument("Division by zero");
    }
    return a / b;
}

void demo_basic() {
    std::cout << "═══ BASIC TRY/CATCH ═══\n";

    try {
        std::cout << "10 / 3 = " << safeDivide(10, 3) << '\n';
        std::cout << "10 / 0 = " << safeDivide(10, 0) << '\n';
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Caught: " << e.what() << '\n';
    }

    // Catch hierarchy
    try {
        throw std::runtime_error("something broke");
    }
    catch (const std::exception& e) {   // Base class catches all standard exceptions
        std::cout << "Base catch: " << e.what() << '\n';
    }

    // Catch-all
    try {
        throw 42;  // Throwing non-exception type (bad practice!)
    }
    catch (...) {
        std::cout << "Caught unknown exception\n";
    }
}


// ═══════════════════════════════════════════════════════════════
//  2. Standard exception hierarchy
// ═══════════════════════════════════════════════════════════════

/*
 *   std::exception
 *   ├── std::logic_error
 *   │   ├── std::invalid_argument
 *   │   ├── std::domain_error
 *   │   ├── std::length_error
 *   │   └── std::out_of_range
 *   └── std::runtime_error
 *       ├── std::range_error
 *       ├── std::overflow_error
 *       └── std::underflow_error
 */

void demo_standard_exceptions() {
    std::cout << "\n═══ STANDARD EXCEPTIONS ═══\n";

    std::vector<int> v{1, 2, 3};

    try {
        v.at(10);  // throws std::out_of_range
    } catch (const std::out_of_range& e) {
        std::cout << "out_of_range: " << e.what() << '\n';
    }

    try {
        std::string s(std::string().max_size() + 1, 'x');
    } catch (const std::length_error& e) {
        std::cout << "length_error: " << e.what() << '\n';
    }
}


// ═══════════════════════════════════════════════════════════════
//  3. Custom exception classes
// ═══════════════════════════════════════════════════════════════

class NetworkError : public std::runtime_error {
    int code_;
public:
    NetworkError(const std::string& msg, int code)
        : std::runtime_error(msg), code_(code) {}

    int code() const noexcept { return code_; }
};

class TimeoutError : public NetworkError {
public:
    TimeoutError(const std::string& host)
        : NetworkError("Connection timed out to " + host, 408) {}
};

void connectToServer(const std::string& host) {
    if (host == "bad-server.com") {
        throw TimeoutError(host);
    }
    if (host.empty()) {
        throw NetworkError("Empty hostname", 400);
    }
    std::cout << "Connected to " << host << '\n';
}

void demo_custom_exceptions() {
    std::cout << "\n═══ CUSTOM EXCEPTIONS ═══\n";

    try {
        connectToServer("good-server.com");
        connectToServer("bad-server.com");
    }
    catch (const TimeoutError& e) {
        std::cout << "Timeout (code " << e.code() << "): " << e.what() << '\n';
    }
    catch (const NetworkError& e) {
        std::cout << "Network (code " << e.code() << "): " << e.what() << '\n';
    }
}


// ═══════════════════════════════════════════════════════════════
//  4. noexcept — promise no exceptions
// ═══════════════════════════════════════════════════════════════

/*
 *  noexcept tells the compiler this function won't throw.
 *  Benefits:
 *    - Compiler optimizations (no exception handling overhead)
 *    - std::vector uses move if move ctor is noexcept
 *    - If it DOES throw → std::terminate() (program crash)
 *
 *  RULE: Destructors are implicitly noexcept.
 *        Move constructors/assignment SHOULD be noexcept.
 *        Swap functions SHOULD be noexcept.
 */

class Buffer {
    std::unique_ptr<char[]> data_;
    size_t size_;
public:
    Buffer(size_t size) : data_(std::make_unique<char[]>(size)), size_(size) {
        std::memset(data_.get(), 0, size);
    }

    // Move operations — noexcept enables vector optimization
    Buffer(Buffer&& other) noexcept
        : data_(std::move(other.data_)), size_(other.size_) {
        other.size_ = 0;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        data_ = std::move(other.data_);
        size_ = other.size_;
        other.size_ = 0;
        return *this;
    }

    // Swap — noexcept
    friend void swap(Buffer& a, Buffer& b) noexcept {
        using std::swap;
        swap(a.data_, b.data_);
        swap(a.size_, b.size_);
    }

    size_t size() const noexcept { return size_; }
};

void demo_noexcept() {
    std::cout << "\n═══ noexcept ═══\n";

    // Check at compile time
    std::cout << "Buffer move ctor noexcept? "
              << std::is_nothrow_move_constructible_v<Buffer> << '\n';
    std::cout << "Buffer move assign noexcept? "
              << std::is_nothrow_move_assignable_v<Buffer> << '\n';

    // Vector uses move when noexcept (faster reallocation!)
    std::vector<Buffer> buffers;
    buffers.reserve(2);
    buffers.emplace_back(1024);
    buffers.emplace_back(2048);
    std::cout << "Vector holds " << buffers.size() << " buffers\n";
}


// ═══════════════════════════════════════════════════════════════
//  5. RAII for exception safety
// ═══════════════════════════════════════════════════════════════

/*
 *  RAII = Resource Acquisition Is Initialization
 *
 *  Key insight: Destructors ALWAYS run when scope exits,
 *  even during stack unwinding from an exception.
 *
 *  ┌───────────────────────────────────────┐
 *  │ try {                                  │
 *  │   Lock lock(mutex);    ← acquired     │
 *  │   File f("data.txt"); ← opened        │
 *  │   doWork();            ← might throw!  │
 *  │ }                                      │
 *  │ ← ~File() runs (closes file)           │
 *  │ ← ~Lock() runs (unlocks mutex)         │
 *  │ (even if doWork() threw!)              │
 *  └───────────────────────────────────────┘
 */

class DatabaseConnection {
    std::string name_;
    bool connected_ = false;
public:
    DatabaseConnection(const std::string& name) : name_(name) {
        std::cout << "  DB: Connected to " << name_ << '\n';
        connected_ = true;
    }
    ~DatabaseConnection() {
        if (connected_) {
            std::cout << "  DB: Disconnected from " << name_ << '\n';
            connected_ = false;
        }
    }
    void query(const std::string& q) {
        if (q == "BAD") throw std::runtime_error("Query failed: " + q);
        std::cout << "  DB: Executed '" << q << "'\n";
    }
};

void demo_raii_safety() {
    std::cout << "\n═══ RAII EXCEPTION SAFETY ═══\n";

    try {
        DatabaseConnection db("production");
        db.query("SELECT * FROM users");
        db.query("BAD");  // throws!
        db.query("This never runs");
    }
    catch (const std::exception& e) {
        std::cout << "  Caught: " << e.what() << '\n';
    }
    // Note: DB destructor ran even though exception was thrown!
}


// ═══════════════════════════════════════════════════════════════
//  6. Copy-and-swap idiom (strong guarantee)
// ═══════════════════════════════════════════════════════════════

class StrongBuffer {
    std::unique_ptr<int[]> data_;
    size_t size_;
public:
    StrongBuffer(size_t size) : data_(std::make_unique<int[]>(size)), size_(size) {}

    StrongBuffer(const StrongBuffer& other)
        : data_(std::make_unique<int[]>(other.size_)), size_(other.size_) {
        std::copy(other.data_.get(), other.data_.get() + size_, data_.get());
    }

    StrongBuffer(StrongBuffer&&) noexcept = default;

    // Copy-and-swap: STRONG exception guarantee
    // If copy fails → original object is untouched
    StrongBuffer& operator=(StrongBuffer other) noexcept {  // copy made here
        swap(*this, other);  // noexcept swap
        return *this;        // old data destroyed via other's destructor
    }

    friend void swap(StrongBuffer& a, StrongBuffer& b) noexcept {
        using std::swap;
        swap(a.data_, b.data_);
        swap(a.size_, b.size_);
    }

    size_t size() const noexcept { return size_; }
};

void demo_copy_swap() {
    std::cout << "\n═══ COPY-AND-SWAP (Strong Guarantee) ═══\n";

    StrongBuffer a(100);
    StrongBuffer b(200);
    a = b;  // If copy throws, a is unchanged
    std::cout << "After assignment: a.size=" << a.size() << ", b.size=" << b.size() << '\n';
}


int main() {
    demo_basic();
    demo_standard_exceptions();
    demo_custom_exceptions();
    demo_noexcept();
    demo_raii_safety();
    demo_copy_swap();
    return 0;
}
