/* ----- ----- ----- ----- */
// AppLogger.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/01
// Update Date: 2025/10/17
// Version: v2.1
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

#include "LogColor.h"
#include "LogFileManager.h"
#include "LogLevel.h"

#ifdef _WIN32
#include <windows.h>
#endif

/**
 * @struct LogRecord
 * @brief Represents a single log entry containing message, level, and timestamp.
 *
 * <summary>
 * Each LogRecord stores the log message content, its severity level,
 * and a timestamp formatted as "YYYY-MM-DD HH:MM:SS".
 * It is created automatically when a log message is issued.
 * </summary>
 */
struct LogRecord {
    std::string message;            ///< The content of the log message
    LogLevel level;                 ///< Log severity level
    std::optional<LogColor> color;  ///< Color for console print
    std::string fullTimestamp;      ///< Timestamp in YYYY-MM-DD HH:MM:SS format

    /**
     * @brief Constructs a LogRecord with message and level.
     *
     * <summary>
     * Initializes the log record with the given message and log level.
     * Captures the current system time for timestamping.
     * </summary>
     *
     * @param message The log message content
     * @param level The severity level of this log record
     */
    LogRecord(const std::string& message, LogLevel logLevel, LogColor logColor);
    
    /**
     * @brief Retrieve formatted timestamp string.
     *
     * <summary>
     * Allows selective display of date, time, or both.
     * Default behavior shows both date and time.
     * </summary>
     *
     * @param showDate If true, include date (YYYY-MM-DD)
     * @param showTime If true, include time (HH:MM:SS)
     * @return Formatted timestamp string
     */
    std::string GetTimestamp(bool showDate = true, bool showTime = true) const;
};

/**
 * @class AppLogger
 * @brief Thread-safe logger supporting console colors, file logging, and test mode.
 *
 * <summary>
 * AppLogger provides a singleton-style interface for logging messages to both
 * console and file. It supports multiple log levels, ANSI/Windows console colors,
 * and is safe to use across multiple threads. Logger initialization can be
 * done with default paths or user-specified folder/filename.
 * </summary>
 */
class AppLogger {
public:
    // ----- Configuration -----

    /**
     * @brief Set the minimum log level for output.
     *
     * <summary>
     * Messages below this level will be ignored.
     * Useful to filter verbose debug output in production environments.
     * </summary>
     *
     * @param logLevel Minimum severity level
     */
    static void SetLogLevel(LogLevel logLevel);

    /**
     * @brief Enable or disable test mode.
     *
     * <summary>
     * Test mode may modify logging behavior, e.g., bypass file output,
     * for unit tests or temporary debugging.
     * </summary>
     *
     * @param enabled True to enable test mode, false to disable
     */
    static void EnableTestMode(bool enabled);

    // ----- Logging Methods -----

    /**
     * @brief General-purpose logging entry point.
     *
     * <summary>
     * Logs a message with the specified severity level.
     * Thread-safe; messages are printed to console and written to file.
     * </summary>
     *
     * @param message The message string to log
     * @param level Severity level (default: Info)
     * @param appendNewline If true, append a newline character
     */
    static void Log(
        const std::string& message,
        LogLevel level = LogLevel::Info,
        bool appendNewline = true,
        std::optional<LogColor> customColor = std::nullopt
    );

    /// Convenience shortcuts for common log levels
    static void Trace(const std::string& message,
        bool appendNewline = true);
    static void Debug(const std::string& message,
        bool appendNewline = true);
    static void Info(const std::string& message,
        bool appendNewline = true);
    static void Warn(const std::string& message,
        bool appendNewline = true);
    static void Error(const std::string& message,
        bool appendNewline = true);
    static void Prompt(const std::string& message,
        bool appendNewline = true);
    static void Prompt(const std::string& message,
        LogColor logColor, bool appendNewline = true);

    // ----- Initialization / Shutdown -----

    /**
     * @brief Initialize logger with custom folder and/or filename template.
     *
     * <summary>
     * If folderPath or fileName are empty, defaults are used.
     * Creates folder if it does not exist and formats the filename using
     * `FilenameFormatter`.
     * </summary>
     *
     * @param folderPath Path to the log folder (defaults to "log")
     * @param fileName Filename template (defaults to "runtime_log_{year}-{month}-{day}_{hour}.{minute}.{second}.log")
     */
    static void Initialize(
        const std::filesystem::path& folderPath = DefaultFolder,
        const std::string& fileName = DefaultTemplate
    );

    /**
     * @brief Shut down the logger and flush any remaining logs.
     *
     * <summary>
     * Safely shuts down logging, writes remaining messages to file,
     * and releases resources. Safe to call multiple times.
     * </summary>
     */
    static void Shutdown();

private:
    // ----- Internal State -----

    static inline bool initialized_ = false;            ///< Flag indicating logger initialization
    static inline std::mutex mutex_;                    ///< Mutex for thread-safe log writes
    static inline LogLevel minLevel_ = LogLevel::Info;  ///< Minimum log level
    static inline bool testMode_ = false;               ///< Test mode flag

    static std::mutex logMutex_;                        ///< Mutex for initialization/shutdown

    static inline const std::filesystem::path DefaultFolder = "log";     ///< Default folder
    static inline const std::string DefaultTemplate =
        "runtime_log_{year}-{month}-{day}_{hour}.{minute}.{second}.log"; ///< Default filename template


    // ----- Internal Helpers -----

    /**
     * @brief Initialize LogFileManager with a given file path.
     *
     * @param filePath Full path to the log file
     */
    static void InitializeFileManager(const std::filesystem::path& filePath);

    /**
     * @brief Print a formatted LogRecord to console and file.
     *
     * @param logRecord LogRecord instance
     * @param appendNewline If true, append a newline character
     */
    static void Print(const LogRecord& logRecord, bool appendNewline);

    /**
     * @brief Get textual label for a log level.
     *
     * @param logLevel LogLevel
     * @return String label (e.g., "[INFO ]")
     */
    static std::string GetLevelLabel(LogLevel logLevel);
};
