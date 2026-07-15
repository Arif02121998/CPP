/*******************************************************************************
 * 03_smart_pointers.cpp — unique_ptr, shared_ptr, weak_ptr
 *******************************************************************************
 *
 *  Smart pointers = RAII wrappers for dynamic memory
 *  They automatically delete memory when no longer needed!
 *
 *  ┌─────────────────────────────────────────────────────────────────┐
 *  │  SMART POINTER      │ OWNERSHIP     │ USE CASE                 │
 *  ├─────────────────────┼───────────────┼──────────────────────────┤
 *  │  unique_ptr<T>      │ EXCLUSIVE     │ Single owner, default    │
 *  │  shared_ptr<T>      │ SHARED        │ Multiple owners          │
 *  │  weak_ptr<T>        │ OBSERVER      │ Break circular refs      │
 *  └─────────────────────┴───────────────┴──────────────────────────┘
 *
 *  unique_ptr:                 shared_ptr:
 *  ┌──────────┐ owns          ┌──────────┐
 *  │unique_ptr│──────►[data]  │shared_ptr│──┐
 *  └──────────┘               └──────────┘  │
 *  (only ONE owner)           ┌──────────┐  ├──►[data] + [ref_count: 2]
 *                             │shared_ptr│──┘
 *                             └──────────┘
 *                             (multiple owners, last one deletes)
 *
 * COMPILE: g++ -std=c++17 -o smart_ptr 03_smart_pointers.cpp
 ******************************************************************************/

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Widget {
    std::string name_;
public:
    Widget(const std::string& n) : name_(n) {
        std::cout << "  Widget '" << name_ << "' created\n";
    }
    ~Widget() {
        std::cout << "  Widget '" << name_ << "' destroyed\n";
    }
    void doWork() const { std::cout << "  " << name_ << " doing work\n"; }
};

int main() {

    // ═════════════════════════════════════════════════════════════════
    // 1. UNIQUE_PTR — Exclusive ownership
    // ═════════════════════════════════════════════════════════════════
    std::cout << "=== UNIQUE_PTR ===\n";
    {
        // Create with make_unique (preferred — exception safe!)
        auto p1 = std::make_unique<Widget>("UniqueA");
        p1->doWork();

        // Cannot copy unique_ptr!
        // auto p2 = p1;  // ERROR! unique_ptr is non-copyable

        // But CAN move it (transfer ownership)
        auto p2 = std::move(p1);  // p1 is now nullptr
        p2->doWork();

        if (!p1) std::cout << "  p1 is null after move\n";

        // Release: give up ownership and get raw pointer
        Widget* raw = p2.release();  // p2 is now null
        delete raw;  // Must manually delete!

        // Reset: destroy current and optionally take new
        auto p3 = std::make_unique<Widget>("UniqueB");
        p3.reset();  // Destroys UniqueB, p3 is null

        std::cout << "  --- scope end ---\n";
    }

    // ═════════════════════════════════════════════════════════════════
    // 2. UNIQUE_PTR WITH ARRAYS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== UNIQUE_PTR ARRAY ===\n";
    {
        auto arr = std::make_unique<int[]>(5);
        for (int i = 0; i < 5; i++) arr[i] = i * 10;
        for (int i = 0; i < 5; i++) std::cout << arr[i] << " ";
        std::cout << '\n';
    }

    // ═════════════════════════════════════════════════════════════════
    // 3. SHARED_PTR — Shared ownership (reference counted)
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== SHARED_PTR ===\n";
    {
        auto sp1 = std::make_shared<Widget>("SharedA");
        std::cout << "  ref count: " << sp1.use_count() << '\n';  // 1

        {
            auto sp2 = sp1;  // Copy — both own the widget
            std::cout << "  ref count: " << sp1.use_count() << '\n';  // 2
            sp2->doWork();

            auto sp3 = sp1;  // Another copy
            std::cout << "  ref count: " << sp1.use_count() << '\n';  // 3
        }
        // sp2 and sp3 destroyed, ref count drops
        std::cout << "  ref count: " << sp1.use_count() << '\n';  // 1
    }
    // sp1 destroyed, ref count = 0 → Widget deleted

    // ═════════════════════════════════════════════════════════════════
    // 4. WEAK_PTR — Non-owning observer (breaks circular references)
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== WEAK_PTR ===\n";
    {
        std::weak_ptr<Widget> weak;

        {
            auto shared = std::make_shared<Widget>("WeakTest");
            weak = shared;  // weak observes but doesn't own

            // To use weak_ptr, must lock() to get shared_ptr
            if (auto locked = weak.lock()) {
                std::cout << "  Weak ptr valid, ref count: " << locked.use_count() << '\n';
                locked->doWork();
            }
        }
        // shared destroyed, Widget deleted

        // weak_ptr knows the object is gone!
        if (weak.expired()) {
            std::cout << "  Weak ptr expired — object is gone\n";
        }

        if (auto locked = weak.lock()) {
            locked->doWork();
        } else {
            std::cout << "  Cannot lock — object destroyed\n";
        }
    }

    /*
     * CIRCULAR REFERENCE PROBLEM (solved by weak_ptr):
     *
     *  shared_ptr<A> ──────────► A
     *                            │
     *                    shared_ptr<B> member
     *                            │
     *                            ▼
     *  shared_ptr<B> ──────────► B
     *                            │
     *                    shared_ptr<A> member  ← CIRCULAR!
     *                            │              Both ref counts
     *                            ▼              stay at 1 forever
     *                          ► A              = MEMORY LEAK
     *
     * FIX: Make one of the pointers a weak_ptr<>
     */

    // ═════════════════════════════════════════════════════════════════
    // 5. SMART POINTERS IN CONTAINERS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== SMART PTRS IN CONTAINERS ===\n";
    {
        std::vector<std::unique_ptr<Widget>> widgets;
        widgets.push_back(std::make_unique<Widget>("W1"));
        widgets.push_back(std::make_unique<Widget>("W2"));
        widgets.push_back(std::make_unique<Widget>("W3"));

        for (const auto& w : widgets) {
            w->doWork();
        }
        std::cout << "  --- clearing vector ---\n";
    }

    /*
     * SUMMARY — Which smart pointer to use?
     *
     * unique_ptr: DEFAULT CHOICE. Single owner. Zero overhead.
     * shared_ptr: Only when ownership is truly shared (rare).
     * weak_ptr:   Only to break circular references with shared_ptr.
     * raw pointer: Only for non-owning, optional observation.
     */

    return 0;
}
