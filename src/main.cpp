/* ----- ----- ----- ----- */
// main.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/09/28
// Update Date: 2025/10/16
// Version: v2.0
/* ----- ----- ----- ----- */

#include <iostream>
#include <algorithm>
#include <format>
#include <unordered_set>
#include "core/AppLogger.h"
#include "logic/CandidateGenerator.h"
#include "logic/ExpressionValidator.h"
#include "Utils.h"

#define FMT_HEADER_ONLY
#include "core.h"
#include "ranges.h"

using namespace std;

// -------------------- Tool functions --------------------
bool isValidOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

bool isValidColor(const string& s, int length) {
    if ((int)s.size() != length) return false;
    for (char c : s) {
        char lower = tolower(c);
        if (lower != 'r' && lower != 'y' && lower != 'g') return false;
    }
    return true;
}

// -------------------- Read expression length and available operators --------------------
bool readLengthAndOps(int& length, unordered_set<char>& operators) {
    while (true) {
        cout << "Input expression length and available operators (e.g. \"8 + - * /\", or \"8+-*/\"): ";
        string line;
        if (!getline(cin, line)) return false;
        AppLogger::Log(std::format("Got raw line: {}", line), LogLevel::Debug);

        line = removeSpaces(line);
        if (line.empty()) continue;

        size_t idx = 0;
        while (idx < line.size() && isspace(static_cast<unsigned char>(line[idx]))) ++idx;
        size_t start = idx;
        while (idx < line.size() && isdigit(static_cast<unsigned char>(line[idx]))) ++idx;

        string firstToken = line.substr(start, idx - start);
        AppLogger::Log(std::format("First token (exp length): {}", firstToken), LogLevel::Debug);

        // Parse length
        try {
            length = stoi(firstToken);
        } catch (...) {
            AppLogger::Log("Input Error: first token must be integer.", LogLevel::Error);
            continue;
        }
        if (length < 5) {
            AppLogger::Log("Input Error: length must be >= 5.", LogLevel::Error);
            continue;
        }

        // Parse rest part (operators)
        string rest = line.substr(firstToken.size());
        rest.erase(remove(rest.begin(), rest.end(), ' '), rest.end());

        unordered_set<char> validOps;
        string invalidChars;
        for (char c : rest) {
            if (!isValidOperator(c)) {
                invalidChars += c;
            } else {
                validOps.insert(c);
            }
        }

        if (!invalidChars.empty()) {
            AppLogger::Log(fmt::format("Operator Error: invalid operator(s): {}", fmt::join(invalidChars, ", ")), LogLevel::Error);
            continue;
        }

        if (!validOps.count('+')) {
            AppLogger::Log("Operator Error: must include '+' operator.", LogLevel::Error);
            continue;
        }

        operators = validOps;

        AppLogger::Log(fmt::format("Operators parsed: {}", fmt::join(operators, ", ")), LogLevel::Debug);

        return true;
    }
    return false;
}

// -------------------- Read math expression --------------------
bool readExpression(string& expression, int length, ExpressionValidator& validator) {
    while (true) {
        cout << "Input expression (or \"END\" to finish) (e.g. \"12 + 46 = 58\", or \"12+46=58\"): ";
        string line;
        if (!getline(cin, line)) return false;
        AppLogger::Log(std::format("Got raw line: {}", line), LogLevel::Debug);

        line = removeSpaces(line);
        if (line == "END" || line == "end") return false;

        if (!validator.isValidExpression(line, length)) {
            if ((int)line.size() != length) AppLogger::Log("Expression Error: length not match.", LogLevel::Error);
            else AppLogger::Log("Expression Error: invalid expression.", LogLevel::Error);
            continue;
        }

        expression = line;
        return true;
    }
    return false;
}

// -------------------- Read color feedback --------------------
bool readColorFeedback(string& color, int length) {
    while (true) {
        cout << "Input color feedback (e.g. \"r y r y y g r r\", or \"ryryygrr\"): ";
        string line;
        if (!getline(cin, line)) return false;
        AppLogger::Log(std::format("Got raw line: {}", line), LogLevel::Debug);

        line = removeSpaces(line);
        transform(line.begin(), line.end(), line.begin(), ::tolower);

        if (!isValidColor(line, length)) {
            if ((int)line.size() != length) AppLogger::Log("Color Error: length not match.", LogLevel::Error);
            else AppLogger::Log("Color Error: invalid color.", LogLevel::Error);
            continue;
        }

        color = line;
        return true;
    }
    return false;
}

// -------------------- Main function --------------------
int main() {
    // Initialize program
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Initialize AppLogger
    AppLogger::Initialize();
    std::atexit([]() {
        AppLogger::Shutdown();
    });
    AppLogger::EnableTestMode(true);
    AppLogger::SetLogLevel(LogLevel::Trace);
    AppLogger::Log(std::format("__cplusplus = {}", __cplusplus), LogLevel::Debug);
    
    // Input first round: length & operators, first expression, first color feedback
    auto firstInput = [&]() -> std::vector<std::string> {
        int length;
        unordered_set<char> operators;
        
        // Error handling & Read expression length and available operators
        if (!readLengthAndOps(length, operators))
            throw std::runtime_error("Failed to read length and operators");

        vector<string> expressions;  ///< Player-guessed expressions
        vector<string> colors;       ///< Game-feedback colors

        ExpressionValidator validator;
        validator.setValidOps(operators);
    
        // First time read expression and color feedback
        string expression;
        if (!readExpression(expression, length, validator))
            throw std::runtime_error("Failed to read first expression");
        expressions.push_back(expression);

        string color;
        if (!readColorFeedback(color, length))
            throw std::runtime_error("Failed to read first color feedback");
        colors.push_back(color);

        // Generate initial candidates by using CandidateGenerator
        CandidateGenerator generator(validator);
        auto results = generator.generate(length, operators, expressions, colors);

        if (results.empty())
            std::cout << "No solution.\n";
        else {
            std::cout << "--- First round candidates ---\n";
            for (auto& r : results)
                std::cout << r << "\n";
        }

        return results;
    };

    while (true) {
        try {
            // First input, generate initial candidates
            std::vector<std::string> candidates = firstInput();
            
            // Subsequent input, filtering from existing candidates
            auto nextInput = [&](std::vector<std::string>& currentCandidates) -> bool {
                // Error handling: Empty candidates (should not be here)
                if (currentCandidates.empty()) return false;

                string expression;  ///< Following expression guess, or "end" for end whole round
                cout << "\nEnter your guess (or 'end' to finish this round): ";
                if (!std::getline(cin, expression)) return false;
                if (expression == "end") return false;

                string color;       ///< The color pairing the expression
                cout << "Enter color feedback: ";
                if (!std::getline(cin, color)) return false;

                // Filter candidates
                ExpressionValidator validator;
                currentCandidates = validator.filterExpressions(currentCandidates, expression, color);

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
            while (nextInput(candidates)) {
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
