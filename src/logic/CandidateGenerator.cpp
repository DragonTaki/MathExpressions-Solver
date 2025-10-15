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

struct GuessConflictInfo
{
    std::vector<std::unordered_set<char>> greenCandidates;
    bool eqPosConsistent = true;
    int fixedEqPos = -1;
    bool hasGreenConflict = false;
};

CandidateGenerator::CandidateGenerator(ExpressionValidator& validator)
    : validator(validator) {}

bool CandidateGenerator::matchesFeedback(const string& candidate,
                                         const string& expression,
                                         const string& color) {
    unordered_map<char,int> cCount;
    for (char ch : candidate) cCount[ch]++;

    // 綠色
    for (size_t i = 0; i < candidate.size(); ++i) {
        if (color[i] == 'g') {
            if (candidate[i] != expression[i]) return false;
            cCount[expression[i]]--;
        }
    }

    // 黃色
    for (size_t i = 0; i < candidate.size(); ++i) {
        if (color[i] == 'y') {
            if (candidate[i] == expression[i]) return false;
            if (cCount[expression[i]] <= 0) return false;
            cCount[expression[i]]--;
        }
    }

    // 紅色
    for (size_t i = 0; i < candidate.size(); ++i) {
        if (color[i] == 'r') {
            if (cCount.count(expression[i]) && cCount[expression[i]] > 0) return false;
        }
    }

    return true;
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

/* ----- 新增輔助函數：分析所有 guess 衝突 ----- */
static GuessConflictInfo analyzeGuessConflicts(const std::vector<std::string>& guesses,
                                               const std::vector<std::string>& feedbacks)
{
    GuessConflictInfo info;

    if (guesses.empty())
        return info;

    size_t maxLen = guesses[0].size();
    info.greenCandidates.resize(maxLen);

    std::unordered_set<int> eqPositions; // '=' green positions

    for (size_t i = 0; i < guesses.size(); ++i)
    {
        const std::string& g = guesses[i];
        const std::string& fb = feedbacks[i];

        for (size_t j = 0; j < g.size(); ++j)
        {
            if (fb[j] == 'g')
            {
                info.greenCandidates[j].insert(g[j]);
                if (g[j] == '=')
                    eqPositions.insert(static_cast<int>(j));
            }
        }
    }

    // 檢查 green 衝突
    for (const auto& set : info.greenCandidates)
    {
        if (set.size() > 1)
        {
            info.hasGreenConflict = true;
            break;
        }
    }

    // 檢查 '=' 位置一致性
    if (eqPositions.size() == 1)
    {
        info.fixedEqPos = *eqPositions.begin();
    }
    else if (eqPositions.size() > 1)
    {
        info.eqPosConsistent = false;
    }

    return info;
}

void CandidateGenerator::generateLeftTokens(
    int lhsLen,
    const std::unordered_set<char>& operators,
    std::string& current,
    std::vector<std::string>& lhsCandidates,
    const std::vector<std::unordered_set<char>>* allowed,
    const std::unordered_map<char, Constraint>* lhsConstraintsMap,
    int depth
) {
    // --- Log entry ---
    {
        std::string ops;
        for (char o : operators) ops.push_back(o), ops.push_back(' ');
        AppLogger::Debug(fmt::format("[generateLeftTokens] lhsLen={}, operators=[{}], depth={}", lhsLen, ops, depth));
        if (allowed)
            AppLogger::Debug(fmt::format("[generateLeftTokens] allowed vector size = {}", (int)allowed->size()));
        if (lhsConstraintsMap)
            AppLogger::Debug(fmt::format("[generateLeftTokens] constraintsMap size = {}", (int)lhsConstraintsMap->size()));
    }

    // ----------------------------
    // Step 1: 初始化 minCount
    // ----------------------------
    std::unordered_map<char,int> remainingMin;
    if (lhsConstraintsMap) {
        for (const auto& [ch, con] : *lhsConstraintsMap) {
            remainingMin[ch] = con.minCount();
            AppLogger::Log(fmt::format("[init] '{}' minCount={}", ch, con.minCount()), LogLevel::Trace);
        }
    }

    // ----------------------------
    // Step 3: 檢查運算子合法性 (語義剪枝)
    // ----------------------------
    auto checkOperatorValidity = [&](const std::string& expr, char nextOp) -> bool {
        if (expr.empty()) return (nextOp == '+' || nextOp == '-'); // 開頭只能 + 或 -
        char last = expr.back();

        // 不允許開頭為 *, /, ^
        if (expr.size() == 1 && (nextOp == '*' || nextOp == '/' || nextOp == '^'))
            return false;

        // 不允許連續運算子
        if (operators.count(last)) return false;

        // 不允許連續 ^（例如 2^2^2）
        if (last == '^' && nextOp == '^') return false;

        // 嘗試評估當前表達式結果
        auto valOpt = ExpressionValidator::safeEval(expr);
        if (!valOpt.has_value()) return false;
        double val = valOpt.value();

        // 除法時檢查可除性（簡易剪枝）
        if (nextOp == '/') {
            if (std::fabs(val) < 1e-9) return false; // 前面結果為0
        }

        // 運算結果不能為負
        if (val < 0) return false;
        
        return true;
    };

    // --- 新增：建立 requiredAtPos (將每個 greenPos 映射到對應的字元) ---
    std::vector<char> requiredAtPos(lhsLen, 0);
    if (lhsConstraintsMap) {
        for (const auto& [ch, con] : *lhsConstraintsMap) {
            for (int gp : con.greenPos()) {
                if (gp >= 0 && gp < lhsLen) {
                    if (requiredAtPos[gp] != 0 && requiredAtPos[gp] != ch) {
                        AppLogger::Log(fmt::format("[Constraint] conflict at pos {}: '{}' vs '{}'", gp, requiredAtPos[gp], ch), LogLevel::Warn);
                    }
                    requiredAtPos[gp] = ch;
                }
            }
        }
    }

    // ----------------------------
    // Step 2: isCharAllowedAt() with debug
    // ----------------------------
    auto isCharAllowedAt = [&](char ch, int pos) -> bool {
        bool allowedPos = true;
        bool passedMax = true;
        bool passedBan = true;

        if (pos >= 0 && pos < (int)requiredAtPos.size() && requiredAtPos[pos] != 0) {
            // 若該位置已被某符號綠格鎖定，只有該符號被允許
            if (requiredAtPos[pos] != ch) {
                AppLogger::Log(fmt::format("[isCharAllowedAt] pos {} requires '{}' but checking '{}'", pos, requiredAtPos[pos], ch), LogLevel::Trace);
                return false;
            }
        }

        if (allowed && pos < (int)allowed->size()) {
            const auto& allowedSet = (*allowed)[pos];
            if (!allowedSet.empty() && allowedSet.count(ch) == 0) {
                allowedPos = false;
            }
        }

        if (lhsConstraintsMap) {
            auto it = lhsConstraintsMap->find(ch);
            if (it != lhsConstraintsMap->end()) {
                const Constraint& con = it->second;
                int used = std::count(current.begin(), current.end(), ch);
                if (con.maxCount() == 0 || used >= con.maxCount())
                    passedMax = false;

                if (pos < (int)con.bannedPos().size() && con.bannedPos()[pos])
                    passedBan = false;
            }
        }

        bool ok = allowedPos && passedMax && passedBan;
        if (!ok) {
            AppLogger::Log(fmt::format("[isCharAllowedAt] ch='{}' pos={} => allowedPos={} max={} ban={}",
                ch, pos, allowedPos, passedMax, passedBan), LogLevel::Trace);
        }
        return ok;
    };

    // ----------------------------
    // Step 3: DFS 生成
    // ----------------------------
    std::function<void(int,bool,bool)> genTokens;
    genTokens = [&](int pos, bool expectNumber, bool hasOperator) {
        int remaining = lhsLen - pos;
        if (remaining <= 0) {
            if (!expectNumber && hasOperator)  // ✅ 必須包含至少一個運算子
                lhsCandidates.push_back(current);
            return;
        }

        // 剪枝：剩餘長度不足以滿足 minCount
        for (auto& [ch, minRem] : remainingMin) {
            if (minRem > remaining) {
                AppLogger::Log(fmt::format("[genTokens] prune@{} remain={} -> minRem['{}']={}", pos, remaining, ch, minRem), LogLevel::Trace);
                return;
            }
        }

        // --- DEBUG: show current state ---
        AppLogger::Log(fmt::format("[genTokens] pos={}, remaining={}, current='{}'", pos, remaining, current), LogLevel::Trace);

        // base case
        if (remaining == 0) {
            AppLogger::Log(fmt::format("[genTokens] reached end => push '{}'", current), LogLevel::Trace);
            lhsCandidates.push_back(current);
            return;
        }

        // 嘗試產生數字區塊
        for (int numLen = 1; numLen <= remaining; ++numLen) {
            int after = remaining - numLen;
            if (after == 1) continue;

            AppLogger::Log(fmt::format("[genTokens] try number length={} at pos={}", numLen, pos), LogLevel::Trace);

            std::function<void(int)> buildNumber;
            buildNumber = [&](int idx) {
                if (idx == numLen) {
                    if (after == 0) {
                        if (hasOperator) lhsCandidates.push_back(current);
                        AppLogger::Log(fmt::format("[buildNumber] complete '{}' => candidate added", current), LogLevel::Trace);
                        return;
                    }
                    for (char op : operators) {
                        int opPos = pos + numLen;
                        if (!isCharAllowedAt(op, opPos)) {
                            AppLogger::Log(fmt::format("[buildNumber] skip op='{}' at pos={}", op, opPos), LogLevel::Trace);
                            continue;
                        }
                        // === 新增語義剪枝 ===
                        if (!checkOperatorValidity(current, op)) {
                            AppLogger::Log(fmt::format("[prune] op='{}' invalid after '{}'", op, current), LogLevel::Trace);
                            continue;
                        }
                        // operator 處理（在 idx==numLen 時的迴圈）
                        if (requiredAtPos.size() > (size_t)opPos && requiredAtPos[opPos] != 0) {
                            // 這個位置被綠格鎖定，必須符合該字元
                            if (requiredAtPos[opPos] != op) {
                                AppLogger::Log(fmt::format("[buildNumber] op '{}' not matching required '{}' at pos={}", op, requiredAtPos[opPos], opPos), LogLevel::Trace);
                                continue;
                            } else {
                                // 必要的綠格符號，直接遞迴（等同於你原先的 isGreen 路徑）
                                current.push_back(op);
                                genTokens(opPos + 1, true, true);
                                current.pop_back();
                                continue; // 該位置僅允許這個 op
                            }
                        }
                        // 否則按原流程嘗試該 op
                        current.push_back(op);
                        genTokens(opPos + 1, true, true);
                        current.pop_back();
                    }
                    return;
                }

                int absolutePos = pos + idx;
                std::vector<char> candidates;
                for (char d = '0'; d <= '9'; ++d) {
                    if (idx == 0 && numLen > 1 && d == '0') continue;
                    if (!isCharAllowedAt(d, absolutePos)) continue;
                    candidates.push_back(d);
                }

                // 如果該位置已被綠格鎖定，僅允許那字元
                if (absolutePos >= 0 && absolutePos < (int)requiredAtPos.size() && requiredAtPos[absolutePos] != 0) {
                    char required = requiredAtPos[absolutePos];
                    if (std::find(candidates.begin(), candidates.end(), required) != candidates.end()) {
                        candidates.clear();
                        candidates.push_back(required);
                        AppLogger::Log(fmt::format("[greenPriority] only allow '{}' at pos={}", required, absolutePos), LogLevel::Trace);
                    } else {
                        // required char 不在 candidates（通常因為 max/banned/allowed 排除了），就沒候選了
                        candidates.clear();
                    }
                }

                if (pos + numLen == lhsLen && !hasOperator) {
                    // 到達結尾但尚未放過符號 → 無效
                    AppLogger::Log(fmt::format("[prune] '{}' has no operator", current), LogLevel::Trace);
                    return;
                }

                if (candidates.empty()) {
                    AppLogger::Log(fmt::format("[buildNumber] no candidates at pos={} idx={} numLen={}", absolutePos, idx, numLen), LogLevel::Trace);
                    return;
                }

                for (char ch : candidates) {
                    AppLogger::Log(fmt::format("[buildNumber] choose '{}' at pos={}", ch, absolutePos), LogLevel::Trace);
                    current.push_back(ch);
                    if (remainingMin.count(ch) > 0 ) remainingMin[ch]--;

                    // === 動態檢查 /0, 結果為負, 可除性 ===
                    auto valOpt = ExpressionValidator::safeEval(current);
                    if (valOpt.has_value()) {
                        double val = valOpt.value();
                        if (val < 0) { // 不允許負結果
                            current.pop_back();
                            if (remainingMin.count(ch)) remainingMin[ch]++;
                            continue;
                        }
                    }

                    buildNumber(idx + 1);  // ←← 關鍵：遞迴呼叫自己產生多位數
                    if (remainingMin.count(ch) > 0) remainingMin[ch]++;
                    current.pop_back();
                }
            };

            buildNumber(0);
        }
    };

    // ----------------------------
    // Step 4: Start recursion
    // ----------------------------
    AppLogger::Debug("[generateLeftTokens] === START DFS ===");
    genTokens(0, true, false);
    AppLogger::Debug(fmt::format("[generateLeftTokens] Finished: generated {} lhsCandidates", lhsCandidates.size()));
}

std::vector<string> CandidateGenerator::generate(
    int length,
    const std::unordered_set<char>& operators,
    const std::vector<string>& expressions,
    const std::vector<string>& colors
) {
    std::vector<string> finalCandidates;

    // 1) build allowedPerPos (greens) as you had
    std::vector<std::unordered_set<char>> allowedPerPos(length);
    bool useAllowed = false;
    for (size_t i = 0; i < expressions.size(); ++i) {
        const string& expression = expressions[i];
        const string& color = colors[i];
        for (int p = 0; p < length; ++p) {
            if (color[p] == 'g') {
                allowedPerPos[p].insert(expression[p]);
                useAllowed = true;
            }
        }
    }

    // 2) build minRequired and forbidden (global)
    std::unordered_map<char, Constraint> constraintsMap = deriveConstraints(expressions, colors, length);

    std::unordered_set<char> forbidden;
    for (auto& [ch, con] : constraintsMap) {
        if (con.minCount() == 0 && con.maxCount() == 0) {
            forbidden.insert(ch);
            AppLogger::Debug(fmt::format("[Constraint] '{}' is forbidden (min=max=0)", ch));
        }
    }

    // ===== 新增分析階段 =====
    GuessConflictInfo conflict = analyzeGuessConflicts(expressions, colors);

    if (conflict.hasGreenConflict)
    {
        AppLogger::Warn("Conflicting green positions detected.");
        AppLogger::Warn("Those positions will not be locked.");
    }

    if (!conflict.eqPosConsistent)
    {
        AppLogger::Warn("'=' green position inconsistent among guesses.");
        AppLogger::Warn("Equation position will be auto-detected.");
    }

    //尋找green '='
    int fixedEqPos = conflict.fixedEqPos;
    for (size_t i = 0; i < expressions.size(); ++i) {
        const std::string& expr = expressions[i];
        const std::string& color = colors[i];
        for (size_t j = 0; j < expr.size(); ++j) {
            if (expr[j] == '=' && color[j] == 'g') {
                fixedEqPos = static_cast<int>(j);
                break;
            }
        }
        if (fixedEqPos != -1) break; // 找到第一個 green '=' 就可以了
    }

    std::vector<int> eqPositions;
    if (fixedEqPos != -1) {
        AppLogger::Debug(fmt::format("[eqPos] eqPos has fixed position at {}", fixedEqPos));
        eqPositions.push_back(fixedEqPos); // 直接固定
    } else {
        AppLogger::Debug(fmt::format("[eqPos] eqPos does not has fixed position, will traverse it"));
        for (int eqPos = length - 2; eqPos >= 3; --eqPos) {
            eqPositions.push_back(eqPos);
        }
    }

    for (int eqPos : eqPositions) {
        int lhsLen = eqPos;
        int rhsLen = length - eqPos - 1;

        // skip impossible rhs by length feasibility (call your isRhsLengthFeasible)
        if (!isRhsLengthFeasible(lhsLen, rhsLen, operators)) {
            AppLogger::Debug(fmt::format("[Skip eqPos={}] unrealistic rhsLen {}", eqPos, rhsLen));
            continue;
        }

        std::vector<string> lhsCandidates;
        std::string temp;
        vector<std::unordered_set<char>> lhsAllowed(lhsLen);
        if (useAllowed) {
            for (int i = 0; i < lhsLen; ++i) lhsAllowed[i] = allowedPerPos[i];
        }

        std::unordered_map<char, Constraint> lhsConstraintsMap = constraintsMap;

        // 調整 minCount，考慮 RHS 可用空間
        for (auto& [ch, con] : lhsConstraintsMap) {
            int rhsAvailable = rhsLen; // 假設每個符號最多可用 RHS 空間補足
            con.minCount() = (std::max)(0, con.minCount() - rhsAvailable);
            // maxCount 可保留原始值
        }

        // call generator with forbidden and minReq and counts
        generateLeftTokens(lhsLen, operators, temp, lhsCandidates,
                           useAllowed ? &lhsAllowed : nullptr,
                           &lhsConstraintsMap, 0);

        // rest as before: eval each lhs, skip negatives, check rhs length and feedback
        for (auto& lhs : lhsCandidates) {
            AppLogger::Debug(fmt::format("[Try] LHS='{}' (eqPos={}, lhsLen={})", lhs, eqPos, lhsLen));

            long long lhsResult;
            try {
                lhsResult = validator.evalExpr(lhs);
            } catch (const std::exception& e) {
                AppLogger::Debug(fmt::format("[Eval Fail] {} : {}", lhs, e.what()));
                continue;
            } catch (...) {
                AppLogger::Debug(fmt::format("[Eval Fail] {} : unknown error", lhs));
                continue;
            }

            // 1) 立刻排除負數（遊戲規則：RHS 不會是負數）
            if (lhsResult < 0) {
                AppLogger::Debug(fmt::format("[Reject Negative] {} => {}", lhs, lhsResult));
                continue;
            }

            std::string rhs = to_string(lhsResult);
            if ((int)rhs.size() != rhsLen) {
                AppLogger::Debug(fmt::format("[Reject] {}={} -> rhs length {} != {}", lhs, rhs, (int)rhs.size(), rhsLen));
                continue;
            }

            std::string candidate = lhs + '=' + rhs;
            bool ok = true;
            for (std::size_t i = 0; i < expressions.size(); ++i) {
                if (!matchesFeedback(candidate, expressions[i], colors[i])) {
                    AppLogger::Debug(fmt::format("[Reject] {} mismatch vs guess '{}' (fb='{}')", candidate, expressions[i], colors[i]));
                    ok = false;
                    break;
                }
            }
            if (ok) {
                AppLogger::Info(fmt::format("[Accept] {}", candidate));
                finalCandidates.push_back(candidate);
            }
        } // for lhsCandidates
    } // for eqPos

    return finalCandidates;
}
