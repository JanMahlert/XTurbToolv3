#include "HelperFunctions.h"
#include "header.h" // For WideCharToMultiByte

// Helper to convert wstring to string (UTF-8)
std::string wstring_to_string(const std::wstring& wstr) {
    if (wstr.empty()) return "";

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(size_needed - 1, 0);  // -1 to exclude null terminator
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size_needed, nullptr, nullptr);
    return result;
}

// Helper to convert double to string with fixed precision
std::string to_string(double value) {
    char buffer[32];
    snprintf(buffer, 32, "%.3f", value); // 3 decimal places for consistency
    return std::string(buffer);
}

// Helper to parse comma-separated values into a vector of doubles
std::vector<double> parseCommaSeparatedDoubles(const std::wstring& text) {
    std::vector<double> values;
    std::wstring wstr = text;
    size_t pos = 0;
    while ((pos = wstr.find(L',')) != std::wstring::npos) {
        std::wstring token = wstr.substr(0, pos);
        values.push_back(_wtof(token.c_str()));
        wstr.erase(0, pos + 1);
    }
    if (!wstr.empty()) {
        values.push_back(_wtof(wstr.c_str()));
    }
    return values;
}

// Helper to parse comma-separated values into a vector of wstrings
std::vector<std::wstring> parseCommaSeparatedWStrings(const std::wstring& text) {
    std::vector<std::wstring> values;
    std::wstring wstr = text;
    size_t pos = 0;
    while ((pos = wstr.find(L',')) != std::wstring::npos) {
        std::wstring token = wstr.substr(0, pos);
        values.push_back(token);
        wstr.erase(0, pos + 1);
    }
    if (!wstr.empty()) {
        values.push_back(wstr);
    }
    return values;
}