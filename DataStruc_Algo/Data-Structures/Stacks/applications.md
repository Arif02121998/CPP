# File: Stacks/applications.md

[← Repository Index](../README.md) · **Stacks:** [stack_basics](stack_basics.md) · [stack_implementation](stack_implementation.md) · [monotonic_stack](monotonic_stack.md) · applications

---

## Conceptual Understanding

**Definition.** This file collects the **canonical real-world and interview applications** of stacks: expression parsing/evaluation, the call stack & recursion-to-iteration, backtracking, and string-processing tricks.

**Why it matters.** Recognizing "this is a stack problem" is half the battle. The applications below recur constantly because they all share one trait: **the most recently seen item must be resolved first**.

**Real-world analogy.** Reading nested instructions — you dive into a sub-task, finish it, then resume exactly where you paused. The pause points form a stack.

---

## Application 1 — Expression Evaluation & Conversion

**Infix → Postfix (Shunting-yard idea).** Use an operator stack; pop higher/equal-precedence operators before pushing a new one; parentheses delimit sub-expressions.

**Postfix evaluation.** Operand stack; on operator pop two, compute, push back (see [stack_basics.md](stack_basics.md)).

```cpp
#include <stack>
#include <string>
#include <cctype>

int prec(char op){ return (op=='+'||op=='-')?1 : (op=='*'||op=='/')?2 : 0; }

std::string infixToPostfix(const std::string& s) {
    std::stack<char> ops;
    std::string out;
    for (char c : s) {
        if (std::isalnum((unsigned char)c)) out += c;
        else if (c=='(') ops.push(c);
        else if (c==')') {
            while (!ops.empty() && ops.top()!='(') { out += ops.top(); ops.pop(); }
            if (!ops.empty()) ops.pop();            // discard '('
        } else {                                    // operator
            while (!ops.empty() && prec(ops.top()) >= prec(c)) { out += ops.top(); ops.pop(); }
            ops.push(c);
        }
    }
    while (!ops.empty()) { out += ops.top(); ops.pop(); }
    return out;
}
```
💡 Precedence + a stack convert/evaluate any expression in O(n).

---

## Application 2 — Balanced Symbols / Syntax Validation

Brackets, HTML/XML tags, code blocks. Push openers, match closers against the top (see [stack_basics.md](stack_basics.md) Valid Parentheses). The basis of compiler/parser front-ends.

---

## Application 3 — The Call Stack & Recursion → Iteration

Every function call pushes a frame (locals, return address); returning pops it. Deep recursion can overflow this finite stack. You can **convert recursion to iteration** with an explicit stack — essential for iterative DFS and to avoid stack overflow.

```cpp
// Iterative DFS over a graph adjacency list
#include <vector>
#include <stack>
void dfsIterative(int start, const std::vector<std::vector<int>>& adj) {
    std::vector<bool> seen(adj.size(), false);
    std::stack<int> st; st.push(start);
    while (!st.empty()) {
        int u = st.top(); st.pop();
        if (seen[u]) continue;
        seen[u] = true;
        // visit u ...
        for (int v : adj[u]) if (!seen[v]) st.push(v);
    }
}
```
**Explanation.** The explicit stack replays exactly what the call stack would do — LIFO ordering yields depth-first order. (More in [../Graphs/dfs.md](../Graphs/dfs.md).)

---

## Application 4 — Backtracking State

Push a choice, recurse/iterate, pop to undo. Maze solving, N-Queens, generating combinations — the stack records the path so you can rewind one step at a time.

---

## Application 5 — String Processing

### Simplify Unix Path (Medium)
```cpp
#include <sstream>
#include <vector>
#include <string>
std::string simplifyPath(const std::string& path) {
    std::vector<std::string> st;                 // vector as a stack
    std::stringstream ss(path);
    std::string part;
    while (std::getline(ss, part, '/')) {
        if (part=="" || part==".") continue;
        if (part=="..") { if (!st.empty()) st.pop_back(); }
        else st.push_back(part);
    }
    std::string res = "/";
    for (size_t i = 0; i < st.size(); ++i) { res += st[i]; if (i+1<st.size()) res += "/"; }
    return res;
}
```
**Explanation.** `..` pops the last directory; valid names push. The stack is the canonical path.

### Remove Adjacent Duplicates
Push chars; if the incoming char equals the top, pop instead of pushing — collapses pairs in O(n).

---

## Complexity Summary

| Application | Time | Space |
|---|---|---|
| Infix→postfix / eval | O(n) | O(n) |
| Balanced symbols | O(n) | O(n) |
| Iterative DFS | O(V+E) | O(V) |
| Simplify path | O(n) | O(n) |

---

## Tradeoffs
- ✅ Stacks linearize naturally nested/recursive logic.
- ❌ Explicit stacks can be more verbose than recursion; choose iteration mainly to avoid overflow or for control.

---

## Use Cases
- Compilers/interpreters (parsing, evaluation).
- Calculators, spreadsheet formula engines.
- Editors (undo/redo), browsers (history).
- Graph/tree traversal without recursion.
- Path normalization, tokenization, template engines.

---

## Pitfalls
- ⚠️ Operator precedence/associativity bugs (right-associative `^` needs `>` not `>=`).
- ⚠️ Mismatched/missing parentheses not handled → invalid output.
- ⚠️ Iterative DFS visit-order differs from recursive unless you reverse neighbor pushes.
- ⚠️ Forgetting to mark visited on **pop** vs **push** can revisit nodes.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Evaluate/parse an expression or formula" → operator/operand **stacks**.
- "Validate nesting/structure" → **matching stack**.
- "Avoid recursion / huge depth" → **explicit stack DFS**.
- "Undo last action / collapse adjacent" → **stack**.

---

## Example Problems
- **Easy:** Valid Parentheses; Remove Adjacent Duplicates; Baseball Game (score stack).
- **Medium:** Evaluate RPN; Basic Calculator II; Simplify Path; Decode String (nested `k[...]`).
- **Hard:** Basic Calculator (with parentheses & unary minus); Largest Rectangle in Histogram (see [monotonic_stack.md](monotonic_stack.md)).

---

*Next →* [../Queues/queue.md](../Queues/queue.md)
