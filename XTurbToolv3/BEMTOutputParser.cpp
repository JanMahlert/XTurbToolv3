#include "BEMTOutputParser.h"
#include <fstream>
#include <sstream>
#include <string>
#include "Logger.h"

BEMTOutputParser::BEMTOutputParser(const std::wstring& filePath)
    : OutputFileParser(filePath), inTableSection(false) {
}

bool BEMTOutputParser::processLine(const std::wstring& line, OutputData& data) {
    std::wstring trimmedLine = line;
    trimmedLine.erase(0, trimmedLine.find_first_not_of(L" \t"));
    trimmedLine.erase(trimmedLine.find_last_not_of(L" \t") + 1);

    if (trimmedLine.empty()) return true;

    if (trimmedLine.find(L"---") != std::wstring::npos) {
        if (inTableSection && !currentTable.headers.empty() && !currentTable.rows.empty()) {
            data.tables.push_back(currentTable);
            Logger::logError(L"Table parsed with " + std::to_wstring(currentTable.rows.size()) + L" rows");
        }
        currentTable = OutputData::Table();
        inTableSection = true;
        Logger::logError(L"Entering table section");
        return true;
    }

    if (inTableSection) {
        if (!currentTable.headers.empty()) {
            std::vector<double> row;
            std::wstring token;
            size_t start = 0, end = 0;
            while (start < trimmedLine.length()) {
                while (start < trimmedLine.length() && (trimmedLine[start] == L' ' || trimmedLine[start] == L'\t')) start++;
                if (start >= trimmedLine.length()) break;
                end = trimmedLine.find_first_of(L" \t", start);
                if (end == std::wstring::npos) end = trimmedLine.length();
                token = trimmedLine.substr(start, end - start);
                if (!token.empty()) {
                    try {
                        if (token == L"Infinity") {
                            row.push_back(std::numeric_limits<double>::infinity());
                            Logger::logError(L"Parsed token: Infinity");
                        }
                        else {
                            double value = std::stod(token);
                            row.push_back(value);
                            Logger::logError(L"Parsed token: " + token);
                        }
                    }
                    catch (...) {
                        Logger::logError(L"Invalid number in row: " + trimmedLine + L", token: " + token);
                        return false;
                    }
                }
                start = end + 1;
            }
            if (!row.empty() && row.size() == currentTable.headers.size()) {
                currentTable.rows.push_back(row);
                Logger::logError(L"Data row added: " + trimmedLine);
            }
            else if (!row.empty()) {
                Logger::logError(L"Row size mismatch: " + trimmedLine + L", expected " + std::to_wstring(currentTable.headers.size()) + L", got " + std::to_wstring(row.size()));
            }
            else {
                Logger::logError(L"No tokens parsed from row: " + trimmedLine);
            }
        }
    }
    else {
        std::wstringstream ss(trimmedLine);
        std::wstring firstToken;
        ss >> firstToken;
        if (firstToken == L"r/R" || firstToken == L"Number") {
            currentTable.headers.clear();
            std::wstringstream headerSS(trimmedLine);
            std::wstring header;
            while (headerSS >> header) {
                currentTable.headers.push_back(header);
            }
            Logger::logError(L"Table headers set: " + trimmedLine);
        }
        else {
            size_t eqPos = trimmedLine.find(L"=");
            if (eqPos != std::wstring::npos) {
                std::wstring key = trimmedLine.substr(0, eqPos);
                std::wstring value = trimmedLine.substr(eqPos + 1);
                key.erase(key.find_last_not_of(L" \t") + 1);
                value.erase(0, value.find_first_not_of(L" \t"));
                data.singleValues[key] = value;
                Logger::logError(L"Single value: " + key + L" = " + value);
            }
        }
    }
    return true;
}

bool BEMTOutputParser::parse(OutputData& data) {
    std::wifstream file(filePath);
    if (!file.is_open()) {
        Logger::logError(L"Failed to open file: " + filePath);
        return false;
    }

    std::wstring line;
    while (std::getline(file, line)) {
        if (!processLine(line, data)) {
            file.close();
            return false;
        }
    }

    if (inTableSection && !currentTable.headers.empty() && !currentTable.rows.empty()) {
        data.tables.push_back(currentTable);
        Logger::logError(L"Final table parsed with " + std::to_wstring(currentTable.rows.size()) + L" rows");
    }

    file.close();
    Logger::logError(L"Parsing completed for " + filePath + L" with " + std::to_wstring(data.tables.size()) + L" tables");
    return true;
}