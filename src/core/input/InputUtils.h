/* ----- ----- ----- ----- */
// InputUtils.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/18
// Update Date: 2025/10/18
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>

/**
 * @enum SpecialCommandResult
 * @brief Represents the result of checking a user input for special commands.
 * 
 * This enum is used to indicate whether a user input line is a special command
 * and how it should be handled by the input processing logic.
 */
enum class SpecialCommandResult {
    NOT_SPECIAL,        ///< Input is not a special command
    HANDLED_CONTINUE,   ///< Special command handled; continue current input process (e.g., show prompt again)
    HANDLED_STOP_INPUT  ///< Special command handled; stop current input process (e.g., user wants to end the turn)
};

namespace InputUtils {

/**
 * @brief Check whether a given character is a valid arithmetic operator.
 * 
 * Uses the predefined lookup table `Expression::OPERATOR_LOOKUP` to verify
 * if the character is recognized as an operator.
 * 
 * @param exprChar The character to check.
 * @return true If `exprChar` is a valid operator.
 * @return false Otherwise.
 */
bool isValidOperator(char exprChar);

/**
 * @brief Validate a color feedback string corresponding to an expression.
 * 
 * Each character in `exprColorLine` should represent a valid feedback color.
 * The function verifies:
 *   1. The length of `exprColorLine` matches `exprLength`.
 *   2. Every character (converted to lowercase) exists in
 *      `Expression::FEEDBACK_COLOR_LOOKUP`.
 * 
 * @param exprColorLine String of color feedback characters (e.g., "gygbr").
 * @param exprLength Expected length of the corresponding expression.
 * @return true If the color line is valid.
 * @return false If length mismatches or invalid color characters are found.
 */
bool isValidColor(const std::string& exprColorLine, int exprLength);

/**
 * @brief Check whether the input string is a special command.
 * 
 * Recognized special commands include:
 *   - "end": End the current input session.
 *   - "undo": Undo the previous action.
 * 
 * @param exprLine Input string to check.
 * @return true If `exprLine` is a recognized special command.
 * @return false Otherwise.
 */
bool isSpecialCommand(const std::string& exprLine);

}  // namespace (end of InputUtils)
