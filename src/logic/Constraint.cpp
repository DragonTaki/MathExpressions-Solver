/* ----- ----- ----- ----- */
// Constraint.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/05
// Update Date: 2025/10/15
// Version: v2.0
/* ----- ----- ----- ----- */

#include <algorithm>
#include <array>
#include <cctype>
#include <stdexcept>
#include <format>
#include <unordered_set>

#include "Constraint.h"
#include "core/AppLogger.h"

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
 * @brief Updates a BaseConstraint based on a single feedback color at a given position.
 *
 * <summary>
 * This simplified version of `updateConstraint()` only records positional information
 * without handling count-based logic. It is intended to be called for each symbol
 * within a single guess-feedback pair.
 *
 * Parameters:
 * - `bc` : The BaseConstraint object to update.
 * - `color` : Feedback character ('g', 'y', or 'r').
 *     - 'g' (green): Character must appear at this position.
 *     - 'y' (yellow): Character exists but not at this position.
 *     - 'r' (red): Character does not exist at this position.
 * - `pos` : Position index in the current expression.
 *
 * The function marks positional constraints:
 * - Green positions are stored in `greenPos`.
 * - Yellow/red positions are flagged as banned in `bannedPos`.
 */
inline void updateConstraint(
    BaseConstraint& bc,
    char color,
    int pos)
{
    // Record positional constraints according to color feedback
    if (color == 'g') {
        bc.greenPos.insert(pos);  // Green: must appear here at this exact position
    } else if (color == 'y') {
        bc.bannedPos.insert(pos);    // Yellow: cannot appear here, but exists elsewhere
    } else if (color == 'r') {
        bc.bannedPos.insert(pos);    // Red: cannot appear here (or not at all)
    }
}

/**
 * @brief Processes a single guess-feedback pair and updates constraint states.
 *
 * <summary>
 * For each character in the guessed expression:
 * - Updates positional constraints (via `updateConstraint()`).
 * - Tracks per-character feedback counts (green/yellow/red).
 * After processing all positions, determines the minimum and maximum occurrence
 * bounds for each symbol based on feedback combinations.
 *
 * Conflict detection:
 * - If previously established exact bounds are contradicted by new feedback, the
 *   constraint is marked as conflicting and loosened (expanded) to preserve consistency.
 * - All detected conflicts are logged through `AppLogger::Warn()`.
 * </summary>
 *
 * @param expression Current guessed expression string.
 * @param color Corresponding feedback string ('g', 'y', 'r').
 * @param constraintsMap Mapping of character → Constraint structure (digit/symbol).
 * @param greenSymbolFlags Boolean vector marking which positions contain green operators.
 * @param INF Large integer representing "unknown upper bound" for max counts.
 * @param globalConflict Reference flag that becomes true if any conflict is found.
 */
void processSingleGuess(
    const std::string& expression,
    const std::string& color,
    std::unordered_map<char, Constraint>& constraintsMap,
    std::vector<bool>& greenSymbolFlags,
    int INF,
    bool& globalConflict)
{
    // Initialize local counters for feedback color statistics
    std::unordered_map<char,int> greenCount;
    std::unordered_map<char,int> yellowCount;
    std::unordered_map<char,int> redCount;

    // Iterate over each charactor in single guess-feedback pair
    for (size_t pos = 0; pos < expression.size(); ++pos) {
        char charactor = expression[pos];
        char charactorColor = std::tolower(static_cast<unsigned char>(color[pos]));

        // Apply position-specific constraint update
        if (std::isdigit(charactor)) {
            //AppLogger::Debug(std::format("Handling digit: {}", charactor));
            updateConstraint(constraintsMap[charactor].digit, charactorColor, (int)pos);
        } else if (std::string("+-*/^=").find(charactor) != std::string::npos) {
            //AppLogger::Debug(std::format("Handling symbol: {}", charactor));
            updateConstraint(constraintsMap[charactor].symbol, charactorColor, (int)pos);

            // Flag green operator location for structure conflict check
            if (charactorColor == 'g')
                greenSymbolFlags[pos] = true;
        }

        // Count color occurrences
        switch(charactorColor) {
            case 'g': greenCount[charactor]++;  break;
            case 'y': yellowCount[charactor]++; break;
            case 'r': redCount[charactor]++;    break;
        }
    }

    // Update min/max for charactor, and check conflicts
    for (auto& [symbol,constraint] : constraintsMap) {
        int g = greenCount[symbol];
        int y = yellowCount[symbol];
        int r = redCount[symbol];

        int oldMin = constraint.minCount();
        int oldMax = constraint.maxCount();
        int newMinCandidate = g + y;  // min >= 'g'+'y'
        int newMaxCandidate = INF;    // max <= INF, before being checked

        // max => decided by if 'r' or 'g'/'y' appears
        if (r > 0) {
            if (g + y > 0)  // Some 'r' and some 'g'/'y' => max = 'g'+'y'
                newMaxCandidate = g + y;
            else            // Only 'r' => max = 0
                newMaxCandidate = 0;
        } else {            // Only 'g'/'y' => max unknown
            //newMaxCandidate = oldMax;
        }

        // Detect contradictory constraints (Conflict check)
        bool previouslyBounded = (oldMin == oldMax);  // If already known exact number of occurrences
        if (previouslyBounded &&
            (newMinCandidate > oldMin || newMaxCandidate < oldMax))  // And bound changed => conflicts
        {
            if (!constraint.hasConflict()) {
                constraint.hasConflict() = true;
                globalConflict = true;
            }
            AppLogger::Warn(std::format(
                "Conflict: Symbol '{}' had bounded min/max ({}..{}); new guess tried to change to ({}..{})",
                symbol, oldMin, oldMax, newMinCandidate, newMaxCandidate));
        }

        // Write back
        if (!constraint.hasConflict()) {  // Directly write back if no conflict
            constraint.minCount() = (std::max)(constraint.minCount(), newMinCandidate);
            constraint.maxCount() = (std::min)(constraint.maxCount(), newMaxCandidate);
        } else {  // Loosen the bound if conflict happened
            constraint.minCount() = (std::min)(newMinCandidate, oldMin);
            constraint.maxCount() = (std::max)(newMaxCandidate, oldMax);
        }
    }
}

/**
 * @brief Derives global symbol constraints across multiple guess-feedback pairs.
 *
 * <summary>
 * This function aggregates constraints from multiple Wordle-like guesses
 * to build a comprehensive mapping of symbol restrictions:
 * - Positional constraints (greenPos, bannedPos)
 * - Count-based constraints (minCount, maxCount)
 * - Conflict detection and logging
 *
 * Additional structural checks:
 * - Detects adjacent green operators (invalid syntax patterns).
 * - Enforces '=' to appear exactly once.
 * </summary>
 *
 * @param expressions Vector of all past guess expressions.
 * @param colors Vector of corresponding feedback strings ('g', 'y', 'r').
 * @param length Total length of each expression.
 * @return std::unordered_map<char, Constraint> A map of symbol → derived constraints.
 */
std::unordered_map<char, Constraint> deriveConstraints(
    const std::vector<std::string>& expressions,
    const std::vector<std::string>& colors,
    int length)
{
    const int INF = length;                                ///< Reasonable upper bound for symbol occurrences
    std::unordered_map<char, Constraint> constraintsMap;   ///< Final result container
    bool globalConflict = false;                           ///< Tracks global conflict status
    std::vector<bool> greenSymbolFlags(length, false);     ///< Marks green operators for structure validation

    // Initialize all digits and operators with default Constraint
    for (char c = '0'; c <= '9'; ++c) {
        constraintsMap[c].digit = DigitConstraint(length);
        constraintsMap[c].type = ConstraintType::Digit;
    }
    for (char c : std::string("+-*/^=")) {
        constraintsMap[c].symbol = SymbolConstraint(length);
        constraintsMap[c].type = ConstraintType::Symbol;
    }

    // Process each guess-feedback pair
    for (size_t i = 0; i < expressions.size(); ++i) {
        const std::string& expression = expressions[i];
        const std::string& color = colors[i];
        AppLogger::Debug(std::format("Start derive constraint: {} -> {}", expression, color));
        
        // Length check, if no match than ignore this pair
        if (expression.size() != length) {
            AppLogger::Error(std::format("Length error: Expression length mismatch at index {}, should be length {}", i, length));
            continue;
        } else if (color.size() != length) {
            AppLogger::Error(std::format("Length error: Color length mismatch at index {}, should be length {}", i, length));
            continue;
        }

        // Accumulate constraint updates for this round
        processSingleGuess(expression, color, constraintsMap, greenSymbolFlags, INF, globalConflict);
    }

    // Structural validation
    for (size_t pos = 1; pos < greenSymbolFlags.size(); ++pos) {
        if (greenSymbolFlags[pos] && greenSymbolFlags[pos - 1]) {
            AppLogger::Warn(std::format(
                "Conflict: Cross-guess adjacent green symbol conflict between pos {} and {}",
                pos - 1, pos));
            globalConflict = true;

            // Mark all symbol constraints as conflicted (since we can’t know which operator caused it)
            for (auto& [symbol, constraint] : constraintsMap) {
                constraint.symbol.structure.hasConflict = true;
                constraint.symbol.structure.conflictPositions.push_back(pos - 1);
                constraint.symbol.structure.conflictPositions.push_back(pos);
            }
        }
    }

    // '=' must appear exactly once
    if (constraintsMap.contains('=')) {
        if (constraintsMap['='].greenPos().size() > 1) {  // If detected more than one green position
            constraintsMap['='].greenPos().clear();
            AppLogger::Warn(std::format("Conflict: '=' has {} green locations", constraintsMap['='].symbol.greenPos.size()));
            constraintsMap['='].hasConflict() = true;
            globalConflict = true;
        }
        constraintsMap['='].minCount() = 1;
        constraintsMap['='].maxCount() = 1;
    } else {
        throw std::runtime_error("Data error: '=' not show in constraints map.");
    }

    if (globalConflict) {
        AppLogger::Warn("Detected conflicts in some constraints.");
    }

    return constraintsMap;
}
