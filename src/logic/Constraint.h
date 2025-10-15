/* ----- ----- ----- ----- */
// Constraint.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/05
// Update Date: 2025/10/15
// Version: v2.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/**
 * @enum ConstraintType
 * @brief Specifies the type of constraint: either a digit or a symbol.
 *
 * <summary>
 * Used in the `Constraint` struct to determine which underlying
 * BaseConstraint (digit or symbol) is currently active.
 * </summary>
 */
enum class ConstraintType { Digit, Symbol };

/**
 * @struct BaseConstraint
 * @brief Base class for character constraints shared by digits and symbols.
 *
 * <summary>
 * Stores the common data for any character in a candidate expression,
 * including positional restrictions, occurrence counts, and conflict flags.
 * This struct is embedded inside more specific constraint types such as SymbolConstraint or DigitConstraint.
 *
 * Members:
 * - `minCount`: Minimum required occurrences of the character.
 * - `maxCount`: Maximum allowed occurrences of the character.
 * - `greenPos`: Positions that must contain this character.
 * - `bannedPos`: Positions where the character is forbidden.
 * - `hasConflict`: Flag indicating conflicting feedback or invalid state.
 * </summary>
 */
struct BaseConstraint {
    int minCount = 0;                   ///< Minimum required occurrences of this character
    int maxCount = 9999;                ///< Maximum allowed occurrences of this character
    std::unordered_set<int> greenPos;   ///< Positions confirmed to contain this character
    std::unordered_set<int> bannedPos;  ///< Positions where this character is forbidden
    bool hasConflict = false;           ///< True if a conflict exists for this character

    int usedCount = 0;                  ///< Count used times for generating lhs

    BaseConstraint(int len = 0) {}
};

/**
 * @struct StructuralConstraint
 * @brief Represents structural rules and conflicts for a candidate expression.
 *
 * <summary>
 * This struct tracks constraints that arise from the structure or arrangement of characters,
 * such as forbidden sequences (e.g., adjacent operators), formatting rules, or other structural issues.
 *
 * Members:
 * - `hasConflict`: True if the candidate violates structural rules.
 * - `conflictPositions`: Positions in the expression where conflicts are detected.
 * </summary>
 */
struct StructuralConstraint {
    bool hasConflict = false;                  ///< True if there is a structural conflict
    std::vector<int> conflictPositions;        ///< Positions where structure issues occur (e.g., adjacent operators)
};

/**
 * @struct DigitConstraint
 * @brief Constraints specific to numeric characters in the expression.
 *
 * <summary>
 * Inherits from BaseConstraint to store common fields for digits.
 * Tracks positional constraints, counts, and conflicts for numeric characters.
 *
 * Members:
 * - Inherits all members of BaseConstraint (minCount, maxCount, greenPos, bannedPos, hasConflict)
 * </summary>
 */
struct DigitConstraint : BaseConstraint {
    using BaseConstraint::BaseConstraint;
};

/**
 * @struct SymbolConstraint
 * @brief Constraints specific to non-digit symbols (operators) in the expression.
 *
 * <summary>
 * Inherits from BaseConstraint for common constraint fields and adds structural rules.
 * Tracks positional constraints, counts, conflicts, and adjacency/formatting rules for operators.
 *
 * Members:
 * - `structure`: Structural rules and adjacency conflicts (StructuralConstraint).
 * - Inherits all BaseConstraint members (minCount, maxCount, greenPos, bannedPos, hasConflict).
 * </summary>
 */
struct SymbolConstraint : BaseConstraint {
    StructuralConstraint structure;    ///< Structural rules and conflicts
    using BaseConstraint::BaseConstraint;
};

/**
 * @struct Constraint
 * @brief Aggregates digit and symbol constraints for a single character.
 *
 * <summary>
 * Each character may either be a digit or a symbol.
 * Provides unified access to BaseConstraint fields via the `type` member.
 * Offers convenient getters/setters for min/max counts, positional constraints,
 * and conflict flags. Symbol structural rules are accessible via `structure()`.
 *
 * Members:
 * - `type`  : Indicates whether this is a digit or symbol constraint.
 * - `digit` : Digit-specific constraint fields.
 * - `symbol`: Symbol-specific constraint fields.
 *
 * Methods:
 * - Unified getters/setters for `minCount`, `maxCount`, `greenPos`, `bannedPos`, `hasConflict`.
 * - `structure()` getter for symbol structural rules.
 * </summary>
 */
struct Constraint {
    ConstraintType type;               ///< Indicates whether the character is a digit or symbol
    DigitConstraint digit;             ///< Digit-specific constraints
    SymbolConstraint symbol;           ///< Operator-specific constraints

    /**
     * @brief Get or modify the minimum required occurrences of this character.
     *
     * <summary>
     * Provides access to `minCount` in the appropriate underlying BaseConstraint.
     * Using a reference allows reading or updating directly.
     * </summary>
     * @return Reference to minCount
     */
    int& minCount() { return type == ConstraintType::Digit ? digit.minCount : symbol.minCount; }
    const int& minCount() const { return type == ConstraintType::Digit ? digit.minCount : symbol.minCount; }
    
    /**
     * @brief Get or modify the maximum allowed occurrences of this character.
     *
     * <summary>
     * Provides access to `maxCount` in the underlying BaseConstraint.
     * </summary>
     * @return Reference to maxCount
     */
    int& maxCount() { return type == ConstraintType::Digit ? digit.maxCount : symbol.maxCount; }
    const int& maxCount() const { return type == ConstraintType::Digit ? digit.maxCount : symbol.maxCount; }

    /**
     * @brief Access confirmed positions (green feedback) for this character.
     *
     * <summary>
     * Returns reference to `greenPos` in the appropriate BaseConstraint.
     * Each position represents an index in the candidate expression where
     * the character must appear.
     * </summary>
     * @return Reference to greenPos set
     */
    std::unordered_set<int>& greenPos() { return type == ConstraintType::Digit ? digit.greenPos : symbol.greenPos; }
    const std::unordered_set<int>& greenPos() const { return type == ConstraintType::Digit ? digit.greenPos : symbol.greenPos; }

    /**
     * @brief Access positions where this character is banned.
     *
     * <summary>
     * Returns reference to `bannedPos` in BaseConstraint.
     * Used for yellow/red feedback indicating positions the character cannot occupy.
     * </summary>
     * @return Reference to bannedPos set
     */
    std::unordered_set<int>& bannedPos() { return type == ConstraintType::Digit ? digit.bannedPos : symbol.bannedPos; }
    const std::unordered_set<int>& bannedPos() const { return type == ConstraintType::Digit ? digit.bannedPos : symbol.bannedPos; }

    /**
     * @brief Access or modify the conflict flag for this character.
     *
     * <summary>
     * Indicates whether feedback or structural rules have resulted in a conflict.
     * Setting this to true can mark a character as invalid in candidate filtering.
     * </summary>
     * @return Reference to hasConflict
     */
    bool& hasConflict() { return type == ConstraintType::Digit ? digit.hasConflict : symbol.hasConflict; }
    const bool& hasConflict() const { return type == ConstraintType::Digit ? digit.hasConflict : symbol.hasConflict; }

    /**
     * @brief Get or modify the used count of this character. For lhs generation.
     *
     * <summary>
     * Provides access to `usedCount` in the appropriate underlying BaseConstraint.
     * Using a reference allows reading or updating directly.
     * </summary>
     * @return Reference to usedCount
     */
    int& usedCount() { return type == ConstraintType::Digit ? digit.usedCount : symbol.usedCount; }
    const int& usedCount() const { return type == ConstraintType::Digit ? digit.usedCount : symbol.usedCount; }

    /**
     * @brief Access structural constraints (only relevant for symbols).
     *
     * <summary>
     * Returns reference to `structure` in SymbolConstraint.
     * Used to inspect or update adjacency and formatting conflicts.
     * </summary>
     * @return Reference to StructuralConstraint
     */
    StructuralConstraint& structure() { return symbol.structure; }
    const StructuralConstraint& structure() const { return symbol.structure; }
};

/**
 * @brief Derives per-character constraints based on previous guess-feedback pairs.
 *
 * <summary>
 * Given multiple guesses and corresponding feedback (like Wordle),
 * this function computes the minimum and maximum counts, green positions,
 * and banned positions for each character in the candidate equation.
 * </summary>
 *
 * @param expressions Vector of guessed strings (e.g., ["12+34=46"])
 * @param colors Vector of corresponding feedback strings ('g' = green, 'y' = yellow, 'r' = red)
 * @param length Total length of the equation (for initializing bannedPos vector)
 * @return std::unordered_map<char, Constraint> Mapping from character to its derived Constraint
 */
std::unordered_map<char, Constraint> deriveConstraints(
    const std::vector<std::string>& expressions,
    const std::vector<std::string>& colors,
    int length
);

/**
 * @brief Checks whether a candidate equation satisfies a single guess-feedback pair.
 *
 * <summary>
 * This function simulates Wordle feedback checking:
 * - 'g' (green): character must be at this position
 * - 'y' (yellow): character must exist but not at this position
 * - 'r' (red): character cannot appear (or within allowed max count)
 * Returns true if candidate would generate the same feedback for the guess.
 * </summary>
 *
 * @param candidate Candidate string to test (e.g., "12+46=58")
 * @param expression The guess string (e.g., "12+35=47")
 * @param color Feedback string for the guess ('g', 'y', 'r')
 * @param allowedOps Set of allowed operator characters (e.g., '+', '-', '*', '/')
 * @return true if candidate satisfies the guess-feedback pair; false otherwise
 */
bool matchesFeedback(
    const std::string& candidate,
    const std::string& expression,
    const std::string& color,
    const std::unordered_set<char>& allowedOps
);
