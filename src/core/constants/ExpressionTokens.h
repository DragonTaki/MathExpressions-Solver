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
 * @brief Describes whether a token represents a numeric value or an operator.
 */
enum class TokenType {
    Digit,      ///< Numeric literal token (e.g., "12", "45")
    Operator    ///< Operator token (e.g., "+", "-", "*", "/")
};

/**
 * @struct Token
 * @brief Represents a single parsed unit in an expression.
 *
 * Tokens are parsed sequentially from an expression string,
 * and can represent either multi-digit numbers or single operator symbols.
 */
struct Token {
    TokenType type;      ///< Token category
    std::string value;   ///< Actual textual content (e.g. "123", "+")
};

} // namespace Expression
