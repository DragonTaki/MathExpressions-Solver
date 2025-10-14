/* ----- ----- ----- ----- */
// AppLogger.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/01
// Update Date: 2025/10/05
// Version: v2.0
/* ----- ----- ----- ----- */

#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <format>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

#include "LogFileManager.h"

#ifdef _WIN32
#include <windows.h>
#endif

// ----- Enum: LogColor ----- //
enum class LogColor {
    White,
    Gray,
    Red,
    Yellow,
    Green,
    Cyan,
    Blue,
    Magenta,
    Default
};

// ----- Enum: LogLevel ----- //
enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warn,
    Error
};

/**
 * @brief Encapsulates a single log message with metadata.
 */
struct LogRecord {
    std::string message;
    LogLevel level;
    std::string timestamp;

    LogRecord(const std::string& msg, LogLevel lvl);
};

/**
 * @brief Thread-safe logger with colorized output and test-mode control.
 */
class AppLogger {
public:
    // --- Configuration ---
    static void SetLogLevel(LogLevel level);
    static void EnableTestMode(bool enabled);

    // --- Logging ---
    static void Log(const std::string& message, LogLevel level);
    static void Trace(const std::string& msg);
    static void Debug(const std::string& msg);
    static void Info(const std::string& msg);
    static void Warn(const std::string& msg);
    static void Error(const std::string& msg);

    static void Initialize();
    static void Initialize(const std::string& logFilename);
    static void Shutdown();

private:
    // --- Internal Helpers ---
    static std::string ColorToAnsi(LogColor c);
#ifdef _WIN32
    static WORD ColorToWindowsAttr(LogColor c);
#endif
    static LogColor GetLevelColor(LogLevel level);
    static std::string GetLevelLabel(LogLevel level);

    static void Print(const LogRecord& record);

private:
    static inline std::mutex mutex_;
    static inline LogLevel minLevel_ = LogLevel::Info;
    static inline bool testMode_ = false;

    // --- Data Members ---
    static std::mutex logMutex;                          ///< Thread safety for logging

    /// Default folder for log files.
    static inline const std::filesystem::path DefaultFolder = "log";

    /// Default filename template.
    static inline const std::string DefaultTemplate =
        "runtime_log_{year}-{month}-{day}_{hour}.{minute}.{second}.log";

    /// Shared helper used by both Initialize() overloads.
    static void InitializeFileManager(const std::filesystem::path& filePath);
};
