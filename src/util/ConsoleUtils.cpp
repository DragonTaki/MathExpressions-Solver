/* ----- ----- ----- ----- */
// ConsoleUtils.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/16
// Update Date: 2025/10/16
// Version: v1.0
/* ----- ----- ----- ----- */

#include "ConsoleUtils.h"
#include <algorithm>

#include "core/logging/AppLogger.h"

#define FMT_HEADER_ONLY
#include "core.h"
#include "ranges.h" 

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace ConsoleUtils {

/**
 * @brief Implementation of getConsoleWidth()
 *
 * <summary>
 * Platform-dependent logic to query the width of the terminal window.
 * - Windows: Uses `GetConsoleScreenBufferInfo` to get window size.
 * - Unix-like: Uses `ioctl` with `TIOCGWINSZ` on STDOUT file descriptor.
 * Returns a default of 80 characters if the width cannot be determined.
 * </summary>
 *
 * @return int Current console width in characters.
 */
int getConsoleWidth() {
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return 80;  // fallback
#else
    struct winsize w{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
        return w.ws_col;
    return 80;  // fallback
#endif
}

/**
 * @brief Implementation of printCandidatesInline()
 *
 * <summary>
 * Prints the vector of candidate strings in an inline table with multiple columns.
 * The number of columns is computed based on console width and string length.
 * Each candidate is padded to align neatly in columns. Supports arbitrary
 * number of candidates and ensures the last row is properly padded.
 *
 * Logic:
 * 1. Return immediately if the candidate list is empty.
 * 2. Determine available width per candidate based on console width / 3.5.
 * 3. Compute number of columns that can fit in a row.
 * 4. Compute number of rows needed (rounding up).
 * 5. Iterate over rows and columns to print each candidate at its correct position.
 * 6. Pad empty cells with spaces in the last row if necessary.
 * </summary>
 *
 * @param candidatesList Vector of candidate strings to display.
 */
void printCandidatesInline(const std::vector<std::string>& candidatesList) {
    if (candidatesList.empty()) return;

    int displayWidth = getConsoleWidth() / 3;
    size_t exprLength = candidatesList[0].size();
    int spaceBetweenWidth = 1;
    // Calculate how many columns can fit in each row
    int totalWidth = displayWidth + spaceBetweenWidth;
    int cellWidth = static_cast<int>(exprLength + spaceBetweenWidth);
    int columnCount = (std::max)(1, totalWidth / cellWidth);

    int rowCount = (candidatesList.size() + columnCount - 1) / columnCount;  // Round up to get the number of rows

    AppLogger::Prompt("Answer candidate(s):", LogColor::Cyan);
    std::string rowString;
    for (int r = 0; r < rowCount; ++r) {
        for (int c = 0; c < columnCount; ++c) {
            int idx = r + c * rowCount;  // Compute index for column-major printing
            if (idx < candidatesList.size()) {
                rowString += fmt::format("{:<{}}", candidatesList[idx], static_cast<int>(exprLength + spaceBetweenWidth));
            } else {
                rowString += fmt::format("{:<{}}", "", static_cast<int>(exprLength + spaceBetweenWidth));  // Fill empty cell
            }
        }
        rowString += "\n";
    }
    AppLogger::Prompt(rowString, LogColor::Green);
}

}  // namespace (end of ConsoleUtils)
