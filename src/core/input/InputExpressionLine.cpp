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


// -------------------- Read math expression --------------------
bool InputExpressionLine::readExpression(
    std::string& exprLine,
    int exprLength,
    ExpressionValidator& validator
) {
    while (true) {
        std::string line;
        if (!getline(std::cin, line)) return false;
        AppLogger::Debug(std::format("Got raw line: {}", line));

        line = Utils::removeSpaces(line);
        if (line == "END" || line == "end") return false;

        if (!validator.isValidExpression(line, exprLength)) {
            if ((int)line.size() != exprLength)
                AppLogger::Error("Expression Error: length not match.");
            else
                AppLogger::Error("Expression Error: invalid expression.");
            continue;
        }

        exprLine = line;
        return true;
    }
    return false;
}

// -------------------- Read color feedback --------------------
bool InputExpressionLine::readColorFeedback(
    std::string& exprColorLine,
    int exprLength
) {
    while (true) {
        std::string line;
        if (!getline(std::cin, line)) return false;
        AppLogger::Debug(std::format("Got raw line: {}", line));

        line = Utils::removeSpaces(line);
        transform(line.begin(), line.end(), line.begin(), ::tolower);

        if (!InputUtils::isValidColor(line, exprLength)) {
            if ((int)line.size() != exprLength)
                AppLogger::Error("Color Error: length not match.");
            else
                AppLogger::Error("Color Error: invalid color.");
            continue;
        }

        exprColorLine = line;
        return true;
    }
    return false;
}

bool InputExpressionLine::readPlayerInput(
    std::string& exprLine,
    std::string& exprColorLine,
    int exprLength,
    ExpressionValidator& validator
) {
    AppLogger::Prompt("Input expression (or 'end' to finish, 'undo' to rollback):", LogColor::Yellow);
    if (!readExpression(exprLine, exprLength, validator)) return false;
    AppLogger::Prompt("Input color feedback:", LogColor::Yellow);
    if (!readColorFeedback(exprColorLine, exprLength)) return false;
    return true;
}
