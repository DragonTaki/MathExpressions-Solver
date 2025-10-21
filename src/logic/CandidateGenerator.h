/* ----- ----- ----- ----- */
// CandidateGenerator.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/02
// Update Date: 2025/10/16
// Version: v2.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>
#include <unordered_set>
#include <vector>

#include "Constraint.h"
#include "ExpressionValidator.h"
#include "core/constants/ExpressionTokens.h"

/**
 * @class CandidateGenerator
 * @brief Generates candidate mathematical expressions based on constraints and previous feedback.
 *
 * <summary>
 * The CandidateGenerator is responsible for generating all valid expressions of a given length
 * that satisfy symbol constraints (minimum/maximum counts, green positions) derived from prior
 * expressions and color hints. It uses recursive DFS to build left-hand-side tokens, evaluates
 * expressions using ExpressionValidator, and prunes infeasible RHS lengths.
 * </summary>
 */
class CandidateGenerator {
public:
    /**
     * @brief Constructs a CandidateGenerator with a reference to an ExpressionValidator.
     * @param validator Reference to an ExpressionValidator used for safe evaluation of expressions.
     *
     * <summary>
     * The ExpressionValidator instance is used internally to calculate LHS results and check
     * validity of expressions during candidate generation.
     * </summary>
     */
    CandidateGenerator(ExpressionValidator& validator);

    /**
     * @brief Generates all valid candidate expressions of a given length.
     * @param expLength Target length of the full expression (LHS + '=' + RHS).
     * @param operatorsSet Set of allowed operators (e.g., '+', '-', '*', '/', '^').
     * @param expressions Previously guessed expressions used to derive symbol constraints.
     * @param expressionColors Corresponding color hints for each expression (green/yellow/gray).
     * @param constraintsMap Map of symbol constraints; will be updated based on input expressions and colors.
     * @return std::vector<std::string> List of generated candidate expressions satisfying all constraints.
     *
     * <summary>
     * This function:
     * - Determines feasible positions for the '=' character.
     * - Prunes impossible RHS lengths.
     * - Generates all valid LHS token sequences using DFS.
     * - Evaluates LHS expressions to produce RHS values.
     * - Filters candidates according to min/max counts and green position constraints.
     * </summary>
     */
    std::vector<std::string> generate(
        int expLength,
        const std::unordered_set<char>& operatorsSet,
        const std::vector<std::string>& expressions,
        const std::vector<std::string>& expressionColors,
        std::unordered_map<char, Constraint>& constraintsMap
    );

private:
    ExpressionValidator& validator;  ///< Reference to ExpressionValidator for evaluating expressions

    /**
     * @brief Checks whether a RHS length is feasible given a LHS length and operators.
     * @param lhsLength Length of the left-hand side expression.
     * @param rhsLength Expected length of the right-hand side (answer) expression.
     * @param operatorsSet Allowed set of operators.
     * @return true if a RHS of length `rhsLength` is theoretically feasible, false otherwise.
     *
     * <summary>
     * Computes an upper bound on the maximum possible LHS value using logarithms,
     * considering operator types and number block compositions. Used to prune impossible '=' positions.
     * </summary>
     */
    bool isRhsLengthFeasible(
        int lhsLength,
        int rhsLength,
        const std::unordered_set<char>& operatorsSet
    ) const;

    /**
     * @brief Recursive DFS function to generate all valid LHS token sequences.
     * @param lhsLength Total length of the LHS expression.
     * @param operatorsSet Set of allowed operators.
     * @param currentTokens Current token sequence under construction.
     * @param lhsCandidatesList Reference to store all generated valid LHS token sequences.
     * @param lhsConstraintsMap Symbol constraints map (min/max counts, used count, green positions).
     * @param requiredCharsAtPos Precomputed list of characters fixed at specific positions (green positions).
     * @param dfsDepth Current recursion depth (mainly for logging/debugging).
     *
     * <summary>
     * This function tries all valid digits/operators at each position, merges digit tokens when possible,
     * respects min/max symbol counts and green positions, and backtracks after recursive calls.
     * </summary>
     */
    void _dfsGenerateLeftTokens(
        int lhsLength,
        const std::unordered_set<char>& operatorsSet,
        std::vector<Expression::Token>& currentTokens,
        std::vector<std::vector<Expression::Token>>& lhsCandidatesList,
        std::unordered_map<char, Constraint>& lhsConstraintsMap,
        const std::vector<char>& requiredCharsAtPos,
        int dfsDepth
    );

    /**
     * @brief Prepares green position map and initiates DFS generation for LHS tokens.
     * @param lhsLength Length of the LHS expression.
     * @param operatorsSet Set of allowed operators.
     * @param currentTokens Initial token sequence (can be empty).
     * @param lhsCandidatesList Reference to store generated LHS token sequences.
     * @param lhsConstraintsMap Symbol constraints map (min/max, used count, green positions).
     * @param dfsDepth Initial recursion depth for DFS (usually 0).
     *
     * <summary>
     * Constructs a requiredAtPos list from green positions in constraints and then
     * calls `_dfsGenerateLeftTokens` to recursively build all valid token sequences.
     * </summary>
     */
    void generateLeftTokens(
        int lhsLength,
        const std::unordered_set<char>& operatorsSet,
        std::vector<Expression::Token> currentTokens,
        std::vector<std::vector<Expression::Token>>& lhsCandidatesList,
        std::unordered_map<char, Constraint>& lhsConstraintsMap,
        int dfsDepth
    );
};
