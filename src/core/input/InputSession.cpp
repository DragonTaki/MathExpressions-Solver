/* ----- ----- ----- ----- */
// InputSession.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/17
// Update Date: 2025/10/17
// Version: v1.0
/* ----- ----- ----- ----- */

#include "InputSession.h"
#include <string>
#include <unordered_set>

InputSession::InputSession() = default;

const std::unordered_map<char, Constraint>& InputSession::getConstraintsMap() const { return constraintsMap; }
const std::vector<std::string>& InputSession::getExpressions() const { return expressions; }
const std::vector<std::string>& InputSession::getExpressionColors() const { return expressionColors; }
const std::unordered_set<char>& InputSession::getOperatorsSet() const { return operatorsSet; }
int InputSession::getExprLength() const { return exprLength; }

void InputSession::setExprLength(int length) { exprLength = length; }
void InputSession::setOperatorsSet(const std::unordered_set<char>& ops) { operatorsSet = ops; }

void InputSession::addExpression(const std::string& expr, const std::string& colorPattern) {
    expressions.push_back(expr);
    expressionColors.push_back(colorPattern);
}

bool InputSession::undoLast() {
    if (expressions.empty()) return false;
    expressions.pop_back();
    expressionColors.pop_back();
    return true;
}

void InputSession::reset() {
    expressions.clear();
    expressionColors.clear();
    constraintsMap.clear();
    operatorsSet.clear();
    exprLength = 0;
}
