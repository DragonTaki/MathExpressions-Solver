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
 * @struct Constraint
 * @brief Represents constraints for a single character derived from Wordle-style feedback.
 *
 * <summary>
 * Each character in the candidate equation can have:
 * - A minimum count (minCount) that it must appear.
 * - A maximum count (maxCount) that it cannot exceed.
 * - Green positions (greenPos) where the character must appear.
 * - Banned positions (bannedPos) where the character cannot appear.
 * This is useful for filtering candidate strings based on previous guesses.
 * </summary>
 */
struct Constraint {
    int minCount = 0;                      ///< Minimum required occurrences of this character
    int maxCount = 9999;                   ///< Maximum allowed occurrences of this character
    std::vector<int> greenPos;             ///< Positions that must contain this character (0-based)
    std::vector<bool> bannedPos;           ///< Positions where this character cannot appear

    Constraint() = default;

    /**
     * @brief Constructor that initializes bannedPos vector for a given length.
     * @param len Length of the candidate string (size of bannedPos vector)
     */
    explicit Constraint(int len) : bannedPos(len, false) {}
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
