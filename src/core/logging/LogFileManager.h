/* ----- ----- ----- ----- */
// LogFileManager.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/13
// Update Date: 2025/10/13
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <fstream>
#include <mutex>
#include <string>

/**
 * @class LogFileManager
 * @brief Thread-safe logging utility for writing messages to a file.
 *
 * <summary>
 * LogFileManager provides a static, globally accessible mechanism for safely managing
 * application log output. It handles file initialization, writing, and cleanup,
 * ensuring thread-safe access using a mutex lock.
 *
 * Key features:
 * - Thread-safe file writes
 * - Automatic session headers and footers with timestamps
 * - Silent mode for disabling file output (useful for testing)
 * - Static-only design (cannot be instantiated)
 * </summary>
 *
 * <remarks>
 * This class is fully static — it cannot be instantiated or copied.
 * All operations are protected by an internal mutex (`mutex_`), ensuring
 * that concurrent log writes from multiple threads do not conflict.
 *
 * The class writes session metadata when the log starts and ends, including timestamps.
 * If `Initialize()` fails, all subsequent write operations will print a warning to `std::cerr`.
 * </remarks>
 *
 * <example>
 * Example usage:
 * @code
 * LogFileManager::Initialize("app.log");          // Open or create a log file
 * LogFileManager::Write("Application started.");  // Write a log message
 * LogFileManager::Shutdown();                     // Close and finalize the log session
 * @endcode
 * </example>
 */
class LogFileManager {
public:

    /**
     * @brief Enables or disables silent mode for the logger.
     *
     * <summary>
     * When silent mode is enabled, all file write operations are ignored.
     * This is useful for suppressing log output during automated tests or benchmarks.
     * </summary>
     *
     * @param enabled True to enable silent mode, false to disable.
     */
    static void SetSilentMode(bool enabled);

    /**
     * @brief Checks whether silent mode is currently active.
     *
     * @return True if silent mode is enabled, false otherwise.
     */
    static bool IsSilentMode();

    /**
     * @brief Retrieves the currently active log file path.
     *
     * <summary>
     * This function provides thread-safe access to the currently opened log file path.
     * If no log file is initialized, the returned string may be empty.
     * </summary>
     *
     * @return Path to the currently open log file as a std::string.
     */
    static std::string GetLogPath();

    /**
     * @brief Initializes the log file for writing.
     *
     * <summary>
     * Opens the specified log file in append mode.
     * If the file is already open (previously initialized), the function does nothing.
     * A session header with the current timestamp is written to mark the start of a new log session.
     * </summary>
     *
     * @param filename Full path to the log file (e.g., "log/app.log").
     * @return true if the log file was successfully opened or already open; false otherwise.
     *
     * @remarks
     * - The function ensures thread safety using an internal mutex.
     * - It automatically flushes the session header after writing.
     * - The timestamp format follows "YYYY-MM-DD HH:MM:SS".
     */
    static bool Initialize(const std::string& filename);

    /**
     * @brief Shuts down the logging system and closes the file stream.
     *
     * <summary>
     * Writes a footer line containing a closing timestamp to indicate the end
     * of the log session, then closes the file safely. If the file is already closed,
     * this call is ignored.
     * </summary>
     *
     * @remarks
     * This should be called once before the program exits, to ensure all logs are flushed
     * and the file handle is released.
     */
    static void Shutdown();

    /**
     * @brief Writes a log message to the file with timestamp.
     *
     * <summary>
     * Appends the specified message to the current log file, prefixed with a timestamp
     * formatted as "YYYY-MM-DD HH:MM:SS". Automatically flushes after writing to ensure
     * immediate disk persistence.
     * </summary>
     *
     * @param line The message text to write to the log file.
     *
     * @remarks
     * - Thread-safe: Uses an internal mutex to serialize access.
     * - If the log file is not initialized, the message is written to `std::cerr`.
     * - If silent mode is enabled, the message is skipped entirely.
     */
    static void Write(const std::string& line);


private:
    // ----- Constructors Disabled -----

    /**
     * @brief Deleted default constructor to prevent instantiation.
     *
     * <summary>
     * LogFileManager is a static-only class; therefore, creating instances
     * of this class is not allowed.
     * </summary>
     */
    LogFileManager() = delete;

    /**
     * @brief Deleted destructor to prevent direct destruction.
     *
     * <summary>
     * Since LogFileManager only contains static members,
     * its lifecycle is managed by the program automatically.
     * </summary>
     */
    ~LogFileManager() = delete;
    
    // ----- Static Members -----

    static inline bool silentMode = false;  ///< Global flag to control whether file output is suppressed

    static std::ofstream ofs_;              ///< File stream for writing logs
    static std::mutex mutex_;               ///< Mutex to ensure thread-safety
    static std::string logPath_;            ///< Current log file path
};
