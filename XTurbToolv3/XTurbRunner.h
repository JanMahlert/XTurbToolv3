#pragma once

#include <windows.h>
#include <string>

class XTurbRunner {
public:
    XTurbRunner(const std::wstring& exePath);
    bool run(const std::wstring& inputFilePath);

private:
    std::wstring exePath;
};