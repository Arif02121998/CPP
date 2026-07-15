/*******************************************************************************
 * 07_operator_overloading.cpp — Custom Operator Behavior for Your Classes
 *******************************************************************************
 *
 *  ┌────────────────────────────────────────────────────────────────┐
 *  │  OVERLOADABLE             │  NOT OVERLOADABLE                  │
 *  ├───────────────────────────┼────────────────────────────────────┤
 *  │  +  -  *  /  %           │  .   (member access)               │
 *  │  ==  !=  <  >  <=  >=   │  .*  (member pointer access)       │
 *  │  <<  >>  &  |  ^  ~     │  ::  (scope resolution)            │
 *  │  ++  --  !               │  ?:  (ternary)                     │
 *  │  =  +=  -=  *=  /=      │  sizeof  typeid                    │
 *  │  []  ()  ->  ->*        │                                     │
 *  │  new  delete             │                                     │
 *  └───────────────────────────┴────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o operators 07_operator_overloading.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <cmath>

class Vector2D {
    double x_, y_;

public:
    Vector2D(double x = 0, double y = 0) : x_(x), y_(y) {}

    double getX() const { return x_; }
    double getY() const { return y_; }
    double magnitude() const { return std::sqrt(x_ * x_ + y_ * y_); }

    // ─── ARITHMETIC OPERATORS (member functions) ───
    Vector2D operator+(const Vector2D& rhs) const {
        return Vector2D(x_ + rhs.x_, y_ + rhs.y_);
    }

    Vector2D operator-(const Vector2D& rhs) const {
        return Vector2D(x_ - rhs.x_, y_ - rhs.y_);
    }

    Vector2D operator*(double scalar) const {
        return Vector2D(x_ * scalar, y_ * scalar);
    }

    // ─── COMPOUND ASSIGNMENT ───
    Vector2D& operator+=(const Vector2D& rhs) {
        x_ += rhs.x_;
        y_ += rhs.y_;
        return *this;
    }

    // ─── COMPARISON OPERATORS ───
    bool operator==(const Vector2D& rhs) const {
        return x_ == rhs.x_ && y_ == rhs.y_;
    }

    bool operator!=(const Vector2D& rhs) const {
        return !(*this == rhs);
    }

    // ─── UNARY OPERATORS ───
    Vector2D operator-() const {  // Negate
        return Vector2D(-x_, -y_);
    }

    // ─── PREFIX/POSTFIX INCREMENT ───
    Vector2D& operator++() {     // Prefix ++v
        ++x_; ++y_;
        return *this;
    }

    Vector2D operator++(int) {   // Postfix v++ (dummy int parameter)
        Vector2D temp = *this;
        ++x_; ++y_;
        return temp;
    }

    // ─── STREAM OUTPUT (friend function — needs access to private members) ───
    friend std::ostream& operator<<(std::ostream& os, const Vector2D& v) {
        os << "(" << v.x_ << ", " << v.y_ << ")";
        return os;
    }

    // ─── SCALAR * VECTOR (non-member friend for commutative: 3 * vec) ───
    friend Vector2D operator*(double scalar, const Vector2D& v) {
        return v * scalar;
    }
};

// ═════════════════════════════════════════════════════════════════
// CLASS WITH [] AND () OPERATORS
// ═════════════════════════════════════════════════════════════════

class Matrix2x2 {
    double data_[2][2];

public:
    Matrix2x2(double a, double b, double c, double d) {
        data_[0][0] = a; data_[0][1] = b;
        data_[1][0] = c; data_[1][1] = d;
    }

    // ─── SUBSCRIPT OPERATOR [] ───
    double* operator[](int row) {
        return data_[row];
    }

    const double* operator[](int row) const {
        return data_[row];
    }

    // ─── FUNCTION CALL OPERATOR () ───
    // Makes object callable like a function (functor)
    double operator()(int row, int col) const {
        return data_[row][col];
    }

    friend std::ostream& operator<<(std::ostream& os, const Matrix2x2& m) {
        os << "|" << m.data_[0][0] << " " << m.data_[0][1] << "|\n"
           << "|" << m.data_[1][0] << " " << m.data_[1][1] << "|";
        return os;
    }
};

int main() {
    std::cout << "=== VECTOR2D OPERATORS ===\n";
    Vector2D v1(3, 4), v2(1, 2);

    std::cout << "v1 = " << v1 << '\n';
    std::cout << "v2 = " << v2 << '\n';
    std::cout << "v1 + v2 = " << (v1 + v2) << '\n';
    std::cout << "v1 - v2 = " << (v1 - v2) << '\n';
    std::cout << "v1 * 3  = " << (v1 * 3) << '\n';
    std::cout << "3 * v1  = " << (3 * v1) << '\n';
    std::cout << "-v1     = " << (-v1) << '\n';
    std::cout << "|v1|    = " << v1.magnitude() << '\n';
    std::cout << "v1 == v2? " << (v1 == v2) << '\n';

    v1 += v2;
    std::cout << "v1 += v2 → " << v1 << '\n';

    std::cout << "++v1 = " << (++v1) << '\n';

    std::cout << "\n=== MATRIX OPERATORS ===\n";
    Matrix2x2 m(1, 2, 3, 4);
    std::cout << m << '\n';
    std::cout << "m[0][1] = " << m[0][1] << '\n';
    std::cout << "m(1,0)  = " << m(1, 0) << '\n';

    return 0;
}
