/* ----- ----- ----- ----- */
// LogLevel.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/18
// Update Date: 2025/10/18
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once

/**
 * @enum LogLevel
 * @brief Defines the severity levels for log messages.
 *
 * <summary>
 * LogLevel controls which messages are displayed based on their importance.
 * Lower levels (Trace, Debug) are verbose and used for debugging,
 * while higher levels (Warn, Error) indicate potential issues or errors.
 * </summary>
 */
enum class LogLevel {
    Trace,  ///< Fine-grained debug messages for tracing program flow.
    Debug,  ///< Standard debug messages with detailed information.
    Info,   ///< General informational messages.
    Warn,   ///< Warning messages indicating potential issues.
    Error,  ///< Error messages indicating failures.
    Prompt  ///< For plain console prompts (no timestamp or tag).
};
