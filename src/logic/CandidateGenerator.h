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

class CandidateGenerator {
public:
    CandidateGenerator(ExpressionValidator& validator);

    std::vector<std::string> generate(
        int expLength,
        const std::unordered_set<char>& operatorsSet,
        const std::vector<std::string>& expressions,
        const std::vector<std::string>& expressionColors,
        std::unordered_map<char, Constraint>& constraintsMap
    );

private:
    ExpressionValidator& validator;

    bool isRhsLengthFeasible(
        int lhsLength,
        int rhsLength,
        const std::unordered_set<char>& operatorsSet
    ) const;

    void _dfsGenerateLeftTokens(
        int lhsLength,
        const std::unordered_set<char>& operatorsSet,
        std::vector<Expression::Token>& currentTokens,
        std::vector<std::vector<Expression::Token>>& lhsCandidatesList,
        std::unordered_map<char, Constraint>& lhsConstraintsMap,
        const std::vector<char>& requiredCharsAtPos,
        int dfsDepth
    );

    void generateLeftTokens(
        int lhsLength,
        const std::unordered_set<char>& operatorsSet,
        std::vector<Expression::Token> currentTokens,
        std::vector<std::vector<Expression::Token>>& lhsCandidatesList,
        std::unordered_map<char, Constraint>& lhsConstraintsMap,
        int dfsDepth
    );
};
