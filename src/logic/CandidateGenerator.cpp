/* ----- ----- ----- ----- */
// CandidateGenerator.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/02
// Update Date: 2025/10/16
// Version: v2.0
/* ----- ----- ----- ----- */

#include <cctype>
#include <functional>
#include <stdexcept>
#include <unordered_map>

#include "CandidateGenerator.h"
#include "Constraint.h"
#include "ExpressionValidator.h"
#include "core/AppLogger.h"

#define FMT_HEADER_ONLY
#include "core.h"
#include "ranges.h"

using namespace std;

CandidateGenerator::CandidateGenerator(ExpressionValidator& validator)
    : validator(validator) {}

std::string tokenVecToString(const std::vector<Expr::Token>& tokens) {
    std::string s;
    for (auto& t : tokens) s += t.value;
    return s;
}

bool CandidateGenerator::isRhsLengthFeasible(
    int lhsLen,
    int rhsLen,
    const std::unordered_set<char>& operators
) const {
    if (lhsLen <= 0 || rhsLen <= 0) return false;

    // m = 數字塊數量 (numbers)，最多為 floor((lhsLen + 1) / 2)
    int m_max = (lhsLen + 1) / 2;
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
        int sumDigits = lhsLen - (m - 1); // 數字總位數（因為 m-1 個運算符各佔 1 字元）
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
                if (operators.count('*')) {
                    long double prodLog = 0.0L;
                    for (int i = 0; i < m; ++i) prodLog += logs[i];
                    if (prodLog > bestLog) bestLog = prodLog;
                }

                // 2) 若有加法/減法 (+ or -)，上界可估為 log10(m * 10^maxLen)
                if (operators.count('+') || operators.count('-')) {
                    // sum <= m * (10^maxLen - 1) < m * 10^maxLen
                    long double addLog = (long double)maxLen + log10l((long double)m);
                    if (addLog > bestLog) bestLog = addLog;
                }

                // 3) 若有冪 (^)，用 a^E 的上界估計：
                //    選擇最大的 base_len 與最大的 exponent_len（保守上界，不要求相鄰）
                if (operators.count('^')) {
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

    return maxDigits >= rhsLen;
}

void CandidateGenerator::_dfsGenerateLeftTokens(
    int lhsLen,
    const std::unordered_set<char>& operators,
    std::vector<Expr::Token>& current,
    std::vector<std::vector<Expr::Token>>& lhsCandidates,
    std::unordered_map<char, Constraint>& lhsConstraintsMap,
    const std::vector<char>& requiredAtPos,
    int depth
) {
    // Log for each depth
    /*{
        std::string ops;
        for (char o : operators) ops.push_back(o), ops.push_back(' ');
        AppLogger::Trace(fmt::format("[_dfs, depth={}] lhsLen={}, operators=[{}]", depth, lhsLen, ops));
    }*/

    // Count used length
    int usedLength = 0;
    for (const auto& t : current)
        usedLength += static_cast<int>(t.value.size());
    // Not enough length, finish recursion
    if (usedLength >= lhsLen) {
        // Only token size >= 3 makes sence, e.g. "12 + 34", "9 * 3"
        // Expression must end with digit token
        if (!current.empty() &&
            current.size() >= 3 &&
            current.back().type == Expr::TokenType::Digit) {
            lhsCandidates.push_back(current);
        }
        return;
    }

    // Calculate if remain required charactor can filled in, if not, than prune
    int remainingLength = lhsLen - usedLength;
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

    // Charactor-level check
    auto isCharAllowed = [&](char ch) -> bool {
        auto it = lhsConstraintsMap.find(ch);

        // Error handling, should not happened
        if (it == lhsConstraintsMap.end()) {
            //AppLogger::Trace(fmt::format("[_dfs, now='{}'] Try {}, passed: not exist in constraints map", tokenVecToString(current), ch));
            return false;
        }

        const Constraint& con = it->second;

        // Only got 'r' color, not contained in answer
        if (con.minCount() == 0 && con.maxCount() == 0) {
            //AppLogger::Trace(fmt::format("[_dfs, now='{}'] Try {}, skipped: forbidden (min=max=0)", tokenVecToString(current), ch));
            return false;
        }

        // Got some 'g'/'y', and has 'r' to restrict the max use
        if (con.usedCount() >= con.maxCount()) {
            //AppLogger::Trace(fmt::format("[_dfs, now='{}'] Try {}, skipped: Reach max usage limit {}", tokenVecToString(current), ch, con.maxCount()));
            return false;
        }

        return true;
    };

    // Position-level check
    auto isCharAllowedAtPos = [&](char ch, int pos) -> bool {
        auto it = lhsConstraintsMap.find(ch);

        // The charactor got 'y' or 'r' at this position, means not allowed here
        if (it != lhsConstraintsMap.end()) {
            const Constraint& con = it->second;
            if (con.bannedPos().count(pos)) {
                //AppLogger::Trace(fmt::format("[_dfs, now='{}'] Try {}, skipped: Disallowed at pos {}", tokenVecToString(current), ch, pos));
                return false;
            }
        }

        return true;
    };

    // Token-level check
    auto isTokenValid = [&](const Expr::Token& token) -> bool {
        if (token.type == Expr::TokenType::Digit) {
            const std::string& v = token.value;

            // Error handling: Token couldn't be empty (shouldn't be here)
            if (v.empty()) {
                return false;
            }

            // Digit token should not be "0", or start with '0' 
            if (v[0] == '0') {
                return false;
            }

            // Check if all digits (shouldn't be here)
            for (char c : v) {
                if (!std::isdigit(static_cast<unsigned char>(c)))
                    return false;
            }

            return true;
        }
        // No check for operator so far
        // Single charactor checking already done in isCharAllowed() and isCharAllowedAtPos()

        return true;
    };

    // Token-level check
    auto isTokenSequenceValid = [&](const std::vector<Expr::Token>& tokens) -> bool {
        if (tokens.empty()) return false;

        const Expr::Token& last = tokens.back();         ///< Last token
        const Expr::Token* prev = tokens.size() >= 2 ?   ///< Previous token
            &tokens[tokens.size() - 2] : nullptr;
        const Expr::Token* prev2 = tokens.size() >= 3 ?  ///< Previous' previous token
            &tokens[tokens.size() - 3] : nullptr;

        // Operator cannot be first charactor
        if (tokens.size() == 1 && last.type == Expr::TokenType::Operator)
            return false;
        
        // Operator logic check
        if (last.type == Expr::TokenType::Operator) {

            // Operator follow another operator is not allowed
            if (prev && prev->type == Expr::TokenType::Operator) {
                return false;
            }

            // Consecutive factorials are not allowed
            if (prev2 && prev2->value == "^" && last.value == "^") {
                return false;
            }
        }
        // Digit logic check
        else {
            // '0' cannot follow '/'
            if (!tokens.empty() && tokens.back().value == "/" && last.value == "0") {
                return false;
            }
        }

        return true;
    };

    // Main logic for operators and digits
    int pos = usedLength;  ///< Same value as used-length, but represent the position
    auto tryAppendToken = [&](char ch) {
        // Charactor-level and position-level check
        if (!isCharAllowed(ch)) return;           // Check if the charactor should be passed
        if (!isCharAllowedAtPos(ch, pos)) return; // Check if the position can fill in this charactor

        // Auto determin token type
        Expr::TokenType type =
            (std::isdigit(static_cast<unsigned char>(ch)))
                ? Expr::TokenType::Digit
                : Expr::TokenType::Operator;

        // Try to get last token
        Expr::Token* last = current.empty() ? nullptr : &current.back();

        // Merge check (If prev == digit && this == digit)
        bool isMerged = false;
        Expr::Token newToken{type, std::string(1, ch)};
        // Previous token == digit && this token == digit => Merge
        if (last && last->type == Expr::TokenType::Digit && type == Expr::TokenType::Digit) {
            // Digit start with '0' is not allowed
            if (last->value.size() == 1 && last->value[0] == '0') {
                return;
            }
            last->value.push_back(ch);
            isMerged = true;
        }
        // Cannot merge, generate new token
        else {
            // Already generated, nothing to do
        }

        // Finalize previous token (when type changes or token ends)
        // Only when NOT merged (e.g., operator after digit, digit after operator)
        if (!isMerged && last) {
            if (!isTokenValid(*last)) {
                // Previous token invalid => rollback (don't proceed)
                return;
            }
        }

        // Push
        if (!isMerged) {
            current.push_back({type, std::string(1, ch)});
        } else {
            // Already merged, nothing to do
        }

        // After merge or adding new token, validate current sequence
        if (!isTokenSequenceValid(current)) {
            // Rollback merge if invalid
            if (isMerged)
                last->value.pop_back();
            else
                current.pop_back();
            return;
        }

        //AppLogger::Trace(fmt::format("[_dfs, now='{}'] Try {}, accepted", tokenVecToString(current), ch));
        if (lhsConstraintsMap.count(ch)) lhsConstraintsMap[ch].usedCount()++;

        // Recursion
        _dfsGenerateLeftTokens(lhsLen, operators, current, lhsCandidates,
                            lhsConstraintsMap, requiredAtPos, depth + 1);

        // Backtracking
        if (lhsConstraintsMap.count(ch)) lhsConstraintsMap[ch].usedCount()--;
        if (isMerged) {
            last->value.pop_back();;
        } else {
            current.pop_back();
        }
    };

    // Check if there's green position (only fill in specific charactor)
    if (pos < (int)requiredAtPos.size() && requiredAtPos[pos] != 0) {
        char fixedChar = requiredAtPos[pos];  ///< The charactor already known green here
        //AppLogger::Trace(fmt::format("[_dfs, now='{}'] Fixed {}, accepted due to green pos", tokenVecToString(current), fixedChar));
        Expr::TokenType type =
            (operators.count(fixedChar) ? Expr::TokenType::Operator : Expr::TokenType::Digit);
        tryAppendToken(fixedChar);
    } else {
        //AppLogger::Trace(fmt::format("[_dfs, now='{}'] No Fixed symbol, start trying 0-9 and ops...", tokenVecToString(current)));
        // Try operators
        for (char op : operators)
            tryAppendToken(op);

        // Try digits
        for (char ch = '0'; ch <= '9'; ++ch)
            tryAppendToken(ch);
    }
}

void CandidateGenerator::generateLeftTokens(
    int lhsLen,
    const std::unordered_set<char>& operators,
    std::vector<Expr::Token> current,
    std::vector<std::vector<Expr::Token>>& lhsCandidates,
    std::unordered_map<char, Constraint>& lhsConstraintsMap,
    int depth
) {
    // Create requiredAtPos (only 1 time)
    std::vector<char> requiredAtPos(lhsLen, 0);
    for (const auto& [ch, con] : lhsConstraintsMap) {
        for (int gp : con.greenPos()) {
            if (gp >= 0 && gp < lhsLen) {
                if (requiredAtPos[gp] != 0 && requiredAtPos[gp] != ch) {
                    AppLogger::Warn(fmt::format("[Constraint] conflict at pos {}: '{}' vs '{}'",
                        gp, requiredAtPos[gp], ch));
                }
                requiredAtPos[gp] = ch;
            }
        }
    }

    // Logging for requiredAtPos
    /*{
        std::string repr;
        for (int i = 0; i < (int)requiredAtPos.size(); ++i) {
            char c = requiredAtPos[i];
            if (c == 0)
                repr += '_';   // '_' indicated empty space (not be fixed)
            else
                repr += c;
        }
        AppLogger::Trace(fmt::format("[Constraint] requiredAtPos = {}", repr));
    }*/

    // DFS to generate tokens
    _dfsGenerateLeftTokens(lhsLen, operators, current, lhsCandidates,
                           lhsConstraintsMap, requiredAtPos, depth);
}

bool CandidateGenerator::isCandidateValid(
    const std::string& expr,
    const std::unordered_map<char, Constraint>& constraints
) {
    // Count every charactor occurrences in expression
    std::unordered_map<char, int> countMap;
    for (char c : expr) {
        countMap[c]++;
    }

    // Compare with constraint
    for (const auto& [ch, con] : constraints) {
        int cnt = countMap[ch];  // Default to 0 if not exist
        if (cnt < con.minCount() || cnt > con.maxCount()) {
            return false;
        }
    }

    return true;
}

std::vector<string> CandidateGenerator::generate(
    int length,
    const std::unordered_set<char>& operators,
    const std::vector<string>& expressions,
    const std::vector<string>& colors
) {
    std::vector<string> finalCandidates;  ///< Record possible answer(s)

    auto formatResult = [&](double val, bool isInt) -> std::string {
        if (isInt) {
            return fmt::format("{}", static_cast<long long>(std::round(val)));
        } else {
            return fmt::format("{:.2f}", val);
        }
    };

    auto tryCandidate = [&](const std::vector<Expr::Token>& lhsTokens,
        int eqPos,
        int lhsLen,
        int rhsLen,
        std::unordered_map<char, Constraint> constraints
    ) {
        std::string lhsString = tokenVecToString(lhsTokens);
        //AppLogger::Trace(fmt::format("[Try eval] LHS='{}' (eqPos={}, lhsLen={})", lhsString, eqPos, lhsLen));

        // Try to evaluate lhs
        double lhsResult;
        try {
            lhsResult = validator.evalExpr(lhsString);
        } catch (const std::exception& e) {
            AppLogger::Trace(fmt::format("[Eval Fail] {} : {}", lhsString, e.what()));
            return;
        } catch (...) {
            AppLogger::Trace(fmt::format("[Eval Fail] {} : unknown error", lhsString));
            return;
        }

        // The answer must be integer
        bool isInt = validator.isInteger(lhsResult);
        std::string rhsString = formatResult(lhsResult, isInt);
        if (!isInt) {
            AppLogger::Trace(fmt::format("[rhs] Reject non-integer rhs {} => {}", lhsString, rhsString));
            return;
        }
        // The answer never be negative
        if (lhsResult < 0) {
            AppLogger::Trace(fmt::format("[rhs] Reject negative rhs {} => {}", lhsString, rhsString));
            return;
        }
        // Check if rhs length match rhsLen
        int rhsSize = rhsString.size();
        if (rhsSize != rhsLen) {
            AppLogger::Trace(fmt::format("[rhs] {} = {} -> rhs length mismatch ({} != {})", lhsString, rhsString, rhsSize, rhsLen));
            return;
        }
        // Check if "lhs + '=' + rhs" match constraint min/max
        std::string candidate = lhsString + '=' + rhsString;
        if(!isCandidateValid(candidate, constraints)) {
            AppLogger::Trace(fmt::format("[rhs] Reject mismatch min/max exp {} = {}", lhsString, rhsString));
            return;
        }

        AppLogger::Debug(fmt::format("[rhs] Accept rhs: {} = {}", lhsString, rhsString));
        finalCandidates.push_back(candidate);
    };

    // Build constraints
    std::unordered_map<char, Constraint> constraintsMap = deriveConstraints(expressions, colors, length);

    // Log for constraints
    {
        AppLogger::Debug("===== Derived Constraints =====");
        for (auto& kv : constraintsMap) {
            const auto& symbol = kv.first;
            const auto& cst    = kv.second;

            std::string greenStr;
            for (int pos : cst.greenPos()) greenStr += std::to_string(pos) + " ";

            std::string bannedStr;
            for (int pos : cst.bannedPos()) bannedStr += std::to_string(pos) + " ";

            AppLogger::Debug(fmt::format(
                "  Symbol: {} | MinCount: {} | MaxCount: {} | GreenPos: {{{}}} | BannedPos: {{{}}} | Conflict: {}",
                symbol,
                cst.minCount(),
                cst.maxCount(),
                greenStr,
                bannedStr,
                cst.hasConflict() ? "YES" : "NO"
            ));
        }
    }

    // Build forbidden set (never shows up in the answer) from constraints, for more clearly meaning
    std::unordered_set<char> forbidden;
    for (auto& [ch, con] : constraintsMap) {
        if (con.minCount() == 0 && con.maxCount() == 0) {
            forbidden.insert(ch);
            AppLogger::Debug(fmt::format("[Constraint] Symbol '{}' is forbidden (min=max=0)", ch));
        }
    }

    // Try to search '=' fixed location
    auto it = constraintsMap.find('=');
    // Error handling
    if (it == constraintsMap.end()) {
        throw std::runtime_error("Data error: Missing '=' constraint.");
    }

    const Constraint& eqConstraint = it->second;

    if (eqConstraint.hasConflict()) {
        // '=' conflict error should be handled in deriveConstraints() function
    }

    std::vector<int> eqPositions;  ///< locations '=' might locate at

    // Check if '=' has fixed location
    if (!eqConstraint.greenPos().empty()) {  // Green position not empty => Has fixed location
        for (int pos : eqConstraint.greenPos()) {
            AppLogger::Debug(fmt::format("[eqPos] '=' has fixed green position at {}", pos));
            eqPositions.push_back(pos);
        }
    } else {                                 // Empty => Try every possible location
        AppLogger::Debug(fmt::format("[eqPos] eqPos does not has fixed position, will traverse it"));
        for (int eqPos = length - 2; eqPos >= 3; --eqPos) {
            eqPositions.push_back(eqPos);
        }
    }

    // Try to generate lhs, sort by '=' positions
    for (int eqPos : eqPositions) {
        int lhsLen = eqPos;
        int rhsLen = length - eqPos - 1;

        AppLogger::Debug("===== ===== ===== ===== ===== =====");
        AppLogger::Debug(fmt::format("===== Processing left tokens for length {} =====", eqPos));
        AppLogger::Debug("===== ===== ===== ===== ===== =====");

        // Skip impossible rhs by length feasibility
        if (!isRhsLengthFeasible(lhsLen, rhsLen, operators)) {
            AppLogger::Debug(fmt::format("[Skip eqPos={}] unrealistic rhsLen {}", eqPos, rhsLen));
            continue;
        }

        std::vector<Expr::Token> tempLhsTokenVector;
        std::vector<std::vector<Expr::Token>> lhsCandidates;
        vector<std::unordered_set<char>> lhsAllowed(lhsLen);

        std::unordered_map<char, Constraint> lhsConstraintsMap = constraintsMap;

        // Adjust minCount, considering available space on the RHS
        for (auto& [ch, con] : lhsConstraintsMap) {
            int rhsAvailable = rhsLen;
            // Assume that each symbol can be filled with at most RHS space
            con.minCount() = (std::max)(0, con.minCount() - rhsAvailable);
        }

        AppLogger::Debug("===== ===== ===== ===== ===== =====");
        AppLogger::Debug("===== Start to generate left tokens =====");
        AppLogger::Debug("===== ===== ===== ===== ===== =====");
        // call generator with forbidden and minReq and counts
        generateLeftTokens(lhsLen, operators, tempLhsTokenVector, lhsCandidates, lhsConstraintsMap, 0);

        AppLogger::Debug("===== ===== ===== ===== ===== =====");
        AppLogger::Debug("===== Start to eval left tokens =====");
        AppLogger::Debug("===== ===== ===== ===== ===== =====");
        // rest as before: eval each lhs, skip negatives, check rhs length and feedback
        for (auto& lhs : lhsCandidates) {
            tryCandidate(lhs, eqPos, lhsLen, rhsLen, constraintsMap);
        }
    } // for eqPos

    return finalCandidates;
}
