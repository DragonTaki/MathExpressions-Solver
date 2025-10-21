/* ----- ----- ----- ----- */
// InputExpressionLine.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/17
// Update Date: 2025/10/17
// Version: v1.0
/* ----- ----- ----- ----- */

#include "InputExpressionLine.h"
#include <iostream>
#include <string>

#include "InputUtils.h"
#include "core/constants/ExpressionConstants.h"
#include "core/logging/AppLogger.h"
#include "core/logging/LogColor.h"
#include "logic/ExpressionValidator.h"
#include "util/Utils.h"

/**
 * @brief Generic input reading loop with validation and special command handling.
 * 
 * This function repeatedly prompts the user with `promptMessage` and reads a line
 * from standard input. The input is preprocessed (spaces removed and converted to lowercase)
 * and then passed to a validator function and/or special command handler.
 * 
 * @param[out] outLine Reference to a string where the valid input line will be stored.
 * @param promptMessage The message displayed to prompt the user.
 * @param handleSpecialCommand Callback function to handle special commands. Returns a `SpecialCommandResult`.
 * @param validatorFunc Callback function to validate normal input lines. Returns true if valid.
 * @return true If a valid input line was successfully read.
 * @return false If the input process was terminated by a special command or input stream ended.
 */
bool InputExpressionLine::readInput(
    std::string& outLine,
    const std::string& promptMessage,
    std::function<SpecialCommandResult(const std::string& specialCommand)> handleSpecialCommand,
    std::function<bool(const std::string&)> validatorFunc
) {
    while (true) {
        AppLogger::Prompt(promptMessage, LogColor::Yellow);

        std::string line;
        if (!getline(std::cin, line)) return false;  ///< Return false if input stream ends

        line = Utils::removeSpaces(line);
        line = Utils::toLower(line);

        // Handle special commands via callback
        if (InputUtils::isSpecialCommand(line)) {
            SpecialCommandResult res = handleSpecialCommand(line);
            if (res == SpecialCommandResult::HANDLED_CONTINUE) {
                // Command handled (e.g., undo), continue waiting for new input
                continue;
            } else if (res == SpecialCommandResult::HANDLED_STOP_INPUT) {
                // Command handled indicating this input process should end (e.g., user requested end)
                return false;  // Inform caller that the current turn ends
            } else {
                // Other situation
                continue;
            }
        }

        // Validate input line
        if (!validatorFunc(line)) {
            AppLogger::Error("Invalid input, try again.");
            continue;
        }

        outLine = line;
        return true;
    }
}

/**
 * @brief Read a mathematical expression from user input with validation and special command handling.
 * 
 * This function wraps `readInput` and uses `ExpressionValidator` to check if
 * the entered line is a valid mathematical expression of the expected length.
 * 
 * @param[out] exprLine Reference to a string where the valid expression will be stored.
 * @param exprLength Expected length of the expression.
 * @param validator Reference to an `ExpressionValidator` instance used for validation.
 * @param handleSpecialCommand Callback to handle special commands.
 * @param promptMessage Message to prompt the user.
 * @return true If a valid expression was successfully read.
 * @return false If the input process was terminated by a special command or input stream ended.
 */
bool InputExpressionLine::readExpression(
    std::string& exprLine,
    int exprLength,
    ExpressionValidator& validator,
    std::function<SpecialCommandResult(const std::string& specialCommand)> handleSpecialCommand,
    const std::string& promptMessage
) {
    auto validatorFunc = [&](const std::string& line) -> bool {
        return validator.isValidExpression(line, exprLength);
    };
    return readInput(exprLine, promptMessage, handleSpecialCommand, validatorFunc);
}

/**
 * @brief Read a color feedback line corresponding to a mathematical expression.
 * 
 * This function wraps `readInput` and uses `InputUtils::isValidColor` to check
 * if the entered line is a valid color feedback string of the expected length.
 * 
 * @param[out] exprColorLine Reference to a string where the valid color line will be stored.
 * @param exprLength Expected length of the corresponding expression.
 * @param handleSpecialCommand Callback to handle special commands.
 * @param promptMessage Message to prompt the user.
 * @return true If a valid color feedback line was successfully read.
 * @return false If the input process was terminated by a special command or input stream ended.
 */
bool InputExpressionLine::readColorFeedback(
    std::string& exprColorLine,
    int exprLength,
    std::function<SpecialCommandResult(const std::string& specialCommand)> handleSpecialCommand,
    const std::string& promptMessage
) {
    auto validatorFunc = [&](const std::string& line) -> bool {
        return InputUtils::isValidColor(line, exprLength);
    };
    return readInput(exprColorLine, promptMessage, handleSpecialCommand, validatorFunc);
}
