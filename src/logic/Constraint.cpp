/* ----- ----- ----- ----- */
// Constraint.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/05
// Update Date: 2025/10/16
// Version: v2.1
/* ----- ----- ----- ----- */

#include "Constraint.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <format>
#include <stdexcept>
#include <unordered_set>

#include "core/AppLogger.h"
#include "core/constants/ExpressionConstants.h"

#define FMT_HEADER_ONLY
#include "core.h"
#include "ranges.h"

// =========================
//  Internal Helper Section
// =========================
namespace {

/**
 * @brief Updates a BaseConstraint based on a single feedback color at a given position.
 *
 * <summary>
 * This simplified version of `updateConstraint()` only records positional information
 * without handling count-based logic. It is intended to be called for each symbol
 * within a single guess-feedback pair.
 *
 * Parameters:
 * - `baseConstraint` : The BaseConstraint object to update.
 * - `color` : Feedback exprChar ('g', 'y', or 'r').
 *     - 'g' (green): exprChar must appear at this position.
 *     - 'y' (yellow): exprChar exists but not at this position.
 *     - 'r' (red): exprChar does not exist at this position.
 * - `position` : Position index in the current expression.
 *
 * The function marks positional constraints:
 * - Green positions are stored in `greenPos`.
 * - Yellow/red positions are flagged as banned in `bannedPos`.
 */
inline bool updateConstraint(
    BaseConstraint& baseConstraint,
    char color,
    int position)
{
    bool isUpdated = false;

    switch (color) {
    case 'g':
        // Green: must appear exactly here
        if (baseConstraint.greenPos.insert(position).second) {
            isUpdated = true;
        }
        break;

    case 'y':
        // Yellow: cannot appear here, but must exist elsewhere
        if (baseConstraint.bannedPos.insert(position).second) {
            isUpdated = true;
        }
        break;

    case 'r':
        // Red: cannot appear here (and possibly not at all)
        if (baseConstraint.bannedPos.insert(position).second) {
            isUpdated = true;
        }
        break;

    default:
        break;
    }

    return isUpdated;
}

/**
 * @brief Processes a single guess-feedback pair and updates constraint states.
 *
 * <summary>
 * For each exprChar in the guessed expression:
 * - Updates positional constraints (via `updateConstraint()`).
 * - Tracks per-exprChar feedback counts (green/yellow/red).
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
 * @param constraintsMap Mapping of exprChar → Constraint structure (digit/symbol).
 * @param greenSymbolFlags Boolean vector marking which positions contain green operators.
 * @param INF Large integer representing "unknown upper bound" for max counts.
 * @param hasGlobalConflict Reference flag that becomes true if any conflict is found.
 */
bool updateConstraintsMapWithSingleGuess(
    const std::string& exprLine,
    const std::string& exprColorLine,
    std::unordered_map<char, Constraint>& constraintsMap,
    std::vector<bool>& greenSymbolFlags,
    int INF,
    bool& hasGlobalConflict
) {
    bool hasAnyUpdate = false;

    // Initialize local counters for feedback color statistics
    std::unordered_map<char,int> greenCount, yellowCount, redCount;

    // Iterate over each exprChar in single guess-feedback pair
    for (size_t position = 0; position < exprLine.size(); ++position) {
        char exprChar = exprLine[position];
        char exprColorChar = std::tolower(static_cast<unsigned char>(exprColorLine[position]));

        // Apply position-specific constraint update
        if (std::isdigit(exprChar)) {
            //AppLogger::Debug(std::format("Handling digit: {}", exprChar));
            hasAnyUpdate |= updateConstraint(constraintsMap[exprChar].digitConstraint, exprColorChar, static_cast<int>(position));
        } else if (Expression::OPERATOR_LOOKUP.count(exprChar)) {
            //AppLogger::Debug(std::format("Handling symbol: {}", exprChar));
            hasAnyUpdate |= updateConstraint(constraintsMap[exprChar].operatorConstraint, exprColorChar, static_cast<int>(position));

            // Flag green operator location for structure conflict check
            if (exprColorChar == 'g')
                greenSymbolFlags[position] = true;
        } else {
            // Error handling (should not be here)
            AppLogger::Error(std::format("Unexpected symbol '{}' at exp '{}', pos '{}'", exprChar, exprLine, position));
        }

        // Count color occurrences
        switch(exprColorChar) {
            case 'g': greenCount[exprChar]++;  break;
            case 'y': yellowCount[exprChar]++; break;
            case 'r': redCount[exprChar]++;    break;
        }
    }

    // Update min/max for exprChar, and check conflicts
    for (auto& [symbol,constraint] : constraintsMap) {
        int g = greenCount[symbol];
        int y = yellowCount[symbol];
        int r = redCount[symbol];

        int currentMin = constraint.minCount();
        int currentMax = constraint.maxCount();
        int candidateMin = g + y;  // min >= 'g'+'y'
        int candidateMax = INF;    // max <= INF, before being checked

        // max => decided by if 'r' or 'g'/'y' appears
        if (r > 0) {
            if (g + y > 0)  // Some 'r' and some 'g'/'y' => max = 'g'+'y'
                candidateMax = g + y;
            else            // Only 'r' => max = 0
                candidateMax = 0;
        } else {            // Only 'g'/'y' => max unknown
            //candidateMax = currentMax;
        }

        // Detect contradictory constraints (Conflict check)
        bool isPreviouslyBounded = (currentMin == currentMax);  // If already known exact number of occurrences
        if (isPreviouslyBounded &&
            (candidateMin > currentMin || candidateMax < currentMax))  // And bound changed => conflicts
        {
            if (!constraint.hasConflict()) {
                constraint.hasConflict() = true;
                hasGlobalConflict = true;
            }
            AppLogger::Warn(std::format(
                "Conflict: Symbol '{}' had bounded min/max ({}..{}); new guess tried to change to ({}..{})",
                symbol, currentMin, currentMax, candidateMin, candidateMax));
        }

        // Write back
        if (!constraint.hasConflict()) {  // Directly write back if no conflict
            constraint.minCount() = (std::max)(constraint.minCount(), candidateMin);
            constraint.maxCount() = (std::min)(constraint.maxCount(), candidateMax);
            hasAnyUpdate = true;
        } else {  // Loosen the bound if conflict happened
            constraint.minCount() = (std::min)(candidateMin, currentMin);
            constraint.maxCount() = (std::max)(candidateMax, currentMax);
            hasAnyUpdate = true;
        }
    }

    return hasAnyUpdate;
}

} // namespace (end of internal helpers)

// =========================
//  Public Functions Section
// =========================

/**
 * @brief Prints all constraints in a readable format via AppLogger.
 *
 * @param constraintsMap Mapping of exprChar → Constraint
 */
void printConstraint(const std::unordered_map<char, Constraint>& constraintsMap) {
    AppLogger::Debug("===== Derived Constraints =====");
    for (const auto& kv : constraintsMap) {
        const char ExprSymbol = kv.first;
        const Constraint& constraint = kv.second;

        // Join green positions
        std::string greenPositionsString;
        for (int pos : constraint.greenPos()) {
            greenPositionsString += std::to_string(pos) + " ";
        }

        // Join banned positions
        std::string bannedPositionsString;
        for (int pos : constraint.bannedPos()) {
            bannedPositionsString += std::to_string(pos) + " ";
        }

        AppLogger::Debug(fmt::format(
            "Symbol: {} | MinCount: {} | MaxCount: {} | GreenPos: {{{}}} | BannedPos: {{{}}} | Conflict: {}",
            ExprSymbol,
            constraint.minCount(),
            constraint.maxCount(),
            greenPositionsString,
            bannedPositionsString,
            constraint.hasConflict() ? "YES" : "NO"
        ));
    }
}

std::unordered_map<char, Constraint> initializeConstraintsMap() {
    std::unordered_map<char, Constraint> initialConstraintsMap;
    for (char symbol : Expression::SYMBOLS) {
        initialConstraintsMap.emplace(symbol, Constraint(symbol));
    }
    return initialConstraintsMap;
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
    const std::vector<std::string>& expressionColors,
    int expLength)
{
    const int INF = expLength;                             ///< Reasonable upper bound for symbol occurrences
    std::unordered_map<char, Constraint> constraintsMap;   ///< Final result container
    bool hasGlobalConflict = false;                        ///< Tracks global conflict status
    std::vector<bool> greenSymbolFlags(expLength, false);  ///< Marks green operators for structure validation

    // Initialize all digits and operators with default Constraint
    for (char exprChar : Expression::OPERATOR_SYMBOLS) {
        constraintsMap[exprChar].operatorConstraint = OperatorConstraint(expLength);
        constraintsMap[exprChar].type = ConstraintType::Operator;
    }
    for (char exprChar : Expression::DIGIT_SYMBOLS) {
        constraintsMap[exprChar].digitConstraint = DigitConstraint(expLength);
        constraintsMap[exprChar].type = ConstraintType::Digit;
    }

    // Process each guess-feedback pair
    for (size_t i = 0; i < expressions.size(); ++i) {
        const std::string& exprLine      = expressions[i];
        const std::string& exprColorLine = expressionColors[i];
        AppLogger::Debug(std::format("Start derive constraint: \"{} -> {}\"", exprLine, exprColorLine));
        
        // Length check, if no match than ignore this pair
        if (exprLine.size() != expLength) {
            AppLogger::Error(std::format(
                "Length error: Expression length mismatch at index '{}', should be '{}', but got '{}'", i, expLength, exprLine.size()));
            continue;
        } else if (exprColorLine.size() != expLength) {
            AppLogger::Error(std::format(
                "Length error: Color length mismatch at index '{}', should be '{}', but got '{}'", i, expLength, exprColorLine.size()));
            continue;
        }

        // Accumulate constraint updates for this round
        updateConstraintsMapWithSingleGuess(exprLine, exprColorLine, constraintsMap, greenSymbolFlags, INF, hasGlobalConflict);
    }

    // Structural validation
    for (size_t position = 1; position < greenSymbolFlags.size(); ++position) {
        if (greenSymbolFlags[position] && greenSymbolFlags[position - 1]) {
            AppLogger::Warn(std::format(
                "Structural conflict: Cross-guess adjacent green symbol conflict between pos '{}' and '{}'",
                position - 1, position));
            hasGlobalConflict = true;

            // Mark all symbol constraints as conflicted (since we can’t know which operator caused it)
            for (auto& [symbol, constraint] : constraintsMap) {
                constraint.operatorConstraint.structure.hasConflict = true;
                constraint.operatorConstraint.structure.conflictPositions.push_back(position - 1);
                constraint.operatorConstraint.structure.conflictPositions.push_back(position);
            }
        }
    }

    // '=' must appear exactly once
    if (constraintsMap.contains('=')) {
        if (constraintsMap['='].greenPos().size() > 1) {  // If detected more than one green position
            constraintsMap['='].greenPos().clear();
            AppLogger::Warn(std::format("Structural conflict: '=' has {} green locations",
                constraintsMap['='].operatorConstraint.greenPos.size()));
            constraintsMap['='].hasConflict() = true;
            hasGlobalConflict = true;
        }
        constraintsMap['='].minCount() = 1;
        constraintsMap['='].maxCount() = 1;
    } else {
        throw std::runtime_error("Data error: '=' not show in constraints map.");
    }

    if (hasGlobalConflict) {
        AppLogger::Warn("Detected conflicts in some constraints.");
    }

    return constraintsMap;
}

bool updateConstraint(
    std::unordered_map<char, Constraint>& constraintsMap,
    const std::string& exprLine,
    const std::string& exprColorLine
) {
    // Error handling: Length error (should not be here)
    if (exprLine.size() != exprColorLine.size()) {
        AppLogger::Error("[updateConstraint] Length mismatch between expression and color");
        return false;
    }

    const int INF = static_cast<int>(exprLine.size());  ///< Upper bound for maxCount
    bool hasGlobalConflict = false;                          ///< Track if any conflicts occurred
    std::vector<bool> greenSymbolFlags(exprLine.size(), false);  ///< Mark green operators

    // Use updateConstraintsMapWithSingleGuess to update the existing Constraints Map
    bool isUpdated = updateConstraintsMapWithSingleGuess(
        exprLine, exprColorLine, constraintsMap, greenSymbolFlags, INF, hasGlobalConflict
    );

    if (hasGlobalConflict) {
        AppLogger::Warn("[UpdateConstraint] Detected conflicts in this update.");
    }

    return isUpdated;
}
