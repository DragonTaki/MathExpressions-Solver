/* ----- ----- ----- ----- */
// InputUtils.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/18
// Update Date: 2025/10/18
// Version: v1.0
/* ----- ----- ----- ----- */

#include "InputUtils.h"
#include <cctype>
#include <string>

#include "core/constants/ExpressionConstants.h"

namespace InputUtils {

bool isValidOperator(char exprChar) {
    return Expression::OPERATOR_LOOKUP.count(exprChar) > 0;
}

bool isValidColor(const std::string& exprColorLine, int exprLength) {
    if ((int)exprColorLine.size() != exprLength) return false;
    for (char exprColorChar : exprColorLine) {
        char loweredColor = std::tolower(static_cast<unsigned char>(exprColorChar));
        if (!Expression::FEEDBACK_COLOR_LOOKUP.count(loweredColor)) return false;
    }
    return true;
}

} // namespace (end of InputUtils)
