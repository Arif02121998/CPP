/*******************************************************************************
 * 05_filesystem.cpp — std::filesystem for file & directory operations
 *******************************************************************************
 *
 *  C++17 finally brings a portable filesystem library!
 *
 *   ┌──────────────────────────────────────────────────────────────────┐
 *   │                     std::filesystem                              │
 *   ├──────────────────┬───────────────────────────────────────────────┤
 *   │ path             │ Platform-independent file path                │
 *   │ directory_entry  │ Info about a single file/directory            │
 *   │ directory_iter   │ Iterate contents of a directory               │
 *   │ file operations  │ copy, rename, remove, create_directory        │
 *   │ file info        │ file_size, last_write_time, permissions       │
 *   └──────────────────┴───────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o filesystem 05_filesystem.cpp
 *          (Some older compilers need: -lstdc++fs)
 ******************************************************************************/

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

// ═══════════════════════════════════════════════════════════════
//  1. path — Platform-independent path manipulation
// ═══════════════════════════════════════════════════════════════

void demo_path() {
    std::cout << "═══ fs::path ═══\n";

    fs::path p = "/home/user/documents/report.txt";

    std::cout << "full:       " << p << '\n';
    std::cout << "root:       " << p.root_path() << '\n';
    std::cout << "parent:     " << p.parent_path() << '\n';
    std::cout << "filename:   " << p.filename() << '\n';
    std::cout << "stem:       " << p.stem() << '\n';       // "report"
    std::cout << "extension:  " << p.extension() << '\n';  // ".txt"

    // Building paths with / operator
    fs::path dir = "/home/user";
    fs::path file = dir / "projects" / "main.cpp";
    std::cout << "built path: " << file << '\n';

    // Replacing parts
    fs::path p2 = p;
    p2.replace_extension(".pdf");
    std::cout << "new ext:    " << p2 << '\n';
    p2.replace_filename("summary.pdf");
    std::cout << "new name:   " << p2 << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  2. Querying file status
// ═══════════════════════════════════════════════════════════════

void demo_status() {
    std::cout << "\n═══ FILE STATUS ═══\n";

    fs::path p = "/tmp";
    if (fs::exists(p)) {
        std::cout << p << " exists\n";
        std::cout << "  is_directory:     " << fs::is_directory(p) << '\n';
        std::cout << "  is_regular_file:  " << fs::is_regular_file(p) << '\n';
        std::cout << "  is_symlink:       " << fs::is_symlink(p) << '\n';
    }

    // Check current executable
    fs::path self = "/proc/self/exe";
    if (fs::exists(self)) {
        std::cout << "Self is symlink: " << fs::is_symlink(self) << '\n';
    }
}


// ═══════════════════════════════════════════════════════════════
//  3. Creating and removing directories/files
// ═══════════════════════════════════════════════════════════════

void demo_create_remove() {
    std::cout << "\n═══ CREATE & REMOVE ═══\n";

    fs::path testDir = "/tmp/cpp17_fs_demo";

    // Create directories (including parents)
    fs::create_directories(testDir / "sub1" / "sub2");
    std::cout << "Created: " << testDir / "sub1" / "sub2" << '\n';

    // Create a test file
    {
        std::ofstream out(testDir / "test.txt");
        out << "Hello from C++17 filesystem!\n";
    }
    std::cout << "Created test.txt, size: "
              << fs::file_size(testDir / "test.txt") << " bytes\n";

    // Copy the file
    fs::copy_file(testDir / "test.txt", testDir / "test_copy.txt",
                  fs::copy_options::overwrite_existing);
    std::cout << "Copied to test_copy.txt\n";

    // Rename
    fs::rename(testDir / "test_copy.txt", testDir / "renamed.txt");
    std::cout << "Renamed to renamed.txt\n";

    // Remove single file
    fs::remove(testDir / "renamed.txt");
    std::cout << "Removed renamed.txt\n";

    // Remove directory tree recursively
    auto count = fs::remove_all(testDir);
    std::cout << "remove_all removed " << count << " items\n";
}


// ═══════════════════════════════════════════════════════════════
//  4. Iterating directories
// ═══════════════════════════════════════════════════════════════

void demo_iterate() {
    std::cout << "\n═══ DIRECTORY ITERATION ═══\n";

    // Non-recursive iteration of /tmp (first 10 entries)
    std::cout << "--- /tmp (first 10 entries) ---\n";
    int count = 0;
    for (const auto& entry : fs::directory_iterator("/tmp")) {
        std::cout << "  " << entry.path().filename();
        if (entry.is_directory()) std::cout << "/";
        std::cout << '\n';
        if (++count >= 10) {
            std::cout << "  ... (truncated)\n";
            break;
        }
    }

    // Recursive iteration example (with depth tracking)
    // fs::recursive_directory_iterator iterates ALL subdirectories
    //
    // for (const auto& entry : fs::recursive_directory_iterator("/some/path")) {
    //     std::cout << entry.path() << '\n';
    // }
    std::cout << "(Recursive iteration available via recursive_directory_iterator)\n";
}


// ═══════════════════════════════════════════════════════════════
//  5. Space info
// ═══════════════════════════════════════════════════════════════

void demo_space() {
    std::cout << "\n═══ DISK SPACE ═══\n";

    auto info = fs::space("/");
    auto toGB = [](uintmax_t bytes) { return static_cast<double>(bytes) / (1024*1024*1024); };

    std::cout << "Root filesystem:\n";
    std::cout << "  Capacity:  " << toGB(info.capacity)  << " GB\n";
    std::cout << "  Free:      " << toGB(info.free)      << " GB\n";
    std::cout << "  Available: " << toGB(info.available)  << " GB\n";
}


// ═══════════════════════════════════════════════════════════════
//  6. Practical: find files by extension
// ═══════════════════════════════════════════════════════════════

void findByExtension(const fs::path& dir, const std::string& ext) {
    std::cout << "\n═══ FIND *" << ext << " in " << dir << " ═══\n";
    int count = 0;
    std::error_code ec;
    for (const auto& entry : fs::recursive_directory_iterator(dir, ec)) {
        if (entry.is_regular_file() && entry.path().extension() == ext) {
            std::cout << "  " << entry.path() << "  ("
                      << entry.file_size() << " bytes)\n";
            if (++count >= 10) {
                std::cout << "  ... (truncated)\n";
                break;
            }
        }
    }
    if (count == 0) std::cout << "  (none found)\n";
}


int main() {
    demo_path();
    demo_status();
    demo_create_remove();
    demo_iterate();
    demo_space();

    // Find .txt files in /tmp as example
    findByExtension("/tmp", ".txt");

    return 0;
}
