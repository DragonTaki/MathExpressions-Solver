/* ----- ----- ----- ----- */
// ExpressionTokens.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/16
// Update Date: 2025/10/16
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>

namespace Expression {

/**
 * @enum TokenType
 * @brief Describes the classification of a token extracted from a mathematical expression.
 *
 * <summary>
 * Every token in an expression falls into one of two categories:
 * - **Digit**: Represents one or more numeric characters forming an integer (e.g., "12", "345").
 * - **Operator**: Represents a mathematical operation (e.g., "+", "-", "*", "/").
 *
 * This distinction allows the parser or evaluator to determine whether
 * a token contributes to a value computation or defines an operation between values.
 *
 * Example usage:
 * @code
 * TokenType t1 = TokenType::Digit;    // Number token
 * TokenType t2 = TokenType::Operator; // Operator token
 * @endcode
 * </summary>
 */
enum class TokenType {
    Digit,    ///< Numeric literal token (e.g., "12", "45")
    Operator  ///< Operator token (e.g., "+", "-", "*", "/")
};

/**
 * @struct Token
 * @brief Represents a single lexical unit (token) parsed from a mathematical expression string.
 *
 * <summary>
 * A `Token` is the smallest meaningful unit in an expression — it may be a number or an operator.
 * During expression parsing, the string (e.g., `"12+34*5"`) is decomposed into a sequence of tokens:
 *
 * | Expression Part  | Token Type | Value |
 * |------------------|------------|-------|
 * | "12"             | Digit      | "12"  |
 * | "+"              | Operator   | "+"   |
 * | "34"             | Digit      | "34"  |
 * | "*"              | Operator   | "*"   |
 * | "5"              | Digit      | "5"   |
 *
 * These tokens are then used for further processing such as validation,
 * constraint checking, and evaluation.
 *
 * Example usage:
 * @code
 * Expression::Token token1 { TokenType::Digit, "123" };   // A number token
 * Expression::Token token2 { TokenType::Operator, "+" };  // An operator token
 * @endcode
 * </summary>
 */
struct Token {
    TokenType type;     ///< Type of the token: either Digit (number) or Operator (symbol)
    std::string value;  ///< The actual textual representation of the token (e.g., "123", "+")
};

} // namespace (end of Expression)

