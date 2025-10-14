/* ----- ----- ----- ----- */
// test_constraints.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/05
// Update Date: 2025/10/05
// Version: v1.0
/* ----- ----- ----- ----- */
/* ----- ----- ----- ----- */
// test_constraints.cpp
// Test for Constraint.cpp logging behaviors
// Author: DragonTaki
// Create Date: 2025/10/05
// Version: v1.0
/* ----- ----- ----- ----- */

#include <iostream>
#include <vector>
#include <string>
#include "../src/Constraint.h"
#include "../src/AppLogger.h"

// 測試輔助函數
void runTest(
    const std::string& name,
    const std::string& candidate,
    const std::string& expression,
    const std::string& color,
    const std::unordered_set<char>& allowedOps,
    bool expected
) {
    std::cout << "\n===== Test: " << name << " =====" << std::endl;
    bool result = matchesFeedback(candidate, expression, color, allowedOps);
    std::cout << "[Expected] " << (expected ? "PASS" : "FAIL")
              << " | [Actual] " << (result ? "PASS" : "FAIL") << std::endl;
    std::cout << "---------------------------------" << std::endl;
}

int main() {
    AppLogger::EnableTestMode(true);
    AppLogger::SetLogLevel(LogLevel::Debug);

    std::unordered_set<char> allowedOps = { '+', '-', '*', '/', '^' };

    // =====================
    // ✅ 正常案例
    // =====================
    runTest(
        "Valid all green",
        "1+2=3", "1+2=3", "ggggg", allowedOps, true
    );

    runTest(
        "Valid yellow case",
        "1+3=4", "1+2=3", "ggrgy", allowedOps, true
    );

    // =====================
    // ❌ 錯誤案例 (Log 測試)
    // =====================

    // 1. 長度不符
    runTest(
        "Mismatched length",
        "12+3", "12+34", "ggggg", allowedOps, false
    );

    // 2. 綠色位置錯誤
    runTest(
        "Green mismatch",
        "1+2=3", "1-2=3", "ggggg", allowedOps, false
    );

    // 3. 不合法運算子
    runTest(
        "Illegal operator",
        "1+2=3", "1?2=3", "grgrg", allowedOps, false
    );

    // 4. 黃色位置顏色不符 (應該不是綠色)
    runTest(
        "Yellow at wrong place",
        "1+2=3", "1+2=3", "gyggy", allowedOps, false
    );

    // 5. 紅色位置符號仍存在於 candidate 中
    runTest(
        "Illegal red presence",
        "1+2=3", "1+2=3", "grrrr", allowedOps, false
    );

    // 6. '=' 處理錯誤
    runTest(
        "Mismatched '=' position",
        "1+2=3", "1+23=", "gyggg", allowedOps, false
    );

    // 7. 無效顏色符號
    runTest(
        "Invalid color symbol",
        "1+2=3", "1+2=3", "gxbgy", allowedOps, false
    );

    // =====================
    // ✅ deriveConstraints 測試
    // =====================
    {
        std::cout << "\n===== deriveConstraints() test =====" << std::endl;
        std::vector<std::string> expressions = { "1+2=3", "1+3=4" };
        std::vector<std::string> colors = { "ggggg", "ggygr" };

        auto constraints = deriveConstraints(expressions, colors, 5);

        for (auto& kv : constraints) {
            const auto& symbol = kv.first;
            const auto& cst = kv.second;
            std::cout << "Symbol: " << symbol
                      << " | MinCount: " << cst.minCount
                      << " | MaxCount: " << cst.maxCount
                      << " | GreenPos: " << cst.greenPos.size()
                      << " | BannedPos: "
                      << std::count(cst.bannedPos.begin(), cst.bannedPos.end(), true)
                      << std::endl;
        }
    }

    std::cout << "\n===== All tests complete. =====" << std::endl;
    return 0;
}
