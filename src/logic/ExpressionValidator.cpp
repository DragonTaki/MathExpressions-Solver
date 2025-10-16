/* ----- ----- ----- ----- */
// ExpressionValidator.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/01
// Update Date: 2025/10/16
// Version: v1.1
/* ----- ----- ----- ----- */

#include <optional>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#include "ExpressionValidator.h"

using namespace std;

namespace {
    /**
     * @brief Get operator precedence.
     *
     * <summary>
     * Defines standard arithmetic operator precedence:
     * '^' > '*' '/' > '+' '-'.
     * </summary>
     *
     * @param op Operator character ('+', '-', '*', '/', '^')
     * @return int Precedence level (higher = higher priority)
     */
    int precedence(char op) {
        if (op == '^') return 3;
        if (op == '*' || op == '/') return 2;
        if (op == '+' || op == '-') return 1;
        return 0;
    }

    /**
     * @brief Checks whether an operator is left-associative.
     *
     * <summary>
     * In this evaluator, all operators are left-associative
     * except for '^' which is right-associative.
     * </summary>
     *
     * @param op Operator character
     * @return true if left-associative; false if right-associative
     */
    bool isLeftAssociative(char op) {
        return (op != '^'); // '^' is right-associative
    }

    /**
     * @brief Applies a binary operator to two operands.
     *
     * @param a Left-hand operand
     * @param b Right-hand operand
     * @param op Operator character ('+', '-', '*', '/', '^')
     * @return double Result of the operation
     * @throws runtime_error On invalid operator, division by zero, or negative exponent
     */
    double applyOp(double a, double b, char op) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/':
                if (b == 0) throw runtime_error("Division by zero");
                return a / b;
            case '^': {
                if (b < 0) throw runtime_error("Negative exponent not supported");
                double res = 1;
                for (int i = 0; i < b; i++) res *= a;
                return res;
            }
        }
        throw runtime_error("Invalid operator");
    }
} // anonymous namespace

// ---- Public API ----
// ---- Shunting Yard + RPN Evaluate ----

/**
 * @brief Evaluates an arithmetic expression string and returns the result.
 *
 * <summary>
 * This function uses the Shunting Yard algorithm to convert infix expressions
 * to Reverse Polish Notation (RPN) and then evaluates the RPN.
 *
 * Supports operators: +, -, *, /, ^ (exponentiation)
 * Supports multi-digit integers.
 *
 * @param s Expression string (e.g., "12+3*4")
 * @return double Evaluation result
 * @throws runtime_error If expression contains invalid characters, malformed RPN, or illegal operations
 */
double ExpressionValidator::evalExpr(const string& s) {
    vector<string> output;   // RPN output queue
    stack<char> opsStack;    // Operator stack
    string num;              // Buffer for multi-digit numbers

    auto flushNum = [&]() {
        if (!num.empty()) {
            output.push_back(num);
            num.clear();
        }
    };

    // --- Shunting Yard Algorithm ---
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

    // --- RPN Evaluation ---
    stack<double> st;
    for (auto& token : output) {
        if (isdigit(token[0])) {
            st.push(stoll(token));
        } else {
            if (st.size() < 2) throw runtime_error("Malformed expression");
            double b = st.top(); st.pop();
            double a = st.top(); st.pop();
            st.push(applyOp(a, b, token[0]));
        }
    }
    if (st.size() != 1) throw runtime_error("Malformed RPN eval");
    return st.top();
}

/**
 * @brief Safely evaluates an expression and returns optional result.
 *
 * <summary>
 * Wraps evalExpr in try-catch to prevent exceptions from propagating.
 * Returns std::nullopt if evaluation fails.
 * </summary>
 *
 * @param expr Expression string
 * @return std::optional<double> Evaluation result if successful; std::nullopt otherwise
 */
std::optional<double> ExpressionValidator::safeEval(const std::string& expr) {
    try {
        ExpressionValidator validator;
        double res = validator.evalExpr(expr);
        return res;
    } catch (...) {
        return std::nullopt;
    }
}

/**
 * @brief Checks whether a string is a valid arithmetic equation.
 *
 * <summary>
 * Validates if the string:
 * 1. Has exactly 'n' characters
 * 2. Contains exactly one '='
 * 3. Has non-empty left and right sides
 * 4. Left and right sides evaluate to the same value
 * </summary>
 *
 * @param s Expression string (e.g., "12+35=47")
 * @param n Expected length of expression
 * @return true if valid equation; false otherwise
 */
bool ExpressionValidator::isValidExpression(const string& s, int n) {
    if ((int)s.size() != n) return false;

    size_t eqPos = s.find('=');
    if (eqPos == string::npos || s.find('=', eqPos+1) != string::npos)
        return false;

    string left = s.substr(0, eqPos);
    string right = s.substr(eqPos+1);
    if (left.empty() || right.empty()) return false;

    try {
        double lv = evalExpr(left);
        double rv = evalExpr(right);
        return lv == rv;
    } catch (...) {
        return false;
    }
}

/**
 * @brief Checks whether a double value is effectively an integer, considering floating-point precision.
 *
 * @param val Value to check
 * @param epsilon Allowed tolerance (default 1e-9)
 * @return true if val is an integer within tolerance
 */
bool ExpressionValidator::isInteger(double val, double epsilon) {
    double nearest = std::round(val);               // Nearest integer
    return std::abs(val - nearest) < epsilon;       // Gap < epsilon => Considered as integer
}

/**
 * @brief 過濾候選運算式，保留符合本輪猜測顏色限制的項目
 * 
 * @param candidates 上一輪候選運算式
 * @param guess 玩家本輪猜測運算式
 * @param color 對應的顏色串（例如 "gyr..."）
 * @return vector<string> 篩選後的候選列表
 */
vector<string> ExpressionValidator::filterExpressions(
    const vector<string>& candidates,
    const string& guess,
    const string& color)
{
    vector<string> filtered;

    for (const auto& cand : candidates) {
        bool valid = true;

        // Error handling: Length check (should not be here)
        if (cand.size() != guess.size() || cand.size() != color.size()) {
            continue;
        }

        // 檢查每個位置
        unordered_multiset<char> guessRemaining;
        for (size_t i = 0; i < guess.size(); ++i) {
            char charactor = guess[i];
            char charactorColor = color[i];

            if (charactorColor == 'g') { // 綠色：位置正確
                if (cand[i] != charactor) {
                    valid = false;
                    break;
                }
            } else {
                // 暫存非綠色字元，後面用於黃色檢查
                guessRemaining.insert(charactor);
            }
        }

        if (!valid) continue;

        // 再檢查黃色：字元必須存在於候選中，但位置不同
        for (size_t i = 0; i < guess.size(); ++i) {
            char c = guess[i];
            char charactorColor = color[i];
            if (charactorColor == 'y') {
                if (cand[i] == c || guessRemaining.count(c) == 0) {
                    valid = false;
                    break;
                } else {
                    guessRemaining.erase(guessRemaining.find(c));
                }
            }
        }

        if (!valid) continue;

        // 灰色/紅色：字元不應出現在候選解的其他位置（已被綠黃排除的字元會先被消耗掉）
        for (size_t i = 0; i < guess.size(); ++i) {
            char charactor = guess[i];
            char charactorColor = color[i];
            if (charactorColor == 'r') {
                // 如果候選還有此字元且不是已確認綠黃位置
                if (count(cand.begin(), cand.end(), charactor) > 0) {
                    valid = false;
                    break;
                }
            }
        }

        if (valid) filtered.push_back(cand);
    }

    return filtered;
}
