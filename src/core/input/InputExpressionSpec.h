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

/**
 * @class InputExpressionSpec
 * @brief Handles reading user specifications for mathematical expressions.
 * 
 * This class provides functions to:
 *   1. Prompt the user for the length of the expression.
 *   2. Prompt the user for the available operators.
 *   3. Validate the inputs and ensure that required operators (e.g., '+') are included.
 * 
 * Typically used to set up a game or puzzle configuration.
 */
class InputExpressionSpec {
public:
    InputExpressionSpec() = default;
    
    /**
     * @brief Read the expression length and available operators from the user.
     * 
     * The user is prompted to enter a line containing the expression length followed
     * by a list of operators, e.g., "8 + - *" or "8+-*". The function validates
     * the length (must be >= 5) and operators (must include '+' and only valid operators).
     * 
     * @param[out] exprLength Reference to store the parsed expression length.
     * @param[out] operatorsSet Reference to store the parsed set of valid operators.
     * @return true If a valid length and operator set were successfully read.
     * @return false If the input stream ends before valid input is provided.
     */
    static bool readLengthAndOps(int& exprLength, std::unordered_set<char>& operatorsSet);

    /**
     * @brief High-level wrapper to read the full expression specification.
     * 
     * Currently, this function simply calls `readLengthAndOps` to get the expression
     * length and available operators from the user.
     * 
     * @param[out] exprLength Reference to store the parsed expression length.
     * @param[out] operatorsSet Reference to store the parsed set of valid operators.
     * @return true If a valid expression specification was successfully read.
     * @return false If the input stream ends before valid input is provided.
     */
    static bool readExpressionSpec(int& exprLength, std::unordered_set<char>& operatorsSet);  ///< For external calls
};
