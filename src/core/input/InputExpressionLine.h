/* ----- ----- ----- ----- */
// InputExpressionLine.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/17
// Update Date: 2025/10/17
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>

#include "logic/ExpressionValidator.h"

/**
 * @class InputExpressionLine
 * @brief Handles user input for a single expression and its color feedback.
 */
class InputExpressionLine {
public:
    InputExpressionLine() = default;
    
    explicit InputExpressionLine(int exprLength);

    static bool readExpression(std::string& exprLine, int exprLength, ExpressionValidator& validator);
    static bool readColorFeedback(std::string& exprColorLine, int exprLength);
    static bool readPlayerInput(
        std::string& exprLine,
        std::string& exprColorLine,
        int exprLength,
        ExpressionValidator& validator
    );  // combine both steps

    const std::string& getExpression() const { return expression; }
    const std::string& getColorFeedback() const { return colorFeedback; }

    void clear();  // reset both

private:
    std::string expression;
    std::string colorFeedback;
    int exprLength;
};
