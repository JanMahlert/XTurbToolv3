#pragma once
#include "header.h" 
#include <string> 

// Log error messages. Use this throughout the app to log errors
class Logger {
public:
    static void logError(const std::wstring& message);
};