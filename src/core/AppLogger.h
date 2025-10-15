/* ----- ----- ----- ----- */
// AppLogger.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/01
// Update Date: 2025/10/05
// Version: v2.0
/* ----- ----- ----- ----- */

#pragma once
#include <chrono>
#include <ctime>
#include <filesystem>
#include <format>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

#include "LogFileManager.h"

#ifdef _WIN32
#include <windows.h>
#endif

/**
 * @enum LogColor
 * @brief Represents the available colors for log output.
 */
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

/**
 * @enum LogLevel
 * @brief Defines severity levels for log messages.
 */
enum class LogLevel {
    Trace,  ///< Fine-grained debug messages.
    Debug,  ///< Standard debug messages.
    Info,   ///< General informational messages.
    Warn,   ///< Warning messages.
    Error   ///< Error messages.
};

/**
 * @struct LogRecord
 * @brief Represents a single log entry with message, level, and timestamp.
 */
struct LogRecord {
    std::string message;   ///< The content of the log message.
    LogLevel level;        ///< Log severity level.
    std::string timestamp; ///< Timestamp in HH:MM:SS format.

    /**
     * @brief Constructs a LogRecord.
     * @param msg The log message content.
     * @param lvl The severity level of the log.
     */
    LogRecord(const std::string& msg, LogLevel lvl);
};

/**
 * @class AppLogger
 * @brief Thread-safe logger with colorized console output, file logging, and test-mode control.
 *
 * <summary>
 * AppLogger provides a singleton-style interface for logging messages to the console and
 * to a file. It supports multiple log levels, ANSI/Windows console colors, and thread safety.
 * It can be initialized with a default folder/filename or a user-specified path.
 * </summary>
 */
class AppLogger {
public:
    // ----- Configuration -----
    /**
     * @brief Set the minimum log level to be output.
     * Messages below this level will be ignored.
     * @param level Minimum severity level.
     */
    static void SetLogLevel(LogLevel level);
    /**
     * @brief Enable or disable test mode.
     * Test mode may alter log behavior for unit testing.
     * @param enabled True to enable test mode, false to disable.
     */
    static void EnableTestMode(bool enabled);

    // ----- Logging Methods -----
    /**
     * @brief General logging entry point.
     * @param message Log message content.
     * @param level Severity level.
     */
    static void Log(const std::string& message, LogLevel level);

    /// Convenience shortcuts for each log level.
    static void Trace(const std::string& msg);
    static void Debug(const std::string& msg);
    static void Info(const std::string& msg);
    static void Warn(const std::string& msg);
    static void Error(const std::string& msg);

    // ----- Initialization / Shutdown -----
    /**
     * @brief Initialize the logger with default folder and filename.
     */
    static void Initialize();

    /**
     * @brief Initialize the logger with a custom folder or file.
     * @param logFilename Path to a folder or a specific filename. If empty, defaults are used.
     */
    static void Initialize(const std::string& logFilename);

    /**
     * @brief Shut down the logger and flush any remaining logs.
     */
    static void Shutdown();

private:
    // ----- Internal Helpers -----
    /**
     * @brief Convert a LogColor enum to ANSI escape code string (for UNIX/terminal).
     * @param c LogColor value.
     * @return ANSI escape code string for colorized output.
     */
    static std::string ColorToAnsi(LogColor c);

#ifdef _WIN32
    /**
     * @brief Convert a LogColor enum to Windows console attribute.
     * @param c LogColor value.
     * @return WORD representing Windows console color attribute.
     */
    static WORD ColorToWindowsAttr(LogColor c);
#endif

    /**
     * @brief Get the color corresponding to a log level.
     * @param level LogLevel value.
     * @return LogColor for console output.
     */
    static LogColor GetLevelColor(LogLevel level);

    /**
     * @brief Get a human-readable label for a log level.
     * @param level LogLevel value.
     * @return Formatted string label (e.g., "[INFO]").
     */
    static std::string GetLevelLabel(LogLevel level);

    /**
     * @brief Print a formatted LogRecord to console and file.
     * @param record LogRecord to output.
     */
    static void Print(const LogRecord& record);

private:
    // ----- Internal State -----
    static inline std::mutex mutex_;                     ///< Mutex for thread-safe logging
    static inline LogLevel minLevel_ = LogLevel::Info;   ///< Minimum log level
    static inline bool testMode_ = false;                ///< Test mode flag

    static std::mutex logMutex;                          ///< Mutex for initialization

    static inline const std::filesystem::path DefaultFolder = "log";     ///< Default folder
    static inline const std::string DefaultTemplate =
        "runtime_log_{year}-{month}-{day}_{hour}.{minute}.{second}.log"; ///< Default filename template

    /**
     * @brief Internal helper to initialize LogFileManager with a specific file path.
     * @param filePath Full path to log file.
     */
    static void InitializeFileManager(const std::filesystem::path& filePath);
};
