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
 * @brief A thread-safe logging utility for writing messages to a log file.
 *
 * This class provides static methods for initializing a log file,
 * writing timestamped messages, and safely shutting down the log system.
 *
 * Example usage:
 * @code
 * LogFileManager::Initialize("app.log");
 * LogFileManager::Write("Application started.");
 * LogFileManager::Shutdown();
 * @endcode
 */
class LogFileManager {
public:
    /**
     * @brief Initializes the log file.
     * @param filename Path to the log file.
     *
     * Opens the specified log file in append mode.
     * Should be called once at program start.
     */
    static bool Initialize(const std::string& filename);

    /**
     * @brief Writes a message to the log file with a timestamp.
     * @param line The message to write.
     *
     * Thread-safe. Automatically adds a newline at the end.
     */
    static void Write(const std::string& line);

    /**
     * @brief Closes the log file safely.
     *
     * Should be called before program exit to release resources.
     */
    static void Shutdown();

    /**
     * @brief Get the current active log file path.
     * @return The log file path as a string.
     */
    static std::string GetLogPath();

private:
    // Disable instantiation (static-only utility class)
    LogFileManager() = delete;
    ~LogFileManager() = delete;

    static std::ofstream ofs_;     ///< Active file stream
    static std::mutex mutex_;      ///< Thread-safety lock
    static std::string logPath_;   ///< Current log file path
};
