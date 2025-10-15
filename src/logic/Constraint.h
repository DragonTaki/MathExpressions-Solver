/* ----- ----- ----- ----- */
// Constraint.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/05
// Update Date: 2025/10/05
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
    int minCount = 0;                  ///< Minimum required occurrences of this character
    int maxCount = 9999;               ///< Maximum allowed occurrences of this character
    std::vector<int> greenPos;         ///< Positions confirmed to contain this character
    std::vector<bool> bannedPos;       ///< Positions where this character is forbidden
    bool hasConflict = false;          ///< True if a conflict exists for this character

    BaseConstraint() = default;
    explicit BaseConstraint(int len) : bannedPos(len, false) {}  ///< Initializes bannedPos vector
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
 * @brief Aggregates all constraint types for a single character in a candidate expression.
 *
 * <summary>
 * Each character may either be a digit or a symbol.
 * This struct unifies digit and symbol constraints, providing a pointer `base` to easily access
 * common fields regardless of character type. Also provides a unified getter/setter interface.
 *
 * Members:
 * - `base`: Pointer to either `digit` or `symbol` BaseConstraint.
 * - `digit`: Digit-specific constraints.
 * - `symbol`: Symbol-specific constraints.
 *
 * Methods:
 * - Constructor `Constraint(int len, bool isDigit)` initializes digit/symbol constraints and sets `base`.
 * - `minCount()`, `maxCount()`, `greenPos()`, `bannedPos()`, `hasConflict()` provide unified access to BaseConstraint fields.
 * - `structure()` provides access to SymbolConstraint structural rules.
 * </summary>
 */
struct Constraint {
    BaseConstraint* base;              ///< Pointer to digit or symbol BaseConstraint
    DigitConstraint digit;             ///< Digit-specific constraints
    SymbolConstraint symbol;           ///< Operator-specific constraints

    Constraint() : digit(0), symbol(0), base(&digit) {}  // default: treat as digit

    /**
     * @brief Constructs a Constraint and selects type.
     * @param len Length of the expression (used to initialize bannedPos vectors)
     * @param isDigit True if this constraint represents a digit, false for symbol
     */
    Constraint(int len, bool isDigit)
        : digit(len), symbol(len) {
        base = isDigit ? static_cast<BaseConstraint*>(&digit)
                       : static_cast<BaseConstraint*>(&symbol);
    }

    /**
     * @brief Get or modify the minimum required occurrences of this character.
     *
     * <summary>
     * Returns a reference to the `minCount` field in the underlying BaseConstraint.
     * This represents the minimum number of times the character must appear in a candidate expression.
     * Using a reference allows both reading and updating the value.
     *
     * Usage:
     * @code
     * Constraint c(len, true); // digit
     * c.minCount() = 2;        // Set minimum occurrences to 2
     * int minVal = c.minCount(); // Read current value
     * @endcode
     * </summary>
     * @return Reference to minCount
     */
    int& minCount() { return base->minCount; }
    const int& minCount() const { return base->minCount; }

    /**
     * @brief Get or modify the maximum allowed occurrences of this character.
     *
     * <summary>
     * Returns a reference to the `maxCount` field in the underlying BaseConstraint.
     * This represents the maximum number of times the character can appear in a candidate expression.
     * A reference is returned for both reading and writing.
     * </summary>
     * @return Reference to maxCount
     */
    int& maxCount() { return base->maxCount; }
    const int& maxCount() const { return base->maxCount; }

    /**
     * @brief Access the vector of confirmed positions (green positions) for this character.
     *
     * <summary>
     * Returns a reference to `greenPos` in BaseConstraint.
     * Each integer in the vector represents an index in the candidate expression where this character
     * must appear (green feedback from previous guesses).
     * Modifying this vector allows adding or removing positional constraints dynamically.
     * </summary>
     * @return Reference to vector of green positions
     */
    std::vector<int>& greenPos() { return base->greenPos; }
    const std::vector<int>& greenPos() const { return base->greenPos; }

    /**
     * @brief Access the vector of banned positions for this character.
     *
     * <summary>
     * Returns a reference to `bannedPos` in BaseConstraint.
     * Each element corresponds to a position in the candidate expression.
     * - true: this character cannot appear at this position (yellow/red feedback)
     * - false: no restriction at this position
     * Modifying this vector updates positional bans for candidate filtering.
     * </summary>
     * @return Reference to vector of banned positions
     */
    std::vector<bool>& bannedPos() { return base->bannedPos; }
    const std::vector<bool>& bannedPos() const { return base->bannedPos; }

    /**
     * @brief Access or modify the conflict flag for this character.
     *
     * <summary>
     * Returns a reference to `hasConflict` in BaseConstraint.
     * This flag indicates whether conflicting feedback or an invalid state has been detected
     * for this character. For example, a character cannot simultaneously satisfy all previous feedback.
     * Modifying this flag can be used during constraint validation or candidate filtering.
     * </summary>
     * @return Reference to hasConflict
     */
    bool& hasConflict() { return base->hasConflict; }
    const bool& hasConflict() const { return base->hasConflict; }

    /**
     * @brief Access structural constraints (only relevant for symbols).
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
