#pragma once

#include <windows.h>
#include <string>

class XTurbRunner {
public:
    XTurbRunner(const std::wstring& exePath);
    bool run(const std::wstring& inputFilePath);
    std::wstring getExePath() const; // Add getter method

private:
    std::wstring exePath;
};