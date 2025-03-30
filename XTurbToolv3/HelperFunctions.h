#pragma once

#include <string> // For std::string, std::wstring
#include <vector> // For std::vector

// Helper function to convert wstring to string (UTF-8)
std::string wstring_to_string(const std::wstring& wstr);

// Helper function to convert double to string with fixed precision
std::string to_string(double value);

// Helper function to parse comma-separated values into a vector of doubles
std::vector<double> parseCommaSeparatedDoubles(const std::wstring& text);

// Helper function to parse comma-separated values into a vector of wstrings
std::vector<std::wstring> parseCommaSeparatedWStrings(const std::wstring& text);