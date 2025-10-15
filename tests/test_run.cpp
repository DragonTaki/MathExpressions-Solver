/* ----- ----- ----- ----- */
// test_run.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/15
// Update Date: 2025/10/15
// Version: v1.0
/* ----- ----- ----- ----- */

#include <fstream>
#include <iostream>
#include <string>

#include "core/AppLogger.h"
#include "logic/CandidateGenerator.h"
#include "logic/Constraint.h"
#include "logic/ExpressionValidator.h"

#define FMT_HEADER_ONLY
#include "core.h"
#include "ranges.h"

using namespace std;

/**
 * @brief Runs a single test case automatically.
 *
 * This test runner loads predefined inputs and executes the full solver pipeline.
 * It also redirects logs to `tests/test_<expr>.log`.
 */
int main() {
    // === 1. 測資設定 ===
    const string operators = "9+-*";
    const string equation  = "198+7=205";
    const string feedback  = "yyyrrgrry";

    // === 2. 設定 log 輸出 ===
    AppLogger::Initialize(); // 使用預設路徑，如 logs/app.log
    AppLogger::EnableTestMode(true);
    AppLogger::SetLogLevel(LogLevel::Trace);
    AppLogger::Info("=== Running unit test ===");

    try {
        // === 3. 準備運算驗證器 ===
        unordered_set<char> validOpsSet(operators.begin(), operators.end());
        ExpressionValidator validator;
        validator.setValidOps(validOpsSet);

        // === 4. 建立 expressions / colors 容器 ===
        vector<string> expressions{equation};
        vector<string> colors{feedback};
        int length = static_cast<int>(equation.size());

        // === 5. 推導 constraints ===
        unordered_map<char, Constraint> constraintsMap = deriveConstraints(expressions, colors, length);

        // === 6. 生成候選表達式 ===
        CandidateGenerator generator(validator);
        vector<string> results = generator.generate(length, validOpsSet, expressions, colors);

        AppLogger::Info(fmt::format("[Result] Total candidates = {}", results.size()));

        for (const auto& r : results) {
            cout << r << "\n";
            AppLogger::Debug(fmt::format("Candidate: {}", r));
        }

        AppLogger::Info("=== Test Completed Successfully ===");
    }
    catch (const std::exception& ex) {
        AppLogger::Error(fmt::format("Exception: {}", ex.what()));
        cerr << "[Test Failed] " << ex.what() << endl;
        return 1;
    }

    cout << "[Test Completed]" << endl;
    return 0;
}
