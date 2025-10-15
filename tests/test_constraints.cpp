/* ----- ----- ----- ----- */
// test_constraints.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/05
// Update Date: 2025/10/05
// Version: v1.0
/* ----- ----- ----- ----- */

#include <iostream>
#include <string>
#include <vector>

#include "core/AppLogger.h"
#include "logic/Constraint.h"

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
    LogFileManager::SetSilentMode(true);
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

        // 🧩 顯示測資
        std::cout << "\nTest Data:\n";
        for (size_t i = 0; i < expressions.size(); ++i)
            std::cout << "  " << expressions[i] << " -> " << colors[i] << std::endl;

        auto constraints = deriveConstraints(expressions, colors, 5);

        // 📊 顯示結果
        std::cout << "\nDerived Constraints:\n";
        for (auto& kv : constraints) {
            const auto& symbol = kv.first;
            const auto& cst = kv.second;
            std::cout << "  Symbol: " << symbol
                    << " | MinCount: " << cst.minCount()
                    << " | MaxCount: " << cst.maxCount()
                    << " | GreenPos: {";
            for (int pos : cst.greenPos()) std::cout << pos << " ";
            std::cout << "}"
                    << " | BannedPos: {";
            for (int pos : cst.bannedPos()) std::cout << pos << " ";
            std::cout << "}"
                    << " | Conflict: " << (cst.hasConflict() ? "YES" : "NO")
                    << std::endl;
        }
    }

    // =====================
    // ⚠️ deriveConstraints 衝突測試
    // =====================
    {
        std::cout << "\n===== deriveConstraints() conflict test =====" << std::endl;

        // 1️⃣ 數字 r/g/y 衝突（多次出現 1）
        {
            std::vector<std::string> expressions = { "11+23=34", "11+12=23", "11+34=45" };
            std::vector<std::string> colors      = { "gyrrrgrr", "gyrrrgrr", "grrrrgrr" };

            std::cout << "\nTest 1: Digit r/g/y conflict (1 repeated)\n";
            std::cout << "Test Data:\n";
            for (size_t i = 0; i < expressions.size(); ++i)
                std::cout << "  " << expressions[i] << " -> " << colors[i] << std::endl;

            auto constraints = deriveConstraints(expressions, colors, 8);

            std::cout << "\nDerived Constraints:\n";
            for (auto& kv : constraints) {
                const auto& symbol = kv.first;
                const auto& cst = kv.second;
                std::cout << "  Symbol: " << symbol
                        << " | MinCount: " << cst.minCount()
                        << " | MaxCount: " << cst.maxCount()
                        << " | GreenPos: " << cst.greenPos().size()
                        << " | BannedPos: " << cst.bannedPos().size()
                        << " | Conflict: " << (cst.hasConflict() ? "YES" : "NO")
                        << std::endl;
            }
        }

        // 2️⃣ 相鄰綠色符號衝突
        {
            std::vector<std::string> expressions = { "11+2=13", "2+11=13" };
            std::vector<std::string> colors      = { "rrgrgrr", "rgrrgrr" };

            std::cout << "\nTest 2: Adjacent green symbol conflict\n";
            std::cout << "Test Data:\n";
            for (size_t i = 0; i < expressions.size(); ++i)
                std::cout << "  " << expressions[i] << " -> " << colors[i] << std::endl;

            auto constraints = deriveConstraints(expressions, colors, 7);

            std::cout << "\nDerived Constraints:\n";
            for (auto& kv : constraints) {
                const auto& symbol = kv.first;
                const auto& cst = kv.second;
                std::cout << "  Symbol: " << symbol
                        << " | MinCount: " << cst.minCount()
                        << " | MaxCount: " << cst.maxCount()
                        << " | GreenPos: " << cst.greenPos().size()
                        << " | BannedPos: " << cst.bannedPos().size()
                        << " | Conflict: " << (cst.hasConflict() ? "YES" : "NO")
                        << std::endl;
            }
        }

        // 3️⃣ r/y 衝突案例（同一數字不同位置 r/y 不一致）
        {
            std::vector<std::string> expressions = { "12+35=47", "12+36=48" };
            std::vector<std::string> colors      = { "yyrrrygr", "yryrrygr" };

            std::cout << "\nTest 3: r/y conflict for same digit\n";
            std::cout << "Test Data:\n";
            for (size_t i = 0; i < expressions.size(); ++i)
                std::cout << "  " << expressions[i] << " -> " << colors[i] << std::endl;

            auto constraints = deriveConstraints(expressions, colors, 8);

            std::cout << "\nDerived Constraints:\n";
            for (auto& kv : constraints) {
                const auto& symbol = kv.first;
                const auto& cst = kv.second;
                std::cout << "  Symbol: " << symbol
                        << " | MinCount: " << cst.minCount()
                        << " | MaxCount: " << cst.maxCount()
                        << " | GreenPos: " << cst.greenPos().size()
                        << " | BannedPos: " << cst.bannedPos().size()
                        << " | Conflict: " << (cst.hasConflict() ? "YES" : "NO")
                        << std::endl;
            }
        }
    }


    std::cout << "\n===== All tests complete. =====" << std::endl;
    return 0;
}
