/* ----- ----- ----- ----- */
// InputExpressionSpec.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/17
// Update Date: 2025/10/17
// Version: v1.0
/* ----- ----- ----- ----- */

#include "InputExpressionSpec.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include "InputUtils.h"
#include "core/logging/AppLogger.h"
#include "util/Utils.h"

#define FMT_HEADER_ONLY
#include "core.h"
#include "ranges.h"

/**
 * @brief Prompt the user to input the expression length and available operators.
 * 
 * This function repeatedly prompts the user until a valid input is provided.
 * The expected input format is: "<length> <operators>", e.g., "8 + - *" or "8+-*".
 * 
 * Validation steps:
 *   1. The first token must be an integer >= 5 representing the expression length.
 *   2. The remaining characters are interpreted as operators.
 *   3. Each operator must exist in `InputUtils::isValidOperator`.
 *   4. The '+' operator is mandatory.
 * 
 * @param[out] exprLength Reference to store the parsed expression length.
 * @param[out] operatorsSet Reference to store the set of valid operators.
 * @return true If a valid expression length and operator set are successfully read.
 * @return false If the input stream ends before valid input is provided.
 */
bool InputExpressionSpec::readLengthAndOps(int& exprLength, std::unordered_set<char>& operatorsSet) {
    while (true) {
        AppLogger::Prompt("Input expression length and available operators (e.g. \"8 + - * /\", or \"8+-*/\"): ", LogColor::Yellow);
        std::string line;
        if (!getline(std::cin, line)) return false;
        AppLogger::Debug(std::format("Got raw line: {}", line));

        line = Utils::removeSpaces(line);
        if (line.empty()) continue;

        size_t idx = 0;
        while (idx < line.size() && isspace(static_cast<unsigned char>(line[idx]))) ++idx;
        size_t start = idx;
        while (idx < line.size() && isdigit(static_cast<unsigned char>(line[idx]))) ++idx;

        std::string firstToken = line.substr(start, idx - start);
        AppLogger::Debug(std::format("First token (exp length): {}", firstToken));

        // Parse length
        try {
            exprLength = stoi(firstToken);
        } catch (...) {
            AppLogger::Error("Input Error: first token must be integer.");
            continue;
        }
        if (exprLength < 5) {
            AppLogger::Error("Input Error: length must be >= 5.");
            continue;
        }

        // Parse rest part (operators)
        std::string rest = line.substr(firstToken.size());

        std::unordered_set<char> validOperatorsSet;
        std::string invalidChars;
        for (char c : rest) {
            if (!InputUtils::isValidOperator(c)) {
                invalidChars += c;
            } else {
                validOperatorsSet.insert(c);
            }
        }

        if (!invalidChars.empty()) {
            AppLogger::Error(fmt::format("Operator Error: invalid operator(s): {}", fmt::join(invalidChars, ", ")));
            continue;
        }

        if (!validOperatorsSet.count('+')) {
            AppLogger::Error("Operator Error: must include '+' operator.");
            continue;
        }

        operatorsSet = validOperatorsSet;

        AppLogger::Debug(fmt::format("Operators parsed: {}", fmt::join(operatorsSet, ", ")));

        return true;
    }
    return false;
}

/**
 * @brief High-level wrapper to read the full expression specification.
 * 
 * Currently, this function simply calls `readLengthAndOps` to get the expression
 * length and available operators from the user.
 * 
 * @param[out] exprLength Reference to store the parsed expression length.
 * @param[out] operatorsSet Reference to store the set of valid operators.
 * @return true If a valid expression specification is successfully read.
 * @return false If the input stream ends before valid input is provided.
 */
bool InputExpressionSpec::readExpressionSpec(int& exprLength, std::unordered_set<char>& operatorsSet) {
    return readLengthAndOps(exprLength, operatorsSet);  // Use existing utility function
}