/* ----- ----- ----- ----- */
// CandidateGenerator.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/02
// Update Date: 2025/10/02
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>
#include <vector>
#include <unordered_set>

#include "Constraint.h"
#include "ExpressionValidator.h"

class CandidateGenerator {
public:
    CandidateGenerator(ExpressionValidator& validator);

    std::vector<std::string> generate(
        int length,
        const std::unordered_set<char>& operators,
        const std::vector<std::string>& expressions,
        const std::vector<std::string>& colors
    );

private:
    ExpressionValidator& validator;

    bool matchesFeedback(
        const std::string& candidate,
        const std::string& expression,
        const std::string& color
    );

    bool isRhsLengthFeasible(
        int lhsLen,
        int rhsLen,
        const std::unordered_set<char>& operators
    ) const;

    void generateLeftTokens(
        int lhsLen,
        const std::unordered_set<char>& operators,
        std::string& current,
        std::vector<std::string>& lhsCandidates,
        const std::vector<std::unordered_set<char>>* allowed,
        const std::unordered_map<char, Constraint>* lhsConstraintsMap,
        int depth
    );
};
