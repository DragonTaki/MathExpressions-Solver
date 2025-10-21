/* ----- ----- ----- ----- */
// main.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/09/28
// Update Date: 2025/10/19
// Version: v2.1
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

/**
 * @brief Main entry point for the Math Expression Solver program.
 *
 * <summary>
 * The program initializes the logging system, creates a RoundManager to handle the game logic,
 * and runs an interactive loop that repeatedly processes player inputs for each round.
 * Each round can include multiple expression inputs and evaluates candidates based on constraints.
 * The main loop handles exceptions gracefully, allows undo or end commands, and supports resetting the game.
 * </summary>
 *
 * @return int Exit status code (0 for normal termination, non-zero for error)
 */
int main() {
    // ------------------------------
    // Program Initialization
    // ------------------------------
    std::ios::sync_with_stdio(false);  ///< Disable C++ stdio synchronization for faster IO
    std::cin.tie(nullptr);             ///< Untie cin from cout to reduce flush overhead

    // ------------------------------
    // Logger Initialization
    // ------------------------------
    AppLogger::Initialize();            ///< Initialize the AppLogger system
    std::atexit([]() {                  ///< Ensure logger shutdown on program exit
        AppLogger::Shutdown();
    });
    AppLogger::EnableTestMode(true);                                 ///< Enable test mode logging (for debug/testing)
    AppLogger::SetLogLevel(LogLevel::Debug);                         ///< Set default log level to Debug
    AppLogger::Debug(std::format("__cplusplus = {}", __cplusplus));  ///< Log C++ version

    // ------------------------------
    // Round Manager Initialization
    // ------------------------------
    RoundManager roundManager;          ///< Handles per-round input, history, constraints, and candidate generation
    
    // ------------------------------
    // Main Interactive Loop
    // ------------------------------
    while (true) {
        try {
            // Process a full round: handles multiple inputs, constraints updates, and undo/end commands
            while (roundManager.processRoundInput()) {
                /**
                 * processRoundInput:
                 * - Determines input prompt based on current round history length
                 * - Accepts user input for expressions
                 * - Handles undo, end, and new round commands
                 * - Updates symbol constraints and filters candidate expressions
                 */
            }

            // Notify user that round is finished
            AppLogger::Prompt("Round finished. Start a new round.", LogColor::Yellow);

            // Reset game for next round
            // Users can optionally call resetRound() instead if partial reset is desired
            roundManager.resetGame();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            break;
        }
    }

    // ------------------------------
    // Program Termination
    // ------------------------------
    AppLogger::Shutdown();  ///< Ensure logger is properly shut down
    return 0;
}
