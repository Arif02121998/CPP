/*******************************************************************************
 * 04_raii.cpp — Resource Acquisition Is Initialization
 *******************************************************************************
 *
 *  RAII PRINCIPLE:
 *  ──────────────
 *  Acquire resource in CONSTRUCTOR → Release resource in DESTRUCTOR
 *  The C++ scope and destructor system guarantees cleanup.
 *
 *  ┌─────────────────────────────────────────────────────────┐
 *  │  {                                                       │
 *  │      FileGuard f("data.txt");  // Constructor opens file │
 *  │      f.write("hello");         // Use the resource       │
 *  │                                                          │
 *  │  }  // Destructor closes file — GUARANTEED, even if      │
 *  │     // exception is thrown!                               │
 *  └─────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o raii 04_raii.cpp
 ******************************************************************************/

#include <iostream>
#include <fstream>
#include <mutex>
#include <memory>

// ═════════════════════════════════════════════════════════════════
// 1. RAII FOR FILE HANDLING
// ═════════════════════════════════════════════════════════════════

class FileGuard {
    std::ofstream file_;
public:
    FileGuard(const std::string& filename) : file_(filename) {
        if (!file_.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        std::cout << "  File opened: " << filename << '\n';
    }

    void write(const std::string& data) {
        file_ << data << '\n';
    }

    ~FileGuard() {
        if (file_.is_open()) {
            file_.close();
            std::cout << "  File closed (RAII destructor)\n";
        }
    }

    // Non-copyable
    FileGuard(const FileGuard&) = delete;
    FileGuard& operator=(const FileGuard&) = delete;
};

// ═════════════════════════════════════════════════════════════════
// 2. RAII FOR MUTEX LOCKING (like std::lock_guard)
// ═════════════════════════════════════════════════════════════════

class LockGuard {
    std::mutex& mutex_;
public:
    explicit LockGuard(std::mutex& m) : mutex_(m) {
        mutex_.lock();
        std::cout << "  Mutex locked\n";
    }
    ~LockGuard() {
        mutex_.unlock();
        std::cout << "  Mutex unlocked (RAII)\n";
    }
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;
};

int main() {
    std::cout << "=== RAII FILE GUARD ===\n";
    {
        FileGuard file("/tmp/raii_test.txt");
        file.write("Hello from RAII!");
        file.write("Resource managed automatically.");
        // File closes when FileGuard goes out of scope
    }

    std::cout << "\n=== RAII LOCK GUARD ===\n";
    std::mutex mtx;
    {
        LockGuard lock(mtx);
        std::cout << "  (doing work with lock held)\n";
        // Mutex unlocks when LockGuard goes out of scope
    }

    std::cout << "\n=== SMART POINTERS = RAII ===\n";
    {
        auto ptr = std::make_unique<int>(42);
        std::cout << "  Value: " << *ptr << '\n';
        // Memory freed when unique_ptr goes out of scope
    }
    std::cout << "  Memory freed automatically\n";

    /*
     * RAII in the Standard Library:
     * ─────────────────────────────
     * std::unique_ptr   — RAII for dynamic memory
     * std::shared_ptr   — RAII for shared ownership
     * std::lock_guard   — RAII for mutexes
     * std::fstream      — RAII for files
     * std::string       — RAII for character arrays
     * std::vector       — RAII for dynamic arrays
     */

    return 0;
}
