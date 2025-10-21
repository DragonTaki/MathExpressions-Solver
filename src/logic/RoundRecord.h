/* ----- ----- ----- ----- */
// RoundRecord.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/18
// Update Date: 2025/10/18
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>
#include <vector>

/**
 * @file RoundRecord.h
 * @brief Defines the data structure for recording a single round's player input
 *        and corresponding feedback state within the expression-solving game.
 *
 * <summary>
 * Each round of gameplay involves a player's attempted expression (e.g., `"12+3*4"`)
 * and the color-coded feedback that indicates correctness or positional accuracy,
 * similar to games like Wordle.  
 *
 * The `RoundRecord` structure encapsulates these two key pieces of data:
 * - The **raw input expression** entered by the player.
 * - The **color feedback string**, where each color represents match state
 *   (`'g'` for correct position, `'y'` for wrong position but correct symbol,
 *   `'r'` for incorrect symbol, etc.).
 *
 * Instances of this struct are typically stored in a history container (e.g., `std::vector<RoundRecord>`)
 * to represent the sequence of attempts made by a player throughout a game session.
 * </summary>
 */
struct RoundRecord {

    /**
     * @brief The player's guessed mathematical expression for this round.
     *
     * This string stores the raw input entered by the player (e.g., `"12+34/5"`).
     * It is used for display, validation, and later analysis when comparing against
     * the target expression.
     */
    std::string exprLine;       ///< Player-guessed expression

    /**
     * @brief The feedback color string corresponding to `exprLine`.
     *
     * Each character in this string represents the feedback state for the symbol
     * at the same position in `exprLine`.  
     * Typical encodings:
     * - `'g'`: Correct symbol and position (green).
     * - `'y'`: Correct symbol but wrong position (yellow).
     * - `'r'`: Symbol does not appear in the solution (red or gray).
     *
     * This information helps guide the player’s next move.
     */
    std::string exprColorLine;  ///< Game-feedback color
};
