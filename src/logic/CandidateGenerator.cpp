/* ----- ----- ----- ----- */
// CandidateGenerator.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/02
// Update Date: 2025/10/16
// Version: v2.0
/* ----- ----- ----- ----- */

#include "CandidateGenerator.h"
#include <cctype>
#include <functional>
#include <stdexcept>
#include <unordered_map>

#include "Constraint.h"
#include "ConstraintUtils.h"
#include "ExpressionValidator.h"
#include "core/constants/ExpressionConstants.h"
#include "core/logging/AppLogger.h"

#define FMT_HEADER_ONLY
#include "core.h"
#include "ranges.h"

CandidateGenerator::CandidateGenerator(ExpressionValidator& validator)
    : validator(validator) {}

// =========================
//  Internal Helper Section
// =========================
namespace {

/**
 * @brief Convert a list of tokens into a single concatenated expression string.
 *
 * @param tokensList A vector containing expression tokens.
 * @return Concatenated string representing the full expression.
 */
std::string tokenVecToString(const std::vector<Expression::Token>& tokensList) {
    std::string exprLine;
    exprLine.reserve(tokensList.size() * 4);  // Preallocate capacity (assume average token length ≈ 4)

    for (const auto& token : tokensList) {
        exprLine.append(token.value);
    }

    return exprLine;
}

} // namespace (end of internal helpers)

// =========================
//  Public Functions Section
// =========================

bool CandidateGenerator::isRhsLengthFeasible(
    int lhsLength,
    int rhsLength,
    const std::unordered_set<char>& operatorsSet
) const {
    if (lhsLength <= 0 || rhsLength <= 0) return false;

    // m = 數字塊數量 (numbers)，最多為 floor((lhsLength + 1) / 2)
    int m_max = (lhsLength + 1) / 2;
    long double bestLog = -INFINITY; // 最佳（最大）log10(value) 的上界

    // 工具 lambda：log10(10^len - 1) 精準近似
    auto log10_of_all9 = [](int len) -> long double {
        // log10(10^len - 1) = len + log10(1 - 10^{-len})
        if (len <= 0) return -INFINITY;
        long double neg = powl(10.0L, - (long double)len);
        long double adj = log10l(1.0L - neg); // 負的小數
        return (long double)len + adj;
    };

    // 遍歷可能的 number 個數 m
    for (int m = 1; m <= m_max; ++m) {
        int sumDigits = lhsLength - (m - 1); // 數字總位數（因為 m-1 個運算符各佔 1 字元）
        if (sumDigits < m) continue; // 不可能（每個數字至少 1 位）

        // 使用遞迴產生把 sumDigits 分成 m 個正整數 (composition)
        std::vector<int> parts(m, 1);

        std::function<void(int,int)> dfs = [&](int idx, int remaining) {
            if (idx == m - 1) {
                // 最後一格直接填剩餘
                parts[idx] = remaining;
                // --- 現在有一組長度分配 parts[0..m-1] ---
                // 計算各 number 的 log10 上界（用 10^len - 1）
                std::vector<long double> logs(m);
                int maxLen = 0;
                for (int i = 0; i < m; ++i) {
                    logs[i] = log10_of_all9(parts[i]);
                    if (parts[i] > maxLen) maxLen = parts[i];
                }

                // 1) 若有乘法 (*)，上界可以用 product 的 log (sum of logs)
                if (operatorsSet.count('*')) {
                    long double prodLog = 0.0L;
                    for (int i = 0; i < m; ++i) prodLog += logs[i];
                    if (prodLog > bestLog) bestLog = prodLog;
                }

                // 2) 若有加法/減法 (+ or -)，上界可估為 log10(m * 10^maxLen)
                if (operatorsSet.count('+') || operatorsSet.count('-')) {
                    // sum <= m * (10^maxLen - 1) < m * 10^maxLen
                    long double addLog = (long double)maxLen + log10l((long double)m);
                    if (addLog > bestLog) bestLog = addLog;
                }

                // 3) 若有冪 (^)，用 a^E 的上界估計：
                //    選擇最大的 base_len 與最大的 exponent_len（保守上界，不要求相鄰）
                if (operatorsSet.count('^')) {
                    int base_len = 0;
                    int exp_len  = 0;
                    for (int x : parts) {
                        if (x > base_len) {
                            exp_len = base_len;
                            base_len = x;
                        } else if (x > exp_len) {
                            exp_len = x;
                        }
                    }

                    if (base_len >= 1 && exp_len >= 1) {
                        long double baseMax = powl(10.0L, (long double)base_len) - 1.0L;
                        long double expMax  = powl(10.0L, (long double)exp_len) - 1.0L;
                        long double expLog;

                        // --- 分層估算 ---
                        if (expMax <= 20.0L) {
                            // 小次方時直接精算（常見：8^2、3^3）
                            expLog = log10l(powl(baseMax, expMax));
                        } else if (expMax <= 1e4L && baseMax <= 9.0L) {
                            // 中等次方（例如 9^9、8^8），log10 不會太大
                            expLog = expMax * log10l(baseMax);
                        } else {
                            // 超大次方 → 保守估算但避免溢位
                            expLog = (std::min)((long double)1e6L, expMax * log10l(baseMax));
                        }

                        if (expLog > bestLog) bestLog = expLog;
                    }
                }

                return;
            }

            // idx < m-1：分配一個長度給 parts[idx]（至少 1），剩下要留至少 (m-idx-1) 給後面
            for (int x = 1; x <= remaining - (m - idx - 1); ++x) {
                parts[idx] = x;
                dfs(idx + 1, remaining - x);
            }
        };

        dfs(0, sumDigits);
    } // for m

    if (bestLog <= -INFINITY/2) {
        // 沒有任何可行上界（理論上不會發生），保守回傳 false
        return false;
    }

    // 轉成位數（digits = floor(log10(value)) + 1）
    // 若 bestLog 非常大（超出 long double 可表示），我們可以直接回傳 true
    const long double LARGE_THRESHOLD = 1e18L; // 任何能到這裡的 RHS 都算可行
    if (bestLog > LARGE_THRESHOLD) return true;

    long double eps = 1e-12L;
    long long maxDigits = (long long)floor(bestLog + eps) + 1LL;

    return maxDigits >= rhsLength;
}

void CandidateGenerator::_dfsGenerateLeftTokens(
    int lhsLength,
    const std::unordered_set<char>& operatorsSet,
    std::vector<Expression::Token>& currentTokens,
    std::vector<std::vector<Expression::Token>>& lhsCandidatesList,
    std::unordered_map<char, Constraint>& lhsConstraintsMap,
    const std::vector<char>& requiredCharsAtPos,
    int dfsDepth
) {
    // Log for each depth
    /*{
        std::string operatorStr;
        for (char o : operatorsSet) operatorStr.push_back(o), operatorStr.push_back(' ');
        AppLogger::Trace(fmt::format("[_dfs, depth={}] lhsLength={}, operators=[{}]", dfsDepth, lhsLength, operatorStr));
    }*/

    // Count used length
    int usedLength = 0;
    for (const auto& t : currentTokens)
        usedLength += static_cast<int>(t.value.size());
    // Not enough length, finish recursion
    if (usedLength >= lhsLength) {
        // Only token size >= 3 makes sence, e.g. "12 + 34", "9 * 3"
        // Expression must end with digit token
        if (!currentTokens.empty() &&
            currentTokens.size() >= 3 &&
            currentTokens.back().type == Expression::TokenType::Digit) {
            lhsCandidatesList.push_back(currentTokens);
        }
        return;
    }

    // Calculate if remain required character can filled in, if not, than prune
    int remainingLength = lhsLength - usedLength;
    int totalMinRequired = 0;
    for (const auto& [ch, con] : lhsConstraintsMap) {
        int remaining = con.minCount() - con.usedCount();
        if (remaining > 0) totalMinRequired += remaining;
    }
    if (remainingLength < totalMinRequired) {
        /*AppLogger::Trace(fmt::format("[_dfs prune] Not enough space: remainingLength={} < totalMinRequired={}", 
            remainingLength, totalMinRequired));*/
        return;
    }

    // Main logic for operators and digits
    int currentPosition = usedLength;  ///< Same value as used-length, but represent the position
    auto tryAppendToken = [&](char exprChar) {
        // character-level and position-level check
        if (!ConstraintUtils::isCharAllowed(exprChar, lhsConstraintsMap))
            return;  // Check if the character should be passed
        if (!ConstraintUtils::isCharAllowedAtPos(exprChar, currentPosition, lhsConstraintsMap))
            return;  // Check if the position can fill in this character

        // Auto determin token type
        Expression::TokenType tokenType =
            (std::isdigit(static_cast<unsigned char>(exprChar)))
                ? Expression::TokenType::Digit
                : Expression::TokenType::Operator;

        // Try to get last token
        Expression::Token* lastToken = currentTokens.empty() ? nullptr : &currentTokens.back();

        // Merge check (If prev == digit && this == digit)
        bool isMerged = false;
        Expression::Token newToken{tokenType, std::string(1, exprChar)};
        // Previous token == digit && this token == digit => Merge
        if (lastToken && lastToken->type == Expression::TokenType::Digit && tokenType == Expression::TokenType::Digit) {
            // Digit start with '0' is not allowed
            if (lastToken->value.size() == 1 && lastToken->value[0] == '0') {
                return;
            }
            lastToken->value.push_back(exprChar);
            isMerged = true;
        }
        // Cannot merge, generate new token
        else {
            // Already generated, nothing to do
        }

        // Finalize previous token (when type changes or token ends)
        // Only when NOT merged (e.g., operator after digit, digit after operator)
        if (!isMerged && lastToken) {
            if (!ConstraintUtils::isTokenValid(*lastToken)) {
                // Previous token invalid => rollback (don't proceed)
                return;
            }
        }

        // Push
        if (!isMerged) {
            currentTokens.push_back({tokenType, std::string(1, exprChar)});
        } else {
            // Already merged, nothing to do
        }

        // After merge or adding new token, validate current sequence
        if (!ConstraintUtils::isTokenSequenceValid(currentTokens)) {
            // Rollback merge if invalid
            if (isMerged)
                lastToken->value.pop_back();
            else
                currentTokens.pop_back();
            return;
        }

        //AppLogger::Trace(fmt::format("[_dfs, now='{}'] Try {}, accepted", tokenVecToString(currentTokens), ch));
        if (lhsConstraintsMap.count(exprChar))
            lhsConstraintsMap[exprChar].usedCount()++;

        // Recursion
        _dfsGenerateLeftTokens(lhsLength, operatorsSet, currentTokens, lhsCandidatesList,
            lhsConstraintsMap, requiredCharsAtPos, dfsDepth + 1);

        // Backtracking
        if (lhsConstraintsMap.count(exprChar))
            lhsConstraintsMap[exprChar].usedCount()--;

        if (isMerged) {
            lastToken->value.pop_back();;
        } else {
            currentTokens.pop_back();
        }
    };
    // Check if there's green position (only fill in specific character)
    if (currentPosition < static_cast<int>(requiredCharsAtPos.size())
        && requiredCharsAtPos[currentPosition] != 0)
    {
        char greenPosChar = requiredCharsAtPos[currentPosition]; ///< The character already known green here
        /*AppLogger::Trace(fmt::format("[_dfs, now='{}'] Fixed {}, accepted due to green pos",
            tokenVecToString(currentTokens), greenPosChar));*/
        tryAppendToken(greenPosChar);
    } else {
        /*AppLogger::Trace(fmt::format("[_dfs, now='{}'] No Fixed symbol, start trying 0-9 and ops...",
            tokenVecToString(currentTokens)));*/
        // Try operators
        for (char c : operatorsSet)
            tryAppendToken(c);

        // Try digits
        for (char d : Expression::DIGIT_SYMBOLS)
            tryAppendToken(d);
    }
}

void CandidateGenerator::generateLeftTokens(
    int lhsLength,
    const std::unordered_set<char>& operatorsSet,
    std::vector<Expression::Token> currentTokens,
    std::vector<std::vector<Expression::Token>>& lhsCandidatesList,
    std::unordered_map<char, Constraint>& lhsConstraintsMap,
    int dfsDepth
) {
    // Create requiredAtPos (only 1 time)
    std::vector<char> requiredAtPosList(lhsLength, 0);
    for (const auto& [ch, con] : lhsConstraintsMap) {
        for (int gp : con.greenPos()) {
            if (gp >= 0 && gp < lhsLength) {
                if (requiredAtPosList[gp] != 0 && requiredAtPosList[gp] != ch) {
                    AppLogger::Warn(fmt::format("[Constraint] conflict at pos {}: '{}' vs '{}'",
                        gp, requiredAtPosList[gp], ch));
                }
                requiredAtPosList[gp] = ch;
            }
        }
    }

    // Logging for requiredAtPosList
    /*{
        std::string repr;
        for (int i = 0; i < (int)requiredAtPosList.size(); ++i) {
            char c = requiredAtPosList[i];
            if (c == 0)
                repr += '_';   // '_' indicated empty space (not be fixed)
            else
                repr += c;
        }
        AppLogger::Trace(fmt::format("[Constraint] requiredAtPosList = {}", repr));
    }*/

    // DFS to generate tokens
    _dfsGenerateLeftTokens(lhsLength, operatorsSet, currentTokens, lhsCandidatesList,
        lhsConstraintsMap, requiredAtPosList, dfsDepth);
}

std::vector<std::string> CandidateGenerator::generate(
    int expLength,
    const std::unordered_set<char>& operatorsSet,
    const std::vector<std::string>& expressions,
    const std::vector<std::string>& expressionColors,
    std::unordered_map<char, Constraint>& constraintsMap
) {
    std::vector<std::string> finalCandidatesList;  ///< Record possible answer(s)

    auto formatResult = [&](double val, bool isInt) -> std::string {
        if (isInt) {
            return fmt::format("{}", static_cast<long long>(std::round(val)));
        } else {
            return fmt::format("{:.2f}", val);
        }
    };

    auto tryCandidate = [&](
        const std::vector<Expression::Token>& lhsTokensList,
        int eqSignPosition,
        int lhsLength,
        int rhsLength,
        std::unordered_map<char, Constraint> constraintsMap
    ) {
        std::string lhsString = tokenVecToString(lhsTokensList);
        /*AppLogger::Trace(fmt::format("[Try eval] LHS='{}' (eqPos={}, lhsLength={})",
            lhsString, eqSignPosition, lhsLength));*/

        // Try to evaluate lhs
        double lhsResult;
        try {
            lhsResult = validator.evalExpr(lhsString);
        } catch (const std::exception& e) {
            //AppLogger::Trace(fmt::format("[Eval Fail] {} : {}", lhsString, e.what()));
            return;
        } catch (...) {
            //AppLogger::Trace(fmt::format("[Eval Fail] {} : Unknown error", lhsString));
            return;
        }

        // The answer must be integer
        bool islhsResultInt = validator.isInteger(lhsResult);
        std::string rhsString = formatResult(lhsResult, islhsResultInt);
        if (!islhsResultInt) {
            //AppLogger::Trace(fmt::format("[rhs] Reject non-integer rhs {} => {}", lhsString, rhsString));
            return;
        }
        // The answer never be negative
        if (lhsResult < 0) {
            //AppLogger::Trace(fmt::format("[rhs] Reject negative rhs {} => {}", lhsString, rhsString));
            return;
        }
        // Check if rhs length match rhsLength
        int rhsSize = rhsString.size();
        if (rhsSize != rhsLength) {
            //AppLogger::Trace(fmt::format("[rhs] {} = {} -> rhs length mismatch ({} != {})", lhsString, rhsString, rhsSize, rhsLength));
            return;
        }
        // Check if "lhs + '=' + rhs" match constraint min/max
        std::string candidateExprLine = lhsString + '=' + rhsString;
        if(!ConstraintUtils::isCandidateValid(candidateExprLine, constraintsMap)) {
            //AppLogger::Trace(fmt::format("[rhs] Reject mismatch min/max exp {} = {}", lhsString, rhsString));
            return;
        }

        //AppLogger::Trace(fmt::format("[rhs] Accept rhs: {} = {}", lhsString, rhsString));
        finalCandidatesList.push_back(candidateExprLine);
    };

    // Build constraints
    constraintsMap = deriveConstraints(expressions, expressionColors, expLength);

    // Log for constraints
    printConstraint(constraintsMap);

    // Build forbidden set (never shows up in the answer) from constraints, for more clearly meaning
    std::unordered_set<char> forbiddenSet;
    for (auto& [ch, con] : constraintsMap) {
        if (con.minCount() == 0 && con.maxCount() == 0) {
            forbiddenSet.insert(ch);
            //AppLogger::Trace(fmt::format("[Constraint] Symbol '{}' is forbidden (min=max=0)", ch));
        }
    }

    // Try to search '=' fixed location
    auto constraintsIt = constraintsMap.find('=');
    // Error handling
    if (constraintsIt == constraintsMap.end()) {
        throw std::runtime_error("Data error: Missing '=' constraint.");
    }

    const Constraint& eqSignConstraint = constraintsIt->second;

    if (eqSignConstraint.hasConflict()) {
        // '=' conflict error should be handled in deriveConstraints() function
    }

    std::vector<int> eqSignPositionsList;  ///< locations '=' might locate at

    // Check if '=' has fixed location
    if (!eqSignConstraint.greenPos().empty()) {  // Green position not empty => Has fixed location
        for (int pos : eqSignConstraint.greenPos()) {
            //AppLogger::Trace(fmt::format("[eqPos] '=' has fixed green position at {}", pos));
            eqSignPositionsList.push_back(pos);
        }
    } else {                                 // Empty => Try every possible location
        //AppLogger::Trace(fmt::format("[eqPos] eqPos does not has fixed position, will traverse it"));
        for (int eqPos = expLength - 2; eqPos >= 3; --eqPos) {
            eqSignPositionsList.push_back(eqPos);
        }
    }

    // Try to generate lhs, sort by '=' positions
    for (int eqPos : eqSignPositionsList) {
        int lhsLength = eqPos;
        int rhsLength = expLength - eqPos - 1;

        AppLogger::Debug(fmt::format("===== Processing left tokens for length {} =====", eqPos));

        // Skip impossible rhs by length feasibility
        if (!isRhsLengthFeasible(lhsLength, rhsLength, operatorsSet)) {
            AppLogger::Debug(fmt::format("[Skip eqPos={}] unrealistic rhsLength {}", eqPos, rhsLength));
            continue;
        }

        std::vector<Expression::Token> tempLhsTokenList;
        std::vector<std::vector<Expression::Token>> lhsCandidatesList;
        std::vector<std::unordered_set<char>> lhsAllowed(lhsLength);

        std::unordered_map<char, Constraint> lhsConstraintsMap = constraintsMap;

        // Adjust minCount, considering available space on the RHS
        for (auto& [ch, con] : lhsConstraintsMap) {
            int rhsAvailable = rhsLength;
            // Assume that each symbol can be filled with at most RHS space
            con.minCount() = (std::max)(0, con.minCount() - rhsAvailable);
        }

        AppLogger::Debug("===== Start to generate left tokens =====");
        // call generator with forbidden and minReq and counts
        generateLeftTokens(lhsLength, operatorsSet, tempLhsTokenList, lhsCandidatesList, lhsConstraintsMap, 0);

        AppLogger::Debug("===== Start to eval left tokens =====");
        // rest as before: eval each lhs, skip negatives, check rhs length and feedback
        for (auto& lhs : lhsCandidatesList) {
            tryCandidate(lhs, eqPos, lhsLength, rhsLength, constraintsMap);
        }
    } // for eqPos

    return finalCandidatesList;
}
