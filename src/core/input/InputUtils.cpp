/* ----- ----- ----- ----- */
// InputUtils.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/18
// Update Date: 2025/10/18
// Version: v1.0
/* ----- ----- ----- ----- */

#include "InputUtils.h"
#include <cctype>
#include <string>

#include "core/constants/ExpressionConstants.h"

namespace InputUtils {

/**
 * @brief Check whether a given character is a valid arithmetic operator.
 * 
 * This function checks if the input character exists in the predefined
 * operator lookup table `Expression::OPERATOR_LOOKUP`.
 * 
 * @param exprChar The character to be validated as an operator.
 * @return true If `exprChar` is a recognized operator.
 * @return false If `exprChar` is not a recognized operator.
 */
bool isValidOperator(char exprChar) {
    return Expression::OPERATOR_LOOKUP.count(exprChar) > 0;
}

/**
 * @brief Validate the color feedback string for an expression.
 * 
 * Each character in `exprColorLine` should represent a valid feedback color.
 * The function performs the following checks:
 *   1. The length of `exprColorLine` must match `exprLength`.
 *   2. Each character, converted to lowercase, must exist in the
 *      `Expression::FEEDBACK_COLOR_LOOKUP` table.
 * 
 * @param exprColorLine A string representing color feedback (e.g., "gygbr").
 * @param exprLength The expected length of the expression corresponding to the color line.
 * @return true If the color line is valid in length and content.
 * @return false If the length mismatches or contains invalid color characters.
 */
bool isValidColor(const std::string& exprColorLine, int exprLength) {
    if ((int)exprColorLine.size() != exprLength) return false;
    for (char exprColorChar : exprColorLine) {
        char loweredColor = std::tolower(static_cast<unsigned char>(exprColorChar));
        if (!Expression::FEEDBACK_COLOR_LOOKUP.count(loweredColor)) return false;
    }
    return true;
}

/**
 * @brief Check whether the given string is a special command.
 * 
 * Currently supported commands:
 *   - "end": Terminate the input session.
 *   - "undo": Undo the previous action.
 * 
 * @param exprLine The input string to be checked.
 * @return true If `exprLine` matches a recognized special command.
 * @return false If `exprLine` does not match any special commands.
 */
bool isSpecialCommand(const std::string& exprLine) {
    static const std::unordered_set<std::string> commands{"end", "undo"};
    return commands.count(exprLine) > 0;
}

}  // namespace (end of InputUtils)
