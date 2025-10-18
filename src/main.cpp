/* ----- ----- ----- ----- */
// main.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/09/28
// Update Date: 2025/10/16
// Version: v2.0
/* ----- ----- ----- ----- */

#include <algorithm>
#include <format>
#include <iostream>
#include <unordered_set>

#include "core/constants/ExpressionConstants.h"
#include "core/logging/AppLogger.h"
#include "logic/CandidateGenerator.h"
#include "logic/ExpressionValidator.h"
#include "logic/RoundManager.h"
#include "util/ConsoleUtils.h"
#include "util/Utils.h"

#define FMT_HEADER_ONLY
#include "core.h"
#include "ranges.h"

int main() {
    // Initialize program
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // Initialize AppLogger
    AppLogger::Initialize();
    std::atexit([]() {
        AppLogger::Shutdown();
    });
    AppLogger::EnableTestMode(true);
    AppLogger::SetLogLevel(LogLevel::Debug);
    AppLogger::Log(std::format("__cplusplus = {}", __cplusplus), LogLevel::Debug);

    RoundManager roundManager;
    
    while (true) {
        try {
            if (!roundManager.startRound()) break; // 初始化並讀第一輪

            // Main process logic
            while (roundManager.nextRoundInput()) {
                // Will continue to let the player to enter new guesses and colors, until "end" is entered
            }

            // Game ended, start new game
            std::cout << "\nRound finished. Start a new round.\n\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            break;
        }
    }

    // Program ending procedure
    AppLogger::Shutdown();
    return 0;
}
