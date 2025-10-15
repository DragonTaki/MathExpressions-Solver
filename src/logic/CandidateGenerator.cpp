/* ----- ----- ----- ----- */
// CandidateGenerator.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/02
// Update Date: 2025/10/02
// Version: v1.0
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

bool CandidateGenerator::isRhsLengthFeasible(int lhsLen, int rhsLen, const std::unordered_set<char>& operators) const {
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
    std::unordered_map<char, Constraint>* lhsConstraintsMap,
    const std::vector<char>& requiredAtPos,
    int depth)
{
    // Log for each depth
    {
        std::string ops;
        for (char o : operators) ops.push_back(o), ops.push_back(' ');
        AppLogger::Debug(fmt::format("[generateLeftTokens, depth={}] lhsLen={}, operators=[{}]", depth, lhsLen, ops));
    }

    // 計算目前 token 已佔長度
    int pos = 0;
    for (const auto& t : current)
        pos += static_cast<int>(t.value.size());

    if (pos >= lhsLen) {
        lhsCandidates.push_back(current);
        return;
    }

    // Initialize remainingMin (=minCount), for charactor remaining available prune
    std::unordered_map<char,int> remainingMin;
    if (lhsConstraintsMap) {
        for (const auto& [ch, con] : *lhsConstraintsMap) {
            remainingMin[ch] = con.minCount();
            //AppLogger::Trace(fmt::format("[init] '{}' minCount={}", ch, con.minCount()));
        }
    }

    // --- Step 2: 檢查某個 token 是否可放在位置 pos ---
    auto isTokenAllowedAt = [&](const Expr::Token& token, int pos) -> bool {
        char ch = token.value[0]; // 第一個字符判斷，因為數字可能多位
        bool allowed = true;

        if (pos < (int)requiredAtPos.size() && requiredAtPos[pos] != 0) {
            if (requiredAtPos[pos] != ch) return false;
        }

        if (lhsConstraintsMap) {
            auto it = lhsConstraintsMap->find(ch);
            if (it != lhsConstraintsMap->end()) {
                const Constraint& con = it->second;
                int used = 0;
                for (const auto& t : current) {
                    if (t.value == token.value) used++;
                }

                if (con.maxCount() != 0 && used >= con.maxCount()) allowed = false;
                if (con.bannedPos().count(pos) > 0) allowed = false;
            }
        }

        return allowed;
    };

    // 嘗試每個可能字符
    for (char ch = '0'; ch <= '9'; ++ch) {
        auto it = lhsConstraintsMap ? lhsConstraintsMap->find(ch) : lhsConstraintsMap->end();

        if (it != lhsConstraintsMap->end()) {
            const Constraint& con = it->second;

            // forbidden
            if (con.minCount() == 0 && con.maxCount() == 0) {
                AppLogger::Trace(fmt::format("[Constraint] Skipped digit {}: forbidden (min=max=0)", ch));
                continue;
            }

            // banned positions
            if (con.bannedPos().count(pos)) {
                AppLogger::Trace(fmt::format("[Constraint] Skipped digit {}: banned positions", ch));
                continue;
            }

            // optional: 檢查 maxCount 是否已達限制
            if (con.usedCount() >= con.maxCount()) {
                AppLogger::Trace(fmt::format("[Constraint] Skipped digit {}: Usage reach max limit {}", ch, con.maxCount()));
                continue;
            } // 已達上限
        }

        AppLogger::Trace(fmt::format("[Constraint] Trying digit {}", ch));
        Expr::Token token{Expr::TokenType::Digit, std::string(1, ch)};
        current.push_back(token);

        // 更新使用次數
        if (it != lhsConstraintsMap->end()) it->second.usedCount()++;

        _dfsGenerateLeftTokens(lhsLen, operators, current, lhsCandidates,
                               lhsConstraintsMap, requiredAtPos, depth + 1);

        // 回溯
        current.pop_back();
        if (it != lhsConstraintsMap->end()) it->second.usedCount()--;
    }

    for (char op : operators) {
        Expr::Token token{Expr::TokenType::Operator, std::string(1, op)};
        if (!isTokenAllowedAt(token, pos)) continue;

        // 運算子檢查
        if (!current.empty()) {
            Expr::Token last = current.back();
            if (last.type == Expr::TokenType::Operator) continue; // 不允許連續運算子
            if (last.value == "^" && token.value == "^") continue; // 不允許連續 ^
        } else {
            if (op != '+' && op != '-') continue; // 開頭只允許 +/-
        }

        AppLogger::Trace(fmt::format("[Constraint] Trying operator {}", op));
        current.push_back(token);
        _dfsGenerateLeftTokens(lhsLen, operators, current, lhsCandidates,
                               lhsConstraintsMap, requiredAtPos, depth + 1);
        current.pop_back();
    }
}

void CandidateGenerator::generateLeftTokens(
    int lhsLen,
    const std::unordered_set<char>& operators,
    std::vector<Expr::Token> current,
    std::vector<std::vector<Expr::Token>>& lhsCandidates,
    std::unordered_map<char, Constraint>* lhsConstraintsMap,
    int depth)
{
    // --- Step 0: 建立 requiredAtPos (僅建立一次) ---
    std::vector<char> requiredAtPos(lhsLen, 0);
    if (lhsConstraintsMap) {
        for (const auto& [ch, con] : *lhsConstraintsMap) {
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
    }

    // --- Step 1: 開始 DFS ---
    _dfsGenerateLeftTokens(lhsLen, operators, current, lhsCandidates,
                           lhsConstraintsMap, requiredAtPos, depth);
}

std::vector<string> CandidateGenerator::generate(
    int length,
    const std::unordered_set<char>& operators,
    const std::vector<string>& expressions,
    const std::vector<string>& colors
) {
    std::vector<string> finalCandidates;  ///< Record possible answer(s)

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

        // 調整 minCount，考慮 RHS 可用空間
        for (auto& [ch, con] : lhsConstraintsMap) {
            int rhsAvailable = rhsLen; // 假設每個符號最多可用 RHS 空間補足
            con.minCount() = (std::max)(0, con.minCount() - rhsAvailable);
            // maxCount 可保留原始值
        }

        AppLogger::Debug("===== ===== ===== ===== ===== =====");
        AppLogger::Debug("===== Start to generate left tokens =====");
        AppLogger::Debug("===== ===== ===== ===== ===== =====");
        // call generator with forbidden and minReq and counts
        generateLeftTokens(lhsLen, operators, tempLhsTokenVector, lhsCandidates, &lhsConstraintsMap, 0);

        AppLogger::Debug("===== ===== ===== ===== ===== =====");
        AppLogger::Debug("===== Start to eval left tokens =====");
        AppLogger::Debug("===== ===== ===== ===== ===== =====");
        // rest as before: eval each lhs, skip negatives, check rhs length and feedback
        for (auto& lhs : lhsCandidates) {
            AppLogger::Debug(fmt::format("[Try] LHS='{}' (eqPos={}, lhsLen={})", tokenVecToString(lhs), eqPos, lhsLen));

            long long lhsResult;
            try {
                lhsResult = validator.evalExpr(tokenVecToString(lhs));
            } catch (const std::exception& e) {
                AppLogger::Debug(fmt::format("[Eval Fail] {} : {}", tokenVecToString(lhs), e.what()));
                continue;
            } catch (...) {
                AppLogger::Debug(fmt::format("[Eval Fail] {} : unknown error", tokenVecToString(lhs)));
                continue;
            }

            // 1) 立刻排除負數（遊戲規則：RHS 不會是負數）
            if (lhsResult < 0) {
                AppLogger::Debug(fmt::format("[Reject Negative] {} => {}", tokenVecToString(lhs), lhsResult));
                continue;
            }

            std::string rhs = to_string(lhsResult);
            if ((int)rhs.size() != rhsLen) {
                AppLogger::Debug(fmt::format("[Reject] {}={} -> rhs length {} != {}", tokenVecToString(lhs), rhs, (int)rhs.size(), rhsLen));
                continue;
            }

            std::string candidate = tokenVecToString(lhs) + '=' + rhs;
            finalCandidates.push_back(candidate);
        } // for lhsCandidates
    } // for eqPos

    return finalCandidates;
}
