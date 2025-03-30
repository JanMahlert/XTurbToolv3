#pragma once

#include "header.h" // For Windows headers
#include <string> // For std::wstring

// Simple logger class for error handling
class Logger {
public:
    static void logError(const std::wstring& message); // Log an error message
};