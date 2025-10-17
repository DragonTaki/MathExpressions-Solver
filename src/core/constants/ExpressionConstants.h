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
 * @brief Defines reusable character sets for digits, operators, and general symbols.
 *
 * <summary>
 * Provides constant unordered_sets used throughout the expression solver:
 * - OPERATOR_SYMBOLS: '+', '-', '*', '/', '^', '='
 * - DIGIT_SYMBOLS:    characters '0'–'9'
 * - SYMBOLS:          union of digits and operators
 * </summary>
 */
namespace Expression {

    /// Operators used in mathematical expressions
    inline constexpr std::array<char, 6> OPERATOR_SYMBOLS = {
        '+','-','*','/','^','='
    };

    // Helper constexpr function to concatenate arrays at compile-time
    template <size_t N>
    constexpr std::array<char, N> makeDigitSymbols() {
        std::array<char, N> arr{};
        for (size_t i = 0; i < N; ++i) {
            arr[i] = '0' + static_cast<char>(i);
        }
        return arr;
    }

    /// Digits used in mathematical expressions
    inline constexpr auto DIGIT_SYMBOLS = makeDigitSymbols<10>();

    
    // Helper constexpr function to concatenate arrays at compile-time
    template <typename T, size_t N1, size_t N2>
    constexpr std::array<T, N1 + N2> concatArrays(const std::array<T, N1>& a1,
                                                  const std::array<T, N2>& a2) {
        std::array<T, N1 + N2> result{};
        for (size_t i = 0; i < N1; ++i) result[i] = a1[i];
        for (size_t i = 0; i < N2; ++i) result[N1 + i] = a2[i];
        return result;
    }

    /// Union of all valid symbols (operators + digits)
    inline constexpr std::array<char, 16> SYMBOLS = concatArrays(OPERATOR_SYMBOLS, DIGIT_SYMBOLS);

    /// Expression feedback colors
    inline constexpr std::array<char, 3> FEEDBACK_COLORS = {'g', 'y', 'r'};


    // Lookup sets
    inline const std::unordered_set<char> OPERATOR_LOOKUP(
        OPERATOR_SYMBOLS.begin(), OPERATOR_SYMBOLS.end()
    );
    inline const std::unordered_set<char> DIGIT_LOOKUP(
        DIGIT_SYMBOLS.begin(), DIGIT_SYMBOLS.end()
    );
    inline const std::unordered_set<char> SYMBOL_LOOKUP(
        SYMBOLS.begin(), SYMBOLS.end()
    );

    inline const std::unordered_set<char> FEEDBACK_COLOR_LOOKUP(
        FEEDBACK_COLORS.begin(), FEEDBACK_COLORS.end()
    );
}
