#include "Logger.h"

// Log an error message to the debug output
void Logger::logError(const std::wstring& message) {
    OutputDebugStringW((message + L"\n").c_str()); // Output to Visual Studio debug window
    // In a production app, you could also write to a file here
}