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

// -------------------- Read expression length and available operators --------------------
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

bool InputExpressionSpec::readExpressionSpec(int& exprLength, std::unordered_set<char>& operatorsSet) {
    return readLengthAndOps(exprLength, operatorsSet); // 使用原本工具函數
}