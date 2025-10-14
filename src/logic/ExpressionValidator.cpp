/* ----- ----- ----- ----- */
// ExpressionValidator.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/01
// Update Date: 2025/10/01
// Version: v1.0
/* ----- ----- ----- ----- */

#include <optional>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#include "ExpressionValidator.h"

using namespace std;

// ---- Operator utils ----
int precedence(char op) {
    if (op == '^') return 3;
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

bool isLeftAssociative(char op) {
    return (op != '^'); // '^' is right-associative
}

long long applyOp(long long a, long long b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) throw runtime_error("Division by zero");
            return a / b;
        case '^': {
            if (b < 0) throw runtime_error("Negative exponent not supported");
            long long res = 1;
            for (int i = 0; i < b; i++) res *= a;
            return res;
        }
    }
    throw runtime_error("Invalid operator");
}

// ---- Shunting Yard + RPN Evaluate ----
long long ExpressionValidator::evalExpr(const string& s) {
    vector<string> output;
    stack<char> opsStack;
    string num;

    auto flushNum = [&]() {
        if (!num.empty()) {
            output.push_back(num);
            num.clear();
        }
    };

    for (char c : s) {
        if (isdigit(c)) {
            num.push_back(c);
        } else if (validOps.count(c)) {
            flushNum();
            while (!opsStack.empty() && validOps.count(opsStack.top())) {
                char top = opsStack.top();
                if ((isLeftAssociative(c) && precedence(c) <= precedence(top)) ||
                    (!isLeftAssociative(c) && precedence(c) < precedence(top))) {
                    output.push_back(string(1, top));
                    opsStack.pop();
                } else break;
            }
            opsStack.push(c);
        } else {
            throw runtime_error("Invalid character in expression");
        }
    }
    flushNum();
    while (!opsStack.empty()) {
        output.push_back(string(1, opsStack.top()));
        opsStack.pop();
    }

    stack<long long> st;
    for (auto& token : output) {
        if (isdigit(token[0])) {
            st.push(stoll(token));
        } else {
            if (st.size() < 2) throw runtime_error("Malformed expression");
            long long b = st.top(); st.pop();
            long long a = st.top(); st.pop();
            st.push(applyOp(a, b, token[0]));
        }
    }
    if (st.size() != 1) throw runtime_error("Malformed RPN eval");
    return st.top();
}

std::optional<long long> ExpressionValidator::safeEval(const std::string& expr) {
    try {
        ExpressionValidator validator;
        long long res = validator.evalExpr(expr);
        return res;
    } catch (...) {
        return std::nullopt;
    }
}

// ---- Public API ----
bool ExpressionValidator::isValidExpression(const string& s, int n) {
    if ((int)s.size() != n) return false;

    size_t eqPos = s.find('=');
    if (eqPos == string::npos || s.find('=', eqPos+1) != string::npos)
        return false;

    string left = s.substr(0, eqPos);
    string right = s.substr(eqPos+1);
    if (left.empty() || right.empty()) return false;

    try {
        long long lv = evalExpr(left);
        long long rv = evalExpr(right);
        return lv == rv;
    } catch (...) {
        return false;
    }
}
