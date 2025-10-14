/* ----- ----- ----- ----- */
// AppLogger.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/01
// Update Date: 2025/10/05
// Version: v2.0
/* ----- ----- ----- ----- */

#include "AppLogger.h"
#include "LogFileManager.h"
#include "FilenameFormatter.h"
#include <chrono>
#include <ctime>
#include <filesystem>
#include <iostream>

std::mutex AppLogger::logMutex;

// --- Internal default settings ---
namespace {
    constexpr const char* DEFAULT_LOG_FOLDER = "log";
    constexpr const char* DEFAULT_LOG_FILENAME_TEMPLATE = "runtime_log_{year}-{month}-{day}_{hour}.{minute}.{second}.log";
}

// --- Shortcuts ---
void AppLogger::Trace(const std::string& msg) { Log(msg, LogLevel::Trace); }
void AppLogger::Debug(const std::string& msg) { Log(msg, LogLevel::Debug); }
void AppLogger::Info(const std::string& msg)  { Log(msg, LogLevel::Info);  }
void AppLogger::Warn(const std::string& msg)  { Log(msg, LogLevel::Warn);  }
void AppLogger::Error(const std::string& msg) { Log(msg, LogLevel::Error); }

/**
 * @brief Initialize logger with default folder and filename.
 */
void AppLogger::Initialize()
{
    std::lock_guard<std::mutex> lock(logMutex);

    std::filesystem::create_directories(DefaultFolder);
    std::filesystem::path filePath = DefaultFolder / FilenameFormatter::Format(DefaultTemplate);

    InitializeFileManager(filePath);
}

/**
 * @brief Initialize logger with a user-specified folder or filename.
 */
void AppLogger::Initialize(const std::string& folderOrFile)
{
    std::lock_guard<std::mutex> lock(logMutex);

    std::filesystem::path folder = DefaultFolder;
    std::string filenameTemplate = DefaultTemplate;

    if (folderOrFile.empty())
    {
        std::filesystem::create_directories(folder);
    }
    else if (folderOrFile.find('.') == std::string::npos)
    {
        // folder mode
        folder = folderOrFile;
        std::filesystem::create_directories(folder);
    }
    else
    {
        // file mode
        std::filesystem::path provided = folderOrFile;
        folder = provided.parent_path().empty() ? DefaultFolder : provided.parent_path();
        filenameTemplate = provided.filename().string();
        std::filesystem::create_directories(folder);
    }

    std::filesystem::path filePath = folder / FilenameFormatter::Format(filenameTemplate);
    InitializeFileManager(filePath);
}

/**
 * @brief Common helper used by both Initialize() overloads.
 * @param filePath Full log file path.
 */
void AppLogger::InitializeFileManager(const std::filesystem::path& filePath)
{
    if (LogFileManager::Initialize(filePath.string()))
    {
        AppLogger::Info("Logger initialized with file: " + filePath.string());
    }
    else
    {
        std::cerr << "[AppLogger] Failed to initialize log file: " << filePath << std::endl;
    }
}

void AppLogger::Shutdown() {
    AppLogger::Info("Logger shutting down.");
    LogFileManager::Shutdown();
}

// --- Construct LogRecord ---
LogRecord::LogRecord(const std::string& msg, LogLevel lvl)
    : message(msg), level(lvl)
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    timestamp = oss.str();
}

// --- Static Config ---
namespace {
    struct AppLoggerFinalizer {
        ~AppLoggerFinalizer() {
            AppLogger::Shutdown();
        }
    } _autoFinalizer;
}

void AppLogger::SetLogLevel(LogLevel level) {
    minLevel_ = level;
}

void AppLogger::EnableTestMode(bool enabled) {
    testMode_ = enabled;
}

// --- Logging Entry Point ---
void AppLogger::Log(const std::string& message, LogLevel level) {
    if (level < minLevel_) return;
    std::lock_guard<std::mutex> lock(mutex_);
    LogRecord record(message, level);
    Print(record);
}

// --- Internal Print ---
void AppLogger::Print(const LogRecord& record) {
    std::string label = GetLevelLabel(record.level);
    LogColor color = GetLevelColor(record.level);

    std::ostringstream oss;
    oss << "[" << record.timestamp << "] "
        << label << " " << record.message;
    std::string outLine = oss.str();

#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD attr = ColorToWindowsAttr(color);
    SetConsoleTextAttribute(hConsole, attr);
    std::cout << outLine << std::endl;
    SetConsoleTextAttribute(hConsole, ColorToWindowsAttr(LogColor::Default));
#else
    std::cout << ColorToAnsi(color)
              << outLine
              << ColorToAnsi(LogColor::Default) << std::endl;
#endif

    // File: 直接寫入已格式化好的字串（LogFileManager 不會再加時間）
    LogFileManager::Write(outLine);
}

// --- Helpers ---
std::string AppLogger::GetLevelLabel(LogLevel level) {
    switch (level) {
    case LogLevel::Trace: return "[TRACE]";
    case LogLevel::Debug: return "[DEBUG]";
    case LogLevel::Info:  return "[INFO ]";
    case LogLevel::Warn:  return "[WARN ]";
    case LogLevel::Error: return "[ERROR]";
    default:              return "[UNKWN]";
    }
}

LogColor AppLogger::GetLevelColor(LogLevel level) {
    switch (level) {
    case LogLevel::Trace: return LogColor::Gray;
    case LogLevel::Debug: return LogColor::Gray;
    case LogLevel::Info:  return LogColor::White;
    case LogLevel::Warn:  return LogColor::Yellow;
    case LogLevel::Error: return LogColor::Red;
    default:              return LogColor::Default;
    }
}

std::string AppLogger::ColorToAnsi(LogColor c) {
    switch (c) {
    case LogColor::Gray:    return "\033[90m";
    case LogColor::Red:     return "\033[91m";
    case LogColor::Yellow:  return "\033[93m";
    case LogColor::Green:   return "\033[92m";
    case LogColor::Cyan:    return "\033[96m";
    case LogColor::Blue:    return "\033[94m";
    case LogColor::Magenta: return "\033[95m";
    case LogColor::White:   return "\033[97m";
    default:                return "\033[0m";
    }
}

#ifdef _WIN32
WORD AppLogger::ColorToWindowsAttr(LogColor c) {
    switch (c) {
    case LogColor::Gray:    return FOREGROUND_INTENSITY;
    case LogColor::Red:     return FOREGROUND_RED | FOREGROUND_INTENSITY;
    case LogColor::Yellow:  return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    case LogColor::Green:   return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    case LogColor::Cyan:    return FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    case LogColor::Blue:    return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    case LogColor::Magenta: return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    case LogColor::White:   return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    default:                return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}
#endif
