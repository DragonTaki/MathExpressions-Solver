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
 * <summary>
 * This function checks if a candidate string conforms to a given expression and its corresponding color feedback,
 * similar to "Wordle"-style validation but adapted for expressions containing digits and operators.
 *
 * Rules:
 * - 'g' (green): character must match exactly at this position.
 * - 'y' (yellow): character must exist in candidate but at a different position.
 * - 'r' (red): character should not appear in candidate.
 * - '=': must appear exactly once and at the correct position.
 * - Only allowed operators from allowedOps are permitted.
 * </summary>
 *
 * @param candidate The candidate expression string to test (e.g., "12+46=58").
 * @param expression The guessed/target expression (e.g., "12+35=47").
 * @param color Feedback string corresponding to the guess ('g', 'y', 'r').
 * @param allowedOps Set of valid operator characters (e.g., '+', '-', '*', '/').
 * @return true if the candidate satisfies the feedback rules; false otherwise.
 */
bool matchesFeedback(
    const std::string& candidate,
    const std::string& expression,
    const std::string& color,
    const std::unordered_set<char>& allowedOps
) {
    // Debug log for current candidate evaluation
    AppLogger::Debug(std::format("Candidate = '{}', expression & color = '{} -> {}'", candidate, expression, color));

    // Validate string lengths
    if (candidate.size() != expression.size() || expression.size() != color.size()) {
        AppLogger::Log(std::format("Mismatched length: candidate length={}, expression length={}, color length={}",
            candidate.size(), expression.size(), color.size()),
            LogLevel::Error);
        return false;
    }

    // Count occurrences of each character in candidate for later validation
    std::unordered_map<char, int> candCount;
    for (char c : candidate) candCount[c]++;

    // --- Step 1: Validate green positions ---
    std::vector<bool> used(candidate.size(), false);
    for (size_t i = 0; i < candidate.size(); ++i) {
        if (color[i] == 'g') {
            if (candidate[i] != expression[i]) {  // Exact match required
                AppLogger::Log(std::format(
                    "Expected green match failed at position '{}': candidate='{}', expression='{}'",
                    i, candidate[i], expression[i]), LogLevel::Error);
                return false;
            }
            candCount[expression[i]]--;  // Consume occurrence
            used[i] = true;
        }
    }

    // --- Step 2: Validate yellow/red positions ---
    for (size_t i = 0; i < candidate.size(); ++i) {
        if (color[i] == 'g') continue;  // Skip already validated green positions

        char ch = expression[i];

        // '=' must be at correct position
        if (ch == '=') {
            if (candidate[i] != '=') {
                AppLogger::Log(std::format("Invalid '=' position at location '{}'.", i), LogLevel::Error);
                return false;
            }
            continue;
        }

        // Only symbols within allowedOps (e.g. + - * /) are permitted
        if (!isdigit(ch) && allowedOps.count(ch) == 0) {
            AppLogger::Log(std::format("Disallowed operator '{}' at location '{}'.", ch, i), LogLevel::Error);
            return false;
        }

        // Handle yellow and red feedback
        if (color[i] == 'y') {  // Yellow validation
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
        } else if (color[i] == 'r') {  // Red validation
            if (candCount[ch] > 0) {  // Character should not exist
                AppLogger::Log(std::format("Invalid symbol presence '{}' at location '{}'.", ch, i), LogLevel::Error);
                return false;
            }
        } else {
            // Invalid color character (must be one of 'g', 'y' or 'r')
            AppLogger::Log(std::format("Unknown feedback color '{}' at location '{}'.", color[i], i), LogLevel::Error);
            return false;
        }
    }

    return true;   // Candidate satisfies all feedback rules
}

/**
 * @brief Derives constraints for each symbol based on multiple feedback pairs.
 *
 * <summary>
 * Given a history of guessed expressions and their color feedbacks,
 * this function summarizes positional and count constraints for each symbol.
 *
 * For each symbol:
 * - greenPos: positions it must occupy (green feedback)
 * - bannedPos: positions it cannot occupy (yellow/red feedback)
 * - minCount: minimum occurrences across all feedback
 * - maxCount: maximum occurrences across all feedback
 *
 * '=' is enforced to appear exactly once.
 * </summary>
 *
 * @param expressions Vector of past guesses.
 * @param colors Vector of corresponding feedback strings ('g', 'y', 'r').
 * @param length Total length of each expression (used for maxCount bounds).
 * @return std::unordered_map<char, Constraint> Mapping from symbol to its derived Constraint.
 */
std::unordered_map<char, Constraint> deriveConstraints(
    const std::vector<std::string>& expressions,
    const std::vector<std::string>& colors,
    int length)
{
    const int INF = length;  // Maximum reasonable bound for character occurrences
    std::unordered_map<char, Constraint> constraintsMap;

    // Initialize all digits and operators with default Constraint
    for (char c = '0'; c <= '9'; ++c)
        constraintsMap[c] = Constraint(length);
    for (char c : std::string("+-*/^="))
        constraintsMap[c] = Constraint(length);

    // Process each guess-feedback pair
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

    // '=' must appear exactly once
    if (constraintsMap.contains('=')) {
        constraintsMap['='].minCount = 1;
        constraintsMap['='].maxCount = 1;
    }

    return constraintsMap;
}
