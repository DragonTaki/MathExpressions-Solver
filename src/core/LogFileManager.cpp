/* ----- ----- ----- ----- */
// LogFileManager.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/13
// Update Date: 2025/10/13
// Version: v1.0
/* ----- ----- ----- ----- */

#include "LogFileManager.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>

namespace fs = std::filesystem;

std::ofstream LogFileManager::ofs_;
std::mutex LogFileManager::mutex_;
std::string LogFileManager::logPath_;

static std::string MakeTimestampForFilename() {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

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
    oss << std::put_time(&tm, "%F %T");
    return oss.str();
}

bool LogFileManager::Initialize(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (ofs_.is_open()) {
        // Already initialized
        return true;
    }

    ofs_.open(filename, std::ios::out | std::ios::app);
    if (!ofs_.is_open()) {
        std::cerr << "[LogFileManager] Failed to open log file: " << filename << std::endl;
        return false;
    }
    logPath_ = filename;

    // Write session header
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ofs_ << "\n========== Log Session Started: "
          << std::put_time(std::localtime(&now), "%F %T")
          << " ==========\n";
    ofs_.flush();
    return true;
}

void LogFileManager::Write(const std::string& line) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!ofs_.is_open()) {
        std::cerr << "[LogFileManager] Log file not initialized. Message skipped: "
                  << line << std::endl;
        return;
    }

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ofs_ << "[" << std::put_time(std::localtime(&now), "%F %T") << "] " << line << std::endl;
    ofs_.flush();
}

void LogFileManager::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!ofs_.is_open())
        return;

    // Write session footer
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ofs_ << "========== Log Session Ended: "
          << std::put_time(std::localtime(&now), "%F %T")
          << " ==========\n\n";
    ofs_.close();
}

std::string LogFileManager::GetLogPath() {
    std::lock_guard<std::mutex> lock(mutex_);
    return logPath_;
}
