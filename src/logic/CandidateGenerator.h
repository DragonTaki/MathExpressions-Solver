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

namespace Expr {
    enum class TokenType { Digit, Operator };

    struct Token {
        TokenType type;
        std::string value; // 對於數字可能多位
    };
}

class CandidateGenerator {
public:
    CandidateGenerator(ExpressionValidator& validator);

    std::vector<std::string> generate(
        int length,
        const std::unordered_set<char>& operators,
        const std::vector<std::string>& expressions,
        const std::vector<std::string>& colors);

private:
    ExpressionValidator& validator;

    bool isRhsLengthFeasible(
        int lhsLen,
        int rhsLen,
        const std::unordered_set<char>& operators) const;

    void _dfsGenerateLeftTokens(
        int lhsLen,
        const std::unordered_set<char>& operators,
        std::vector<Expr::Token>& current,
        std::vector<std::vector<Expr::Token>>& lhsCandidates,
        std::unordered_map<char, Constraint>* lhsConstraintsMap,
        const std::vector<char>& requiredAtPos,
        int depth);

    void generateLeftTokens(
        int lhsLen,
        const std::unordered_set<char>& operators,
        std::vector<Expr::Token> current,
        std::vector<std::vector<Expr::Token>>& lhsCandidates,
        std::unordered_map<char, Constraint>* lhsConstraintsMap,
        int depth);
};
