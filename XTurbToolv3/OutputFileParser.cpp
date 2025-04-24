#include "OutputFileParser.h"
#include "OutputData.h"
#include <fstream>
#include <sstream>
#include <algorithm>

OutputFileParser::OutputFileParser(const std::wstring& filePath) : filePath(filePath) {}

// Reads a file, processes each line after trimming whitespace, and populates data. Returns false if any line fails to process.
bool OutputFileParser::parse(OutputData& data) {
    data.clear();
    std::wifstream file(filePath);
    // Checks if the file failed to open and returns false if so.
    if (!file.is_open()) {
        return false;
    }

    std::wstring line;
    // Reads each line, trims whitespace, and processes it, returning false if processing fails.
    while (std::getline(file, line)) {
        // Remove leading/trailing whitespace
        line.erase(0, line.find_first_not_of(L" \t"));
        line.erase(line.find_last_not_of(L" \t") + 1);
        // Processes the line if it's not empty, and returns false if processing fails.
        if (!line.empty()) {
            if (!processLine(line, data)) {
                file.close();
                return false;
            }
        }
    }
    file.close();
    return true;
}