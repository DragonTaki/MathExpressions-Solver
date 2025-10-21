/* ----- ----- ----- ----- */
// ExpressionConstants.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/16
// Update Date: 2025/10/16
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <array>
#include <unordered_set>

/**
 * @file ExpressionConstants.h
 * @brief Defines reusable character sets and lookup tables for expression parsing and constraint checking.
 *
 * <summary>
 * This file provides constant arrays and unordered_sets used throughout the expression solver for:
 *  - Valid mathematical operators
 *  - Digits
 *  - All valid symbols (union of operators and digits)
 *  - Feedback colors used in Wordle-style evaluation ('g', 'y', 'r')
 *
 * All constants are defined in the Expression namespace for modular usage.
 * </summary>
 */
namespace Expression {

/**
 * @brief Mathematical operator symbols used in expressions.
 *
 * Contains the standard arithmetic operators and equality:
 * - '+', '-', '*', '/', '^', '='
 */
inline constexpr std::array<char, 6> OPERATOR_SYMBOLS = {
    '+','-','*','/','^','='
};

/**
 * @brief Helper constexpr function to generate digit characters at compile-time.
 *
 * Generates an array containing characters '0' through 'N-1' + '0'.
 *
 * @tparam N Number of digits to generate
 * @return std::array<char, N> Array of digit characters
 */
template <size_t N>
constexpr std::array<char, N> makeDigitSymbols() {
    std::array<char, N> arr{};
    for (size_t i = 0; i < N; ++i) {
        arr[i] = '0' + static_cast<char>(i);
    }
    return arr;
}

/**
 * @brief Digits used in mathematical expressions.
 *
 * Contains characters '0'–'9' as a constexpr array.
 */
inline constexpr auto DIGIT_SYMBOLS = makeDigitSymbols<10>();

/**
 * @brief Helper constexpr function to concatenate two arrays at compile-time.
 *
 * @tparam T Type of array elements
 * @tparam N1 Size of first array
 * @tparam N2 Size of second array
 * @param a1 First array
 * @param a2 Second array
 * @return std::array<T, N1+N2> Concatenated array containing elements of a1 followed by a2
 */
template <typename T, size_t N1, size_t N2>
constexpr std::array<T, N1 + N2> concatArrays(const std::array<T, N1>& a1,
                                                const std::array<T, N2>& a2) {
    std::array<T, N1 + N2> result{};
    for (size_t i = 0; i < N1; ++i) result[i] = a1[i];
    for (size_t i = 0; i < N2; ++i) result[N1 + i] = a2[i];
    return result;
}

/**
 * @brief All valid symbols in expressions (operators + digits).
 *
 * Concatenates OPERATOR_SYMBOLS and DIGIT_SYMBOLS into a single constexpr array.
 */
inline constexpr std::array<char, 16> SYMBOLS = concatArrays(OPERATOR_SYMBOLS, DIGIT_SYMBOLS);

/**
 * @brief Feedback colors for Wordle-style evaluation of expressions.
 *
 * 'g' = green, character is correct and in correct position  
 * 'y' = yellow, character exists but wrong position  
 * 'r' = red, character does not exist
 */
inline constexpr std::array<char, 3> FEEDBACK_COLORS = {'g', 'y', 'r'};


/**
 * @brief Lookup sets for fast membership checks.
 *
 * Using unordered_set for O(1) lookup of operators, digits, symbols, and feedback colors.
 */
inline const std::unordered_set<char> OPERATOR_LOOKUP(
    OPERATOR_SYMBOLS.begin(), OPERATOR_SYMBOLS.end()  ///< Initialize from array
);
inline const std::unordered_set<char> DIGIT_LOOKUP(
    DIGIT_SYMBOLS.begin(), DIGIT_SYMBOLS.end()        ///< Initialize from array
);
inline const std::unordered_set<char> SYMBOL_LOOKUP(
    SYMBOLS.begin(), SYMBOLS.end()                    ///< Initialize from array
);

inline const std::unordered_set<char> FEEDBACK_COLOR_LOOKUP(
    FEEDBACK_COLORS.begin(), FEEDBACK_COLORS.end()    ///< Initialize from array
);

}  // namespace (end of Expression)
