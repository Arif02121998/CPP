/*******************************************************************************
 * 02_class_templates.cpp — Generic Classes
 *******************************************************************************
 *
 *  template<typename T>
 *  class Stack {
 *      T data[100];     // T is replaced with actual type
 *      ...
 *  };
 *
 *  Stack<int> intStack;       → T = int
 *  Stack<string> strStack;    → T = string
 *  Stack<double> dblStack;    → T = double
 *
 * COMPILE: g++ -std=c++17 -o class_templates 02_class_templates.cpp
 ******************************************************************************/

#include <iostream>
#include <stdexcept>
#include <string>

// ═════════════════════════════════════════════════════════════════
// 1. BASIC CLASS TEMPLATE — Stack
// ═════════════════════════════════════════════════════════════════

template<typename T, int MaxSize = 100>
class Stack {
    T data_[MaxSize];
    int top_;

public:
    Stack() : top_(-1) {}

    void push(const T& value) {
        if (top_ >= MaxSize - 1) throw std::overflow_error("Stack full");
        data_[++top_] = value;
    }

    T pop() {
        if (top_ < 0) throw std::underflow_error("Stack empty");
        return data_[top_--];
    }

    const T& peek() const {
        if (top_ < 0) throw std::underflow_error("Stack empty");
        return data_[top_];
    }

    bool empty() const { return top_ < 0; }
    int size() const { return top_ + 1; }
};

// ═════════════════════════════════════════════════════════════════
// 2. CLASS TEMPLATE — Pair (like std::pair)
// ═════════════════════════════════════════════════════════════════

template<typename T1, typename T2>
class Pair {
public:
    T1 first;
    T2 second;

    Pair(const T1& f, const T2& s) : first(f), second(s) {}

    void print() const {
        std::cout << "(" << first << ", " << second << ")";
    }
};

// Template deduction guide (C++17)
template<typename T1, typename T2>
Pair(T1, T2) -> Pair<T1, T2>;

// ═════════════════════════════════════════════════════════════════
// 3. CLASS TEMPLATE WITH MEMBER TEMPLATES
// ═════════════════════════════════════════════════════════════════

template<typename T>
class Container {
    T value_;
public:
    Container(const T& val) : value_(val) {}

    // Member function template
    template<typename U>
    bool isEqual(const Container<U>& other) const {
        return value_ == static_cast<T>(other.getValue());
    }

    T getValue() const { return value_; }

    friend std::ostream& operator<<(std::ostream& os, const Container<T>& c) {
        os << c.value_;
        return os;
    }
};

int main() {
    std::cout << "=== STACK ===\n";
    Stack<int> intStack;
    intStack.push(10);
    intStack.push(20);
    intStack.push(30);
    std::cout << "Top: " << intStack.peek() << '\n';
    std::cout << "Pop: " << intStack.pop() << '\n';
    std::cout << "Size: " << intStack.size() << '\n';

    Stack<std::string, 5> strStack;
    strStack.push("Hello");
    strStack.push("World");
    std::cout << "String top: " << strStack.peek() << '\n';

    std::cout << "\n=== PAIR ===\n";
    Pair<std::string, int> person("Alice", 30);
    person.print();
    std::cout << '\n';

    // C++17 CTAD (Class Template Argument Deduction)
    Pair p2(3.14, 42);  // Deduced as Pair<double, int>
    p2.print();
    std::cout << '\n';

    std::cout << "\n=== CONTAINER ===\n";
    Container<int> c1(42);
    Container<double> c2(42.0);
    std::cout << "c1 = " << c1 << ", c2 = " << c2 << '\n';

    return 0;
}
