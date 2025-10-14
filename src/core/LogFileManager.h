/* ----- ----- ----- ----- */
// LogFileManager.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/13
// Update Date: 2025/10/13
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>
#include <fstream>
#include <mutex>

/**
 * @class LogFileManager
 * @brief Thread-safe logging utility for writing messages to a file.
 *
 * <summary>
 * LogFileManager provides static methods for:
 * - Initializing a log file
 * - Writing timestamped log messages
 * - Safely shutting down the logging system
 * </summary>
 *
 * <remarks>
 * This is a static-only utility class; instantiation is disabled.
 * All operations are thread-safe via an internal mutex.
 * Each log line is prepended with a timestamp.
 * </remarks>
 *
 * <example>
 * LogFileManager::Initialize("app.log");
 * LogFileManager::Write("Application started.");
 * LogFileManager::Shutdown();
 * </example>
 */
class LogFileManager {
public:
    /**
     * @brief Initializes the log file for writing.
     *
     * <summary>
     * Opens the specified file in append mode.
     * If already initialized, the call is ignored.
     * Writes a session header with timestamp to the log file.
     * </summary>
     *
     * @param filename Full path to the log file.
     * @return true if the log file was successfully opened or already open; false otherwise.
     */
    static bool Initialize(const std::string& filename);

    /**
     * @brief Writes a timestamped log message to the file.
     *
     * <summary>
     * Each message is prepended with a timestamp in format "YYYY-MM-DD HH:MM:SS".
     * Thread-safe. Automatically flushes the output stream after writing.
     * </summary>
     *
     * @param line The message to write to the log file.
     */
    static void Write(const std::string& line);

    /**
     * @brief Shuts down the logging system.
     *
     * <summary>
     * Writes a session footer with timestamp and closes the file stream safely.
     * Should be called before program exit to release resources.
     * </summary>
     */
    static void Shutdown();

    /**
     * @brief Retrieves the current active log file path.
     *
     * @return Path to the currently open log file as a string.
     */
    static std::string GetLogPath();

private:
    // Prevent instantiation
    LogFileManager() = delete;
    ~LogFileManager() = delete;

    static std::ofstream ofs_;     ///< File stream for writing logs
    static std::mutex mutex_;      ///< Mutex to ensure thread-safety
    static std::string logPath_;   ///< Current log file path
};
