/* ----- ----- ----- ----- */
// AppLogger.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/01
// Update Date: 2025/10/17
// Version: v2.1
/* ----- ----- ----- ----- */

#include "AppLogger.h"
#include <chrono>
#include <ctime>
#include <filesystem>
#include <iostream>

#include "ConsoleColor.h"
#include "FilenameFormatter.h"
#include "LogFileManager.h"

std::mutex AppLogger::logMutex_;  ///< Mutex for thread-safe initialization and shutdown

// ----- Public Functions -----

/**
 * @brief Set the minimum log level for output.
 *
 * <summary>
 * Any log messages below the specified `logLevel` will be ignored.
 * This allows filtering out verbose logs in production environments.
 * </summary>
 *
 * @param logLevel Minimum log level to output.
 */
void AppLogger::SetLogLevel(LogLevel logLevel) {
    minLevel_ = logLevel;
}

/**
 * @brief Enable or disable test mode.
 *
 * <summary>
 * Test mode may disable file logging or alter behavior for unit testing purposes.
 * </summary>
 *
 * @param enabled true to enable test mode, false to disable
 */
void AppLogger::EnableTestMode(bool enabled) {
    testMode_ = enabled;
}

/**
 * @brief Primary logging function.
 *
 * <summary>
 * Logs a message with a specified severity level. Messages below the current
 * minimum level are ignored. Thread-safe using `mutex_`.
 * </summary>
 *
 * @param message The log message string.
 * @param logLevel Severity level of the message.
 * @param appendNewline If true, appends a newline character to the message.
 */
void AppLogger::Log(
    const std::string& message,
    LogLevel logLevel,
    bool appendNewline,
    std::optional<LogColor> customColor
) {
    if (logLevel < minLevel_) return;

    std::lock_guard<std::mutex> lock(mutex_);  ///< Ensure thread-safe logging

    // 若沒有指定顏色，則依照level取得預設顏色
    LogColor logColor = customColor.has_value()
        ? customColor.value()
        : ConsoleColor::GetLevelColor(logLevel);

    LogRecord record(message, logLevel, logColor);  ///< Construct log record with timestamp
    Print(record, appendNewline);                   ///< Print to console and file
}

// --- Convenience logging shortcuts ---
void AppLogger::Trace(const std::string& message, bool appendNewline) {
    Log(message, LogLevel::Trace, appendNewline);
}
void AppLogger::Debug(const std::string& message, bool appendNewline) {
    Log(message, LogLevel::Debug, appendNewline);
}
void AppLogger::Info(const std::string& message, bool appendNewline) {
    Log(message, LogLevel::Info, appendNewline);
}
void AppLogger::Warn(const std::string& message, bool appendNewline) {
    Log(message, LogLevel::Warn, appendNewline);
}
void AppLogger::Error(const std::string& message, bool appendNewline) {
    Log(message, LogLevel::Error, appendNewline);
}
void AppLogger::Prompt(const std::string& message, bool appendNewline) {
    Log(message, LogLevel::Prompt, appendNewline);
}
void AppLogger::Prompt(const std::string& message, LogColor logColor, bool appendNewline) {
    Log(message, LogLevel::Prompt, appendNewline, logColor);
}

/**
 * @brief Initialize logger with custom folder and/or filename.
 *
 * <summary>
 * If `folderPath` or `fileName` are empty, defaults are used. Creates the folder
 * if it does not exist and generates a properly formatted filename.
 * Thread-safe via `logMutex_`.
 * </summary>
 *
 * @param folderPath Path to folder to store log file.
 * @param fileName Filename template to use for log file.
 */
void AppLogger::Initialize(
    const std::filesystem::path& folderPath,
    const std::string& fileName
) {
    std::lock_guard<std::mutex> lock(logMutex_);

    std::filesystem::path folder = folderPath.empty() ? DefaultFolder : folderPath;
    std::string filenameTemplate = fileName.empty() ? DefaultTemplate : fileName;

    std::filesystem::create_directories(folder);
    std::filesystem::path filePath = folder / FilenameFormatter::Format(filenameTemplate);

    InitializeFileManager(filePath);
}

/**
 * @brief Shutdown logger and flush any remaining logs.
 *
 * <summary>
 * Safely shuts down logging by writing remaining messages to file and
 * releasing resources. Does nothing if logger is not initialized.
 * </summary>
 */
void AppLogger::Shutdown() {
    if (!initialized_) return;

    AppLogger::Info("Logger shutting down.");
    LogFileManager::Shutdown();

    initialized_ = false;
}

// --- Auto shutdown via RAII ---
namespace {
    struct AppLoggerFinalizer {
        ~AppLoggerFinalizer() {
            AppLogger::Shutdown();  ///< Ensure logger is flushed at program exit
        }
    } _autoFinalizer;
}

// ----- Constructor Implementation -----

/**
 * @brief Construct a LogRecord with timestamp.
 *
 * <summary>
 * Stores the log message, level, and the current timestamp formatted as
 * "YYYY-MM-DD HH:MM:SS".
 * </summary>
 *
 * @param message Log message string.
 * @param logLevel Log level for this record.
 */
LogRecord::LogRecord(const std::string& message, LogLevel logLevel, LogColor logColor)
    : message(message), level(logLevel), color(logColor)
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
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    fullTimestamp = oss.str();
}

/**
 * @brief Retrieve formatted timestamp string.
 *
 * @param showDate Include date portion (YYYY-MM-DD).
 * @param showTime Include time portion (HH:MM:SS).
 * @return Formatted timestamp string based on flags.
 */
std::string LogRecord::GetTimestamp(bool showDate, bool showTime) const {
    if (showDate && showTime) return fullTimestamp;
    if (showDate) return fullTimestamp.substr(0, 10); // YYYY-MM-DD
    if (showTime) return fullTimestamp.substr(11, 8); // HH:MM:SS
    return "";
}

// ----- Helper Functions -----

/**
 * @brief Internal helper to initialize LogFileManager.
 *
 * <summary>
 * Initializes the log file for writing. If initialization fails, writes
 * an error to `std::cerr` and sets `initialized_` to false.
 * </summary>
 *
 * @param filePath Full path to the log file.
 */
void AppLogger::InitializeFileManager(const std::filesystem::path& filePath)
{
    if (initialized_) return;

    if (LogFileManager::Initialize(filePath.string()))
    {
        AppLogger::Info("Logger initialized with file: " + filePath.string());
        initialized_ = true;
    }
    else
    {
        std::cerr << "[AppLogger] Failed to initialize log file: " << filePath << std::endl;
        initialized_ = false;
    }
}

/**
 * @brief Print log record to console and file with appropriate color.
 *
 * <summary>
 * Formats the log message and applies colors to console output.
 * File output always includes date + time; console output can omit date.
 * Thread-safe via internal mutex if called via `Log`.
 * </summary>
 *
 * @param logRecord LogRecord instance containing message and timestamp.
 * @param appendNewline Whether to append a newline character.
 */
void AppLogger::Print(const LogRecord& logRecord, bool appendNewline) {
    std::string logLabel = GetLevelLabel(logRecord.level);
    LogColor logColor = logRecord.color.has_value()
        ? logRecord.color.value()
        : ConsoleColor::GetLevelColor(logRecord.level);

    // Check if is prompt level
    bool isPrompt = (logRecord.level == LogLevel::Prompt);

    // Lambda to generate log line string
    auto makeLogLine = [&](bool showDate, bool showTime) -> std::string {
        std::ostringstream logLine;
        if (isPrompt) {
            logLine << logRecord.message;
        } else {
            logLine << "["
                << logRecord.GetTimestamp(showDate, showTime) << " "
                << logLabel << "] "
                << logRecord.message;
        }
        if (appendNewline) logLine << '\n';
        return logLine.str();
    };

    std::string consoleString = makeLogLine(false, true);  // Only show time on console
    std::string fileString    = makeLogLine(true, true);   // Record fill date + time in log file

#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD attr = ConsoleColor::ToWindowsAttr(logColor);
    SetConsoleTextAttribute(hConsole, attr);
    std::cout << consoleString;
    SetConsoleTextAttribute(hConsole, ConsoleColor::ToWindowsAttr(LogColor::Default));
#else
    std::cout << ConsoleColor::ToAnsi(logColor)
        << consoleString
        << ConsoleColor::ToAnsi(LogColor::Default);
#endif

    LogFileManager::Write(fileString);
}

/**
 * @brief Retrieve textual label for a log level.
 *
 * @param logLevel LogLevel
 * @return std::string Label string (e.g., "[INFO ]")
 */
std::string AppLogger::GetLevelLabel(LogLevel logLevel) {
    switch (logLevel) {
    case LogLevel::Trace: return "TRACE";
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Info:  return "INFO ";
    case LogLevel::Warn:  return "WARN ";
    case LogLevel::Error: return "ERROR";
    default:              return "UNKWN";
    }
}
