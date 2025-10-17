/* ----- ----- ----- ----- */
// main.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/09/28
// Update Date: 2025/10/16
// Version: v2.0
/* ----- ----- ----- ----- */

#include <algorithm>
#include <format>
#include <iostream>
#include <unordered_set>

#include "core/AppLogger.h"
#include "core/constants/ExpressionConstants.h"
#include "logic/CandidateGenerator.h"
#include "logic/ExpressionValidator.h"
#include "util/ConsoleUtils.h"
#include "util/Utils.h"

#define FMT_HEADER_ONLY
#include "core.h"
#include "ranges.h"

// -------------------- Tool functions --------------------
bool isValidOperator(char exprChar) {
    return Expression::OPERATOR_LOOKUP.count(exprChar) > 0;
}

bool isValidColor(const std::string& exprColorLine, int exprLength) {
    if ((int)exprColorLine.size() != exprLength) return false;
    for (char exprColorChar : exprColorLine) {
        char loweredColor = std::tolower(static_cast<unsigned char>(exprColorChar));
        if (!Expression::FEEDBACK_COLOR_LOOKUP.count(loweredColor)) return false;
    }
    return true;
}

// -------------------- Read expression length and available operators --------------------
bool readLengthAndOps(int& exprLength, std::unordered_set<char>& operatorsSet) {
    while (true) {
        std::cout << "Input expression length and available operators (e.g. \"8 + - * /\", or \"8+-*/\"): ";
        std::string line;
        if (!getline(std::cin, line)) return false;
        AppLogger::Debug(std::format("Got raw line: {}", line));

        line = removeSpaces(line);
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
        rest.erase(remove(rest.begin(), rest.end(), ' '), rest.end());

        std::unordered_set<char> validOperatorsSet;
        std::string invalidChars;
        for (char c : rest) {
            if (!isValidOperator(c)) {
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

// -------------------- Read math expression --------------------
bool readExpression(std::string& exprLine, int exprLength, ExpressionValidator& validator) {
    while (true) {
        std::cout << "Input expression (or \"END\" to finish) (e.g. \"12 + 46 = 58\", or \"12+46=58\"): ";
        std::string line;
        if (!getline(std::cin, line)) return false;
        AppLogger::Debug(std::format("Got raw line: {}", line));

        line = removeSpaces(line);
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
bool readColorFeedback(std::string& exprColorLine, int exprLength) {
    while (true) {
        std::cout << "Input color feedback (e.g. \"r y r y y g r r\", or \"ryryygrr\"): ";
        std::string line;
        if (!getline(std::cin, line)) return false;
        AppLogger::Log(std::format("Got raw line: {}", line), LogLevel::Debug);

        line = removeSpaces(line);
        transform(line.begin(), line.end(), line.begin(), ::tolower);

        if (!isValidColor(line, exprLength)) {
            if ((int)line.size() != exprLength)
                AppLogger::Log("Color Error: length not match.", LogLevel::Error);
            else
                AppLogger::Log("Color Error: invalid color.", LogLevel::Error);
            continue;
        }

        exprColorLine = line;
        return true;
    }
    return false;
}

// -------------------- Main function --------------------
int main() {
    // Initialize program
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // Initialize AppLogger
    AppLogger::Initialize();
    std::atexit([]() {
        AppLogger::Shutdown();
    });
    AppLogger::EnableTestMode(true);
    AppLogger::SetLogLevel(LogLevel::Debug);
    AppLogger::Log(std::format("__cplusplus = {}", __cplusplus), LogLevel::Debug);
    
    // Input first round: length & operators, first expression, first color feedback
    auto firstInput = [&](std::unordered_map<char, Constraint> constraintsMap) -> std::vector<std::string> {
        int exprLength;
        std::unordered_set<char> operatorsSet;
        
        // Error handling & Read expression length and available operators
        if (!readLengthAndOps(exprLength, operatorsSet))
            throw std::runtime_error("Failed to read length and operators");

        std::vector<std::string> expressions;       ///< Player-guessed expressions
        std::vector<std::string> expressionColors;  ///< Game-feedback colors

        ExpressionValidator validator;
        validator.setValidOps(operatorsSet);
    
        // First time read expression and color feedback
        std::string exprLine;
        if (!readExpression(exprLine, exprLength, validator))
            throw std::runtime_error("Failed to read first expression");
        expressions.push_back(exprLine);

        std::string exprColorLine;
        if (!readColorFeedback(exprColorLine, exprLength))
            throw std::runtime_error("Failed to read first color feedback");
        expressionColors.push_back(exprColorLine);

        // Generate initial candidates by using CandidateGenerator
        CandidateGenerator generator(validator);
        auto possibleResults = generator.generate(exprLength, operatorsSet, expressions, expressionColors, constraintsMap);

        // Print results
        if (possibleResults.empty())
            std::cout << "No solution.\n";
        else {
            std::cout << "--- First round candidates ---\n";
            printCandidatesInline(possibleResults);
        }

        return possibleResults;
    };

    while (true) {
        try {
            // Build constraints
            std::unordered_map<char, Constraint> constraintsMap = initializeConstraintsMap();
            // First input, generate initial candidates
            std::vector<std::string> candidatesList = firstInput(constraintsMap);
            
            // Subsequent input, filtering from existing candidates
            auto nextInput = [&](std::vector<std::string>& currentCandidates) -> bool {
                // Error handling: Empty candidates (should not be here)
                if (currentCandidates.empty()) return false;

                std::string exprLine;       ///< Following expression guess, or "end" for end whole round
                std::cout << "\nEnter your guess (or 'end' to finish this round): ";
                if (!getline(std::cin, exprLine)) return false;
                if (exprLine == "end") return false;

                std::string exprColorLine;  ///< The color pairing the expression
                std::cout << "Enter color feedback: ";
                if (!getline(std::cin, exprColorLine)) return false;
                
                // Update constraint
                bool isUpdated = updateConstraint(constraintsMap, exprLine, exprColorLine);
                if (isUpdated) {
                    printConstraint(constraintsMap);
                }
                // Filter candidates
                ExpressionValidator validator;
                currentCandidates = validator.filterExpressions(currentCandidates, constraintsMap);

                // Show filtered results
                std::cout << "--- Filtered candidates ---\n";
                if (currentCandidates.empty())
                    std::cout << "No solution.\n";
                else
                    for (auto& c : currentCandidates)
                        std::cout << c << "\n";

                return true;
            };

            // Main process logic
            while (nextInput(candidatesList)) {
                // Will continue to let the player to enter new guesses and colors, until "end" is entered
            }

            // Game ended, start new game
            std::cout << "\nRound finished. Start a new round.\n\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            break;
        }
    }

    // Program ending procedure
    AppLogger::Shutdown();
    return 0;
}
