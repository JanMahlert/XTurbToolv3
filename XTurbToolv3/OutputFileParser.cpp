#include "OutputFileParser.h"
#include "OutputData.h"
#include <fstream>
#include <sstream>
#include <algorithm>

OutputFileParser::OutputFileParser(const std::wstring& filePath) : filePath(filePath) {}

bool OutputFileParser::parse(OutputData& data) {
    data.clear();
    std::wifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::wstring line;
    while (std::getline(file, line)) {
        // Remove leading/trailing whitespace
        line.erase(0, line.find_first_not_of(L" \t"));
        line.erase(line.find_last_not_of(L" \t") + 1);
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