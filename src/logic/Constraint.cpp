/* ----- ----- ----- ----- */
// Constraint.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/05
// Update Date: 2025/10/05
// Version: v1.0
/* ----- ----- ----- ----- */

#include <algorithm>
#include <array>
#include <cctype>
#include <format>
#include <unordered_set>
#include "core/AppLogger.h"
#include "Constraint.h"

/**
 * @brief Validates whether a candidate expression matches the expected color feedback pattern.
 *
 * This function checks if a candidate string conforms to a given expression and its corresponding color feedback,
 * similar to "Wordle"-style validation but adapted for expression-based rules (digits and operators).
 *
 * The rules:
 * - 'g' (green): exact match at this position.
 * - 'y' (yellow): character exists in candidate but at a different position.
 * - 'r' (red): character does not exist in the candidate.
 * - '=' (equal sign): must appear only once and exactly at the correct position.
 * - All other operators must be within the allowed operator set.
 *
 * @param candidate The candidate expression string being tested.
 * @param expression The target expression string to match against.
 * @param color A color feedback string consisting of 'g', 'y', 'r' characters.
 * @param allowedOps A set of valid operator characters (e.g., '+', '-', '*', '/').
 * @return true if the candidate matches the feedback rules; false otherwise.
 */
bool matchesFeedback(
    const std::string& candidate,
    const std::string& expression,
    const std::string& color,
    const std::unordered_set<char>& allowedOps
) {
    AppLogger::Log(std::format("Candidate = '{}', expression & color = '{} -> {}'", candidate, expression, color), LogLevel::Debug);

    // Check if length is matched
    if (candidate.size() != expression.size() || expression.size() != color.size()) {
        AppLogger::Log(std::format("Mismatched length: candidate length={}, expression length={}, color length={}",
            candidate.size(), expression.size(), color.size()),
            LogLevel::Error);
        return false;
    }

    // Count for character occurrences
    // Used later to validate yellow/red counts and prevent overmatching
    std::unordered_map<char, int> candCount;
    for (char c : candidate) candCount[c]++;

    // Check green first
    // If color is green, the characters must match exactly
    std::vector<bool> used(candidate.size(), false);
    for (size_t i = 0; i < candidate.size(); ++i) {
        if (color[i] == 'g') {
            // Mismatch on a green-marked character → invalid
            if (candidate[i] != expression[i]) {
                AppLogger::Log(std::format(
                    "Expected green match failed at position '{}': candidate='{}', expression='{}'",
                    i, candidate[i], expression[i]), LogLevel::Error);
                return false;
            }
            // Consume one occurrence of this character from candidate's count
            candCount[expression[i]]--;
            used[i] = true;
        }
    }

    // Check yellow / red
    for (size_t i = 0; i < candidate.size(); ++i) {
        if (color[i] == 'g') continue;  // Skip already validated green positions

        char ch = expression[i];

        // '=' must appear once and in correct position
        if (ch == '=') {
            if (candidate[i] != '=') {
                AppLogger::Log(std::format("Invalid '=' position at location '{}'.", i), LogLevel::Error);
                return false;
            }
            continue;
        }

        // Only symbols within allowedOps (e.g. + - * /) are permitted.
        if (!isdigit(ch) && allowedOps.count(ch) == 0) {
            AppLogger::Log(std::format("Disallowed operator '{}' at location '{}'.", ch, i), LogLevel::Error);
            return false;
        }

        // Handle yellow and red feedback
        if (color[i] == 'y') {
            // Yellow means character exists elsewhere in candidate
            // Candidate No remaining occurrences
            if (candCount[ch] <= 0) {
                AppLogger::Log(std::format("Symbol '{}' exceeds allowed occurrences at location '{}'.", ch, i), LogLevel::Error);
                return false;
            }
            // If character matches in same position, color is invalid (should be green)
            if (candidate[i] == ch) {
                AppLogger::Log(std::format("Symbol '{}' mismatched color (green/yellow) at location '{}'.", ch, i), LogLevel::Error);
                return false;
            }
            // Consume one occurrence of this character from candidate's count
            candCount[ch]--;
        } else if (color[i] == 'r') {
            // Red means this character should not appear in candidate at all
            // Red candidate shows up
            if (candCount[ch] > 0) {
                AppLogger::Log(std::format("Invalid symbol presence '{}' at location '{}'.", ch, i), LogLevel::Error);
                return false;
            }
        } else {
            // Invalid color character (must be one of 'g'/'y'/'r')
            AppLogger::Log(std::format("Unknown feedback color '{}' at location '{}'.", color[i], i), LogLevel::Error);
            return false;
        }
    }

    return true;
}

/**
 * @brief Derives positional and count constraints for each symbol based on feedback history.
 *
 * This function analyzes multiple expression–color feedback pairs (like multiple Wordle rounds)
 * and summarizes the constraints on each character (digits and operators).
 *
 * For each symbol:
 * - Tracks all positions where it must appear ('green' positions).
 * - Tracks positions where it must NOT appear ('yellow' or 'red' positions).
 * - Updates the minimum and maximum number of times it can appear based on all feedback.
 *
 * @param expressions A list of past expression guesses (each same length).
 * @param colors A list of corresponding color feedback strings ('g', 'y', 'r').
 * @param length The expected expression length (used as an upper bound for counts).
 * @return std::unordered_map<char, Constraint> Mapping from each symbol to its derived constraints.
 */
std::unordered_map<char, Constraint> deriveConstraints(
    const std::vector<std::string>& expressions,
    const std::vector<std::string>& colors,
    int length)
{
    const int INF = length;  // Maximum reasonable bound for character occurrences
    std::unordered_map<char, Constraint> constraintsMap;

    // Initialize all possible characters
    for (char c = '0'; c <= '9'; ++c)
        constraintsMap[c] = Constraint(length);
    for (char c : std::string("+-*/^="))
        constraintsMap[c] = Constraint(length);

    // Process each feedback record
    for (size_t i = 0; i < expressions.size(); ++i) {
        const std::string& expression = expressions[i];
        const std::string& colorFeedback = colors[i];
        
        if (expression.size() != colorFeedback.size()) {
            AppLogger::Log(std::format("Expression length mismatch at index {}", i), LogLevel::Error);
            continue;
        }

        // Count total occurrences and "green/yellow" appearances for each symbol in this round
        std::unordered_map<char, int> totalCount;       // Total count of each character in expression
        std::unordered_map<char, int> validColorCount;  // Count of characters marked green or yellow

        // Analyze position-based constraints
        for (int pos = 0; pos < static_cast<int>(expression.size()); ++pos) {
            char symbol = expression[pos];
            char color = std::tolower(static_cast<unsigned char>(colorFeedback[pos]));
            totalCount[symbol]++;

            // Green or yellow means the character exists somewhere in candidate
            if (color == 'g' || color == 'y')
                validColorCount[symbol]++;

            // Record green position (must match)
            if (color == 'g')
                constraintsMap[symbol].greenPos.push_back(pos);

            // Record banned positions (yellow/red cannot appear here)
            if (color == 'y' || color == 'r')
                constraintsMap[symbol].bannedPos[pos] = true;
        }

        // Update global min/max occurrence constraints
        for (auto& [symbol, total] : totalCount) {
            int validCount = validColorCount.count(symbol) ? validColorCount[symbol] : 0;


            // minCount: must appear at least as many times as green+yellow in this guess
            constraintsMap[symbol].minCount = (std::max)(constraintsMap[symbol].minCount, validCount);

            // Determine per-guess max:
            // - if validCount == 0 => symbol not present at all (max = 0)
            // - if validCount < total  => some occurrences were red -> exact upper bound = validCount
            // - if validCount == total => no definite upper bound from this guess (use length)
            int perGuessMax;
            if (validCount == 0) {
                perGuessMax = 0;
            } else if (validCount < total) {
                perGuessMax = validCount; // definite upper bound
            } else { // validCount == total
                perGuessMax = length; // no new info on upper bound
            }

            // Narrow global maxCount only when this guess provides a definite upper bound
            constraintsMap[symbol].maxCount = (std::min)(constraintsMap[symbol].maxCount, perGuessMax);
        }
    }

    if (constraintsMap.contains('=')) {
        constraintsMap['='].minCount = 1;
        constraintsMap['='].maxCount = 1;
    }

    return constraintsMap;
}
