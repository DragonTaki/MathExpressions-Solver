/* ----- ----- ----- ----- */
// ConsoleUtils.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/16
// Update Date: 2025/10/16
// Version: v1.0
/* ----- ----- ----- ----- */

#include "ConsoleUtils.h"

#define FMT_HEADER_ONLY
#include "core.h"
#include "ranges.h" 

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

int getConsoleWidth() {
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return 80; // fallback
#else
    struct winsize w{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
        return w.ws_col;
    return 80; // fallback
#endif
}

void printCandidatesInline(const std::vector<std::string>& candidates) {
    if (candidates.empty()) return;

    int width = getConsoleWidth() / 3.5;
    size_t len = candidates[0].size();
    int space = 1;
    int nCols = width / (len + space);  // Calculate how many columns can fit in each row
    if (nCols < 1) nCols = 1;

    int nRows = (candidates.size() + nCols - 1) / nCols; // 向上取整得到行數

    for (int r = 0; r < nRows; ++r) {
        for (int c = 0; c < nCols; ++c) {
            int idx = r + c * nRows;  // 計算直行索引
            if (idx < candidates.size()) {
                fmt::print("{:<{}}", candidates[idx], static_cast<int>(len + space));
            } else {
                fmt::print("{:<{}}", "", static_cast<int>(len + space)); // 空白補齊
            }
        }
        fmt::print("\n");
    }
}
