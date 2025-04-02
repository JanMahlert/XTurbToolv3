#pragma once

#include <string>
#include <vector> 

// convert wstring to string (UTF-8)
std::string wstring_to_string(const std::wstring& wstr);

// convert double to string
std::string to_string(double value);

// parse comma-separated values into a vector
std::vector<double> parseCommaSeparatedDoubles(const std::wstring& text);

// parse comma-separated values into a vector of wstrings
std::vector<std::wstring> parseCommaSeparatedWStrings(const std::wstring& text);