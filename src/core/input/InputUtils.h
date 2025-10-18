/* ----- ----- ----- ----- */
// InputUtils.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/18
// Update Date: 2025/10/18
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>

namespace InputUtils {

bool isValidOperator(char exprChar);
bool isValidColor(const std::string& exprColorLine, int exprLength);

} // namespace (end of InputUtils)
