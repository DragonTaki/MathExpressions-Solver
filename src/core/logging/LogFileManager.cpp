/* ----- ----- ----- ----- */
// LogFileManager.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/13
// Update Date: 2025/10/13
// Version: v1.0
/* ----- ----- ----- ----- */

#include "LogFileManager.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

/**
 * @file LogFileManager.cpp
 * @brief Implements the LogFileManager class for handling file-based logging.
 *
 * <summary>
 * This source file provides the implementation for managing a thread-safe log file.
 * It handles log initialization, writing, and shutdown operations while maintaining
 * a consistent format and timestamped session headers/footers. The LogFileManager
 * is designed to be used as a static singleton utility within a larger logging system.
 * </summary>
 */

// ----- Static Member Initialization -----

std::mutex LogFileManager::mutex_;      ///< Mutex for thread-safe initialization and shutdown
std::ofstream LogFileManager::ofs_;     ///< File stream for writing logs
std::string LogFileManager::logPath_;   ///< Stores current log file path

// ----- Public Functions -----

/**
 * @brief Enables or disables silent mode for the log system.
 *
 * <summary>
 * When silent mode is enabled, all file output operations are suppressed.
 * This is useful for test environments or temporary disabling of file logs
 * without altering the rest of the system behavior.
 * </summary>
 *
 * @param enabled True to enable silent mode, false to disable.
 */
void LogFileManager::SetSilentMode(bool enabled) {
    silentMode = enabled;
}

/**
 * @brief Checks whether silent mode is currently enabled.
 * @return True if silent mode is active, otherwise false.
 */
bool LogFileManager::IsSilentMode() {
    return silentMode;
}


/**
 * @brief Retrieves the path of the currently active log file.
 *
 * <summary>
 * This function provides read-only access to the current log file path.
 * It uses a mutex to ensure thread-safe access, allowing concurrent reads
 * from multiple threads without data races.
 * </summary>
 *
 * @return A std::string representing the full path of the current log file.
 */
std::string LogFileManager::GetLogPath() {
    std::lock_guard<std::mutex> lock(mutex_);
    return logPath_;
}

/**
 * @brief Initializes and opens the log file for writing.
 *
 * <summary>
 * This function creates or appends to the specified log file. It ensures
 * thread-safe initialization using a mutex. If the file cannot be opened,
 * an error message is printed to std::cerr.
 *
 * Upon successful initialization, a session header is written to the file,
 * indicating the start of a new logging session with a timestamp.
 * </summary>
 *
 * @param filename The full path to the log file to create or append.
 * @return True if the log file was successfully opened or already initialized, false otherwise.
 *
 * @note This function must be called before any Write() calls.
 */
bool LogFileManager::Initialize(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);  // Ensure thread-safety

    if (ofs_.is_open()) {  // If already initialized
        return true;
    }

    ofs_.open(filename, std::ios::out | std::ios::app);
    if (!ofs_.is_open()) {  // Failed to open file
        std::cerr << "[LogFileManager] Failed to open log file: " << filename << std::endl;
        return false;
    }

    logPath_ = filename;  // Store current log file path

    // Write log session header with current timestamp
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ofs_ << "\n========== Log Session Started: "
         << std::put_time(std::localtime(&now), "%F %T")
         << " ==========\n";
    ofs_.flush();  // Ensure header is written immediately
    return true;
}

/**
 * @brief Safely closes the log file and writes a session footer.
 *
 * <summary>
 * This function terminates the current log session by appending a timestamped
 * footer message. It then closes the file stream to release the file handle.
 * Thread-safe via mutex locking.
 * </summary>
 *
 * @note If the log file is already closed, this function does nothing.
 */
void LogFileManager::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!ofs_.is_open())  // Already closed
        return;

    // Write session footer with timestamp
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ofs_ << "========== Log Session Ended: "
         << std::put_time(std::localtime(&now), "%F %T")
         << " ==========\n\n";
    ofs_.close();  // Close file stream
}

/**
 * @brief Writes a single log line to the file with thread safety.
 *
 * <summary>
 * This method appends the given text line to the currently open log file.
 * If silent mode is active, the function returns immediately without writing.
 * If the file has not been initialized, the function outputs an error message
 * to std::cerr instead.
 *
 * The method automatically appends a newline and flushes the stream after writing,
 * ensuring the data is written even in case of application crashes or early exits.
 * </summary>
 *
 * @param line The text line to write into the log file.
 *
 * @note This function is fully thread-safe and can be called concurrently
 *       from multiple threads.
 * @warning If the file is not open and silent mode is disabled, the message
 *          will be lost after printing an error.
 */
void LogFileManager::Write(const std::string& line) {
    std::lock_guard<std::mutex> lock(mutex_);  // Ensure thread-safety

    if (silentMode) {  // Pass function if in silent mode
        return;
    }

    if (!ofs_.is_open()) {  // Log file not initialized
        std::cerr << "[LogFileManager] Log file not initialized. Message skipped:\n  "
            << line << std::endl;
        return;
    }

    ofs_.flush();  // Flush immediately for reliability
}
