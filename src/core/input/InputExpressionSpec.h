/* ----- ----- ----- ----- */
// InputExpressionSpec.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/17
// Update Date: 2025/10/17
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <unordered_set>

class InputExpressionSpec {
public:
    InputExpressionSpec() = default;
    
    static bool readLengthAndOps(int& exprLength, std::unordered_set<char>& operatorsSet);
    static bool readExpressionSpec(int& exprLength, std::unordered_set<char>& operatorsSet);  // For outside call
};
