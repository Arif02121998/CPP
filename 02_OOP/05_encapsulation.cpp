/*******************************************************************************
 * 05_encapsulation.cpp — Data Hiding and Access Control
 *******************************************************************************
 *
 *  Encapsulation = Bundling data + methods + controlling access
 *
 *  ┌─────────────────────────────────────────────────┐
 *  │                  BankAccount                     │
 *  │─────────────── private ─────────────────────────│
 *  │  balance_        ← Hidden from outside           │
 *  │  account_number_ ← Only class can access         │
 *  │─────────────── public ──────────────────────────│
 *  │  deposit(amt)    ← Controlled entry point        │
 *  │  withdraw(amt)   ← With validation               │
 *  │  getBalance()    ← Read-only access              │
 *  └─────────────────────────────────────────────────┘
 *
 *  WHY? → Protects data integrity, hides implementation details
 *
 * COMPILE: g++ -std=c++17 -o encapsulation 05_encapsulation.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <stdexcept>

class BankAccount {
private:
    std::string owner_;
    double balance_;

    // Private helper — internal use only
    void log(const std::string& action, double amount) const {
        std::cout << "  [LOG] " << action << " $" << amount
                  << " | Balance: $" << balance_ << '\n';
    }

public:
    BankAccount(const std::string& owner, double initial = 0.0)
        : owner_(owner), balance_(initial)
    {
        if (initial < 0) {
            throw std::invalid_argument("Initial balance cannot be negative");
        }
    }

    // Getter (read-only access)
    double getBalance() const { return balance_; }
    std::string getOwner() const { return owner_; }

    // Controlled modification with validation
    void deposit(double amount) {
        if (amount <= 0) {
            throw std::invalid_argument("Deposit amount must be positive");
        }
        balance_ += amount;
        log("Deposit", amount);
    }

    bool withdraw(double amount) {
        if (amount <= 0 || amount > balance_) {
            std::cout << "  [DENIED] Withdrawal of $" << amount << '\n';
            return false;
        }
        balance_ -= amount;
        log("Withdraw", amount);
        return true;
    }
};

// ═════════════════════════════════════════════════════════════════
// FRIEND FUNCTIONS AND CLASSES
// ═════════════════════════════════════════════════════════════════

class SecureVault {
    int secret_code_;
    std::string contents_;

    // Friend function — can access private members
    friend void adminAccess(const SecureVault& vault);

    // Friend class — all its methods can access private members
    friend class SecurityAuditor;

public:
    SecureVault(int code, const std::string& contents)
        : secret_code_(code), contents_(contents) {}
};

void adminAccess(const SecureVault& vault) {
    // Can access private members because it's a friend
    std::cout << "  Admin sees: code=" << vault.secret_code_
              << ", contents=" << vault.contents_ << '\n';
}

class SecurityAuditor {
public:
    void audit(const SecureVault& vault) {
        std::cout << "  Audit: code=" << vault.secret_code_ << '\n';
    }
};

int main() {
    std::cout << "=== ENCAPSULATION ===\n";
    BankAccount acc("Alice", 1000);
    acc.deposit(500);
    acc.withdraw(200);
    acc.withdraw(5000);  // Denied
    std::cout << "Final balance: $" << acc.getBalance() << '\n';

    std::cout << "\n=== FRIEND ACCESS ===\n";
    SecureVault vault(1234, "Gold bars");
    adminAccess(vault);
    SecurityAuditor auditor;
    auditor.audit(vault);

    return 0;
}
