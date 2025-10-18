/* ----- ----- ----- ----- */
// InputSession.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/17
// Update Date: 2025/10/17
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "logic/Constraint.h"

struct SessionSnapshot {
    std::vector<std::string> expressions;
    std::vector<std::string> expressionColors;
    std::unordered_map<char, Constraint> constraintsMap;
};

class InputSession {
    std::vector<SessionSnapshot> history;
public:
    InputSession();

    // --- Basic accessors ---
    const std::unordered_map<char, Constraint>& getConstraintsMap() const;
    const std::vector<std::string>& getExpressions() const;
    const std::vector<std::string>& getExpressionColors() const;
    const std::unordered_set<char>& getOperatorsSet() const;
    int getExprLength() const;

    // --- Modifiers ---
    void setExprLength(int length);
    void setOperatorsSet(const std::unordered_set<char>& ops);
    void addExpression(const std::string& expr, const std::string& colorPattern);

    // --- Undo/Reset operations ---
    bool undoLast();
    void reset();

    void saveSnapshot();   // 存當前狀態
    void restoreLast();    // 回溯
private:
    std::unordered_map<char, Constraint> constraintsMap;
    std::vector<std::string> expressions;
    std::vector<std::string> expressionColors;
    std::unordered_set<char> operatorsSet;
    int exprLength = 0;
};
