/*******************************************************************************
 * 05_other_containers.cpp — deque, list, stack, queue, priority_queue
 *******************************************************************************
 * COMPILE: g++ -std=c++17 -o containers 05_other_containers.cpp
 ******************************************************************************/

#include <iostream>
#include <deque>
#include <list>
#include <stack>
#include <queue>
#include <array>

int main() {
    // ═══════════════ std::array (fixed size) ═══════════════
    std::cout << "=== std::array ===\n";
    std::array<int, 5> arr = {10, 20, 30, 40, 50};
    std::cout << "Size: " << arr.size() << ", arr[2] = " << arr[2] << '\n';

    // ═══════════════ std::deque (double-ended queue) ═══════════════
    std::cout << "\n=== std::deque ===\n";
    std::deque<int> dq = {2, 3, 4};
    dq.push_front(1);  // O(1) at front!
    dq.push_back(5);   // O(1) at back
    for (int x : dq) std::cout << x << " ";
    std::cout << '\n';

    // ═══════════════ std::list (doubly-linked list) ═══════════════
    std::cout << "\n=== std::list ===\n";
    std::list<int> lst = {1, 3, 5};
    lst.push_front(0);
    lst.push_back(7);
    auto it = lst.begin();
    std::advance(it, 2);
    lst.insert(it, 2);  // Insert at position: O(1) if iterator known!

    for (int x : lst) std::cout << x << " ";
    std::cout << '\n';

    lst.sort();
    lst.reverse();
    std::cout << "Sorted desc: ";
    for (int x : lst) std::cout << x << " ";
    std::cout << '\n';

    // ═══════════════ std::stack (LIFO) ═══════════════
    std::cout << "\n=== std::stack (LIFO) ===\n";
    /*
     *   push → │ 30 │ ← top (last in, first out)
     *          │ 20 │
     *          │ 10 │
     *          └────┘
     */
    std::stack<int> stk;
    stk.push(10);
    stk.push(20);
    stk.push(30);
    std::cout << "Top: " << stk.top() << '\n';
    stk.pop();
    std::cout << "After pop, top: " << stk.top() << '\n';

    // ═══════════════ std::queue (FIFO) ═══════════════
    std::cout << "\n=== std::queue (FIFO) ===\n";
    /*
     *   push → │ 30 │ 20 │ 10 │ → pop (first in, first out)
     *          back          front
     */
    std::queue<int> q;
    q.push(10);
    q.push(20);
    q.push(30);
    std::cout << "Front: " << q.front() << ", Back: " << q.back() << '\n';
    q.pop();
    std::cout << "After pop, front: " << q.front() << '\n';

    // ═══════════════ std::priority_queue (max-heap) ═══════════════
    std::cout << "\n=== std::priority_queue (max-heap) ===\n";
    std::priority_queue<int> pq;
    pq.push(30);
    pq.push(10);
    pq.push(50);
    pq.push(20);

    std::cout << "Pop order (largest first): ";
    while (!pq.empty()) {
        std::cout << pq.top() << " ";
        pq.pop();
    }
    std::cout << '\n';

    // Min-heap:
    std::priority_queue<int, std::vector<int>, std::greater<int>> minPQ;
    minPQ.push(30);
    minPQ.push(10);
    minPQ.push(50);
    std::cout << "Min-heap top: " << minPQ.top() << '\n';

    return 0;
}
