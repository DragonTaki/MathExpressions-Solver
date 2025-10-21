/* ----- ----- ----- ----- */
// InputExpressionLine.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/17
// Update Date: 2025/10/17
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <functional>
#include <string>

#include "core/input/InputUtils.h"
#include "logic/ExpressionValidator.h"

/**
 * @class InputExpressionLine
 * @brief Handles user input for a single mathematical expression and its color feedback.
 * 
 * This class provides high-level functions to:
 *   1. Prompt the user for input with a customizable message.
 *   2. Preprocess input by removing spaces and converting to lowercase.
 *   3. Validate the input using a custom validator callback.
 *   4. Handle special commands via a callback mechanism.
 * 
 * It is used for reading both mathematical expressions and feedback color lines
 * in a safe, reusable way.
 */
class InputExpressionLine {
public:
    InputExpressionLine() = default;
    
    /**
     * @brief Generic input reading function with validation and special command handling.
     * 
     * Repeatedly prompts the user with `promptMessage` until a valid input is entered
     * or a special command ends the input process. Spaces are removed and input is
     * converted to lowercase before validation.
     * 
     * @param[out] outLine Reference to store the valid input line.
     * @param promptMessage Message displayed to prompt the user.
     * @param handleSpecialCommand Callback to handle special commands.
     * @param validatorFunc Callback to validate normal input lines. Should return true if valid.
     * @return true If a valid input line was successfully read.
     * @return false If input was terminated by a special command or input stream ended.
     */
    bool readInput(
        std::string& outLine,
        const std::string& promptMessage,
        std::function<SpecialCommandResult(const std::string& specialCommand)> handleSpecialCommand,
        std::function<bool(const std::string&)> validatorFunc
    );

    /**
     * @brief Read a mathematical expression from the user.
     * 
     * Wraps `readInput` and uses `ExpressionValidator` to ensure the input line is a
     * valid mathematical expression of the expected length.
     * 
     * @param[out] exprLine Reference to store the valid expression.
     * @param exprLength Expected length of the expression.
     * @param validator Reference to an `ExpressionValidator` instance for validation.
     * @param handleSpecialCommand Callback to handle special commands.
     * @param promptMessage Message displayed to prompt the user.
     * @return true If a valid expression was successfully read.
     * @return false If input was terminated by a special command or input stream ended.
     */
    bool readExpression(
        std::string& exprLine,
        int exprLength,
        ExpressionValidator& validator,
        std::function<SpecialCommandResult(const std::string& specialCommand)> handleSpecialCommand,
        const std::string& promptMessage
    );

    /**
     * @brief Read a color feedback line from the user corresponding to a mathematical expression.
     * 
     * Wraps `readInput` and uses `InputUtils::isValidColor` to ensure the input line
     * is a valid color feedback string of the expected length.
     * 
     * @param[out] exprColorLine Reference to store the valid color feedback line.
     * @param exprLength Expected length of the corresponding expression.
     * @param handleSpecialCommand Callback to handle special commands.
     * @param promptMessage Message displayed to prompt the user.
     * @return true If a valid color feedback line was successfully read.
     * @return false If input was terminated by a special command or input stream ended.
     */
    bool readColorFeedback(
        std::string& exprColorLine,
        int exprLength,
        std::function<SpecialCommandResult(const std::string& specialCommand)> handleSpecialCommand,
        const std::string& promptMessage
    );
};
