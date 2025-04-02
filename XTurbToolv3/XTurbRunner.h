#pragma once
#include <windows.h>
#include <string>

// This class runs the XTurb Executable.
class XTurbRunner {
public:
    XTurbRunner(const std::wstring& exePath);
    bool run(const std::wstring& inputFilePath);
    std::wstring getExePath() const;

private:
    std::wstring exePath;
};