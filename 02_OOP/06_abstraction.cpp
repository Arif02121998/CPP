/*******************************************************************************
 * 06_abstraction.cpp — Abstract Classes, Pure Virtual Functions, Interfaces
 *******************************************************************************
 *
 *  Abstraction = Define WHAT an object does, not HOW it does it
 *
 *  ┌──────────────────────────────────────────────────────────────┐
 *  │          ABSTRACT CLASS (has at least one pure virtual)      │
 *  │  ┌─────────────────────────────────────────────────────┐    │
 *  │  │  class Shape {                                       │    │
 *  │  │      virtual double area() const = 0;   ← PURE      │    │
 *  │  │      virtual double perimeter() const = 0;           │    │
 *  │  │      void print() { ... }  ← Can have regular methods│   │
 *  │  │  };                                                   │    │
 *  │  └─────────────────────────────────────────────────────┘    │
 *  │                                                              │
 *  │  Cannot instantiate:  Shape s;  // ERROR!                    │
 *  │  Can use as pointer:  Shape* s = new Circle(5);  // OK!     │
 *  └──────────────────────────────────────────────────────────────┘
 *
 *  INTERFACE = Abstract class with ONLY pure virtual functions
 *  (C++ doesn't have a keyword for interface — it's a pattern)
 *
 * COMPILE: g++ -std=c++17 -o abstraction 06_abstraction.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// ═════════════════════════════════════════════════════════════════
// 1. INTERFACE (all pure virtual)
// ═════════════════════════════════════════════════════════════════

class ILogger {
public:
    virtual void log(const std::string& message) = 0;
    virtual void setLevel(int level) = 0;
    virtual ~ILogger() = default;
};

class ConsoleLogger : public ILogger {
    int level_ = 0;
public:
    void log(const std::string& message) override {
        std::cout << "[CONSOLE] " << message << '\n';
    }
    void setLevel(int level) override { level_ = level; }
};

class FileLogger : public ILogger {
    int level_ = 0;
public:
    void log(const std::string& message) override {
        std::cout << "[FILE] Would write: " << message << '\n';
    }
    void setLevel(int level) override { level_ = level; }
};

// ═════════════════════════════════════════════════════════════════
// 2. ABSTRACT CLASS (mix of pure virtual + implemented methods)
// ═════════════════════════════════════════════════════════════════

class Database {
public:
    virtual ~Database() = default;

    // Pure virtual — derived classes MUST implement these
    virtual bool connect(const std::string& connection_string) = 0;
    virtual void disconnect() = 0;
    virtual bool execute(const std::string& query) = 0;

    // Template Method pattern — uses pure virtual functions
    bool runQuery(const std::string& query) {
        std::cout << "  Preparing query...\n";
        bool result = execute(query);  // Calls derived version
        std::cout << "  Query completed.\n";
        return result;
    }
};

class SQLiteDB : public Database {
public:
    bool connect(const std::string& cs) override {
        std::cout << "  SQLite: Connected to " << cs << '\n';
        return true;
    }
    void disconnect() override {
        std::cout << "  SQLite: Disconnected\n";
    }
    bool execute(const std::string& query) override {
        std::cout << "  SQLite: Executing: " << query << '\n';
        return true;
    }
};

class PostgresDB : public Database {
public:
    bool connect(const std::string& cs) override {
        std::cout << "  Postgres: Connected to " << cs << '\n';
        return true;
    }
    void disconnect() override {
        std::cout << "  Postgres: Disconnected\n";
    }
    bool execute(const std::string& query) override {
        std::cout << "  Postgres: Executing: " << query << '\n';
        return true;
    }
};

// Works with ANY database — abstraction!
void processData(Database& db) {
    db.connect("mydb://localhost");
    db.runQuery("SELECT * FROM users");
    db.disconnect();
}

int main() {
    std::cout << "=== INTERFACES ===\n";
    std::unique_ptr<ILogger> logger = std::make_unique<ConsoleLogger>();
    logger->log("Application started");
    logger = std::make_unique<FileLogger>();
    logger->log("Application started");

    std::cout << "\n=== ABSTRACT CLASSES ===\n";
    SQLiteDB sqlite;
    PostgresDB postgres;
    processData(sqlite);
    std::cout << '\n';
    processData(postgres);

    return 0;
}
