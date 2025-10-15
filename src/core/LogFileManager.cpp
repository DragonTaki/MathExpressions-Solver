/* ----- ----- ----- ----- */
// LogFileManager.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/13
// Update Date: 2025/10/13
// Version: v1.0
/* ----- ----- ----- ----- */

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "LogFileManager.h"

namespace fs = std::filesystem;

// Static member initialization
std::ofstream LogFileManager::ofs_;     ///< File stream for writing logs
std::mutex LogFileManager::mutex_;      ///< Mutex for thread-safety
std::string LogFileManager::logPath_;   ///< Stores current log file path

/**
 * @brief Generates a human-readable timestamp for log lines in format "YYYY-MM-DD HH:MM:SS".
 * @return Formatted timestamp string.
 */
static std::string MakeTimestampForLogLine() {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};

#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%F %T");  // Format: 2025-10-14 10:15:30
    return oss.str();
}

/**
 * @brief Initializes the log file.
 * @param filename Full path of the log file to create or append.
 * @return true if the file is successfully opened or already initialized; false otherwise.
 * @remarks Writes a session header with the current timestamp to indicate log start.
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

    // Write log session header with timestamp
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ofs_ << "\n========== Log Session Started: "
          << std::put_time(std::localtime(&now), "%F %T")
          << " ==========\n";
    ofs_.flush();  // Ensure header is written immediately
    return true;
}

void LogFileManager::SetSilentMode(bool enabled) {
    silentMode = enabled;
}

bool LogFileManager::IsSilentMode() {
    return silentMode;
}

/**
 * @brief Writes a message to the log file with timestamp.
 * @param line The message string to write.
 * @remarks Thread-safe; automatically adds a newline. If file is not initialized, prints error to std::cerr.
 */
void LogFileManager::Write(const std::string& line) {
    std::lock_guard<std::mutex> lock(mutex_);  // Ensure thread-safety

    if (silentMode) {  // Pass function if in silent mode
        return;
    }

    if (!ofs_.is_open()) {  // Log file not initialized
        std::cerr << "[LogFileManager] Log file not initialized. Message skipped: "
                  << line << std::endl;
        return;
    }

    // Write timestamped log line
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ofs_ << "[" << std::put_time(std::localtime(&now), "%F %T") << "] " << line << std::endl;
    ofs_.flush();  // Flush immediately for reliability
}

/**
 * @brief Shuts down the log file safely.
 * @remarks Writes a session footer with timestamp and closes the file. Thread-safe.
 */
void LogFileManager::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!ofs_.is_open())  // Already closed
        return;

    // Write session footer
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ofs_ << "========== Log Session Ended: "
          << std::put_time(std::localtime(&now), "%F %T")
          << " ==========\n\n";
    ofs_.close();  // Close file stream
}

/**
 * @brief Returns the currently active log file path.
 * @return Current log file path as std::string.
 * @remarks Thread-safe access to logPath_.
 */
std::string LogFileManager::GetLogPath() {
    std::lock_guard<std::mutex> lock(mutex_);
    return logPath_;
}
