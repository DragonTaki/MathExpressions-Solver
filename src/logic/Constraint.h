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
 * @brief Represents constraints for a single character derived from Wordle-style feedback.
 *
 * minCount: minimum number of times the character must appear in candidate
 * maxCount: maximum number of times the character may appear in candidate
 * greenPos: list of positions that must contain this character
 * bannedPos: positions where this character cannot appear
 */
struct Constraint {
    int minCount = 0;
    int maxCount = 9999;
    std::vector<int> greenPos;
    std::vector<bool> bannedPos;

    Constraint() = default;
    explicit Constraint(int len) : bannedPos(len, false) {}
};

/**
 * @brief Builds per-character constraints based on guess-feedback pairs.
 *
 * @param expressions Vector of guessed strings.
 * @param colors Vector of corresponding feedback strings ('g', 'y', 'r').
 * @param length Total length of equation (for bannedPos vector sizing).
 * @return Map from character to its Constraint.
 */
std::unordered_map<char, Constraint> deriveConstraints(
    const std::vector<std::string>& expressions,
    const std::vector<std::string>& colors,
    int length
);

/**
 * @brief Checks whether a candidate equation satisfies a given guess-feedback pair.
 *
 * @param candidate Candidate string (e.g. "12+46=58")
 * @param expression Guessed string
 * @param color Feedback string ('g', 'y', 'r')
 * @return true if feedback matches Wordle rules; false otherwise.
 */
bool matchesFeedback(
    const std::string& candidate,
    const std::string& expression,
    const std::string& color,
    const std::unordered_set<char>& allowedOps
);
