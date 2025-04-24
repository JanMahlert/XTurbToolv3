#include "BEMTOutputParser.h"
#include <fstream>
#include <sstream>
#include "Logger.h"

BEMTOutputParser::BEMTOutputParser(const std::wstring& filePath)
    : OutputFileParser(filePath), inTableSection(false) {
}

bool BEMTOutputParser::processLine(const std::wstring& line, OutputData& data) {
    std::wstring trimmedLine = line;
    trimmedLine.erase(0, trimmedLine.find_first_not_of(L" \t"));
    trimmedLine.erase(trimmedLine.find_last_not_of(L" \t") + 1);

    //Log error, add empty line to header text, and mark as processed
    if (trimmedLine.empty()) {
        Logger::logError(L"Empty line skipped");
        data.headerText += line + L"\r\n"; // Add to header text
        return true;
    }

    //Check for table delimiter, handle current table, and update header text
    if (trimmedLine.find(L"---") != std::wstring::npos) {
        if (inTableSection && !currentTable.headers.empty() && !currentTable.rows.empty()) {
            data.tables.push_back(currentTable);
            Logger::logError(L"Table parsed with " + std::to_wstring(currentTable.rows.size()) + L" rows");
        }
        currentTable.rows.clear();
        inTableSection = true;
        data.headerText += line + L"\r\n"; // Add to header text
        return true;
    }

    if (inTableSection) {
        //Log entry into table section for current line
        Logger::logError(L"Reached table section for line: " + trimmedLine);
        if (!currentTable.headers.empty()) {
            //Process a row of data in the table
            Logger::logError(L"Processing table row: " + trimmedLine);
            std::vector<double> row;
            size_t pos = 0, prev = 0;
            while ((pos = trimmedLine.find(L" ", prev)) != std::wstring::npos) {
                //Extract each token separated by spaces and validate it
                std::wstring token = trimmedLine.substr(prev, pos - prev);
                if (!token.empty()) {
                    Logger::logError(L"Token found: " + token);
                    if (token == L"Infinity") {
                        row.push_back(std::numeric_limits<double>::infinity());
                    }
                    else {
                        try {
                            //Convert valid tokens into double values
                            row.push_back(std::stod(token));
                        }
                        catch (...) {
                            //Handle invalid tokens and replace them with NaN
                            Logger::logError(L"Invalid token '" + token + L"' replaced with NaN");
                            row.push_back(std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                }
                prev = pos + 1;
            }
            //Handle the last token separately
            std::wstring lastToken = trimmedLine.substr(prev);
            if (!lastToken.empty()) {
                Logger::logError(L"Token found: " + lastToken);
                if (lastToken == L"Infinity") {
                    row.push_back(std::numeric_limits<double>::infinity());
                }
                else {
                    try {
                        row.push_back(std::stod(lastToken));
                    }
                    catch (...) {
                        Logger::logError(L"Invalid token '" + lastToken + L"' replaced with NaN");
                        row.push_back(std::numeric_limits<double>::quiet_NaN());
                    }
                }
            }
            //Verify the row matches the expected number of columns
            Logger::logError(L"Parsed " + std::to_wstring(row.size()) + L" values, expected " + std::to_wstring(currentTable.headers.size()));
            if (!row.empty() && row.size() == currentTable.headers.size()) {
                //Add the valid row to the table
                currentTable.rows.push_back(row);
                Logger::logError(L"Data row added: " + trimmedLine);
            }
            else if (!row.empty()) {
                //Log a mismatch between row size and expected columns
                Logger::logError(L"Row size mismatch: got " + std::to_wstring(row.size()) + L", expected " + std::to_wstring(currentTable.headers.size()));
            }
            else {
                //No valid data parsed from row
                Logger::logError(L"No valid tokens parsed from row");
            }
        }
        else {
            //Handle case where table headers are not set
            Logger::logError(L"Headers empty in table section");
        }
    }
    else {
        //Parse non-table lines for headers or key-value pairs
        std::wstringstream ss(trimmedLine);
        std::wstring firstToken;
        ss >> firstToken;
        if (firstToken == L"r/R" || firstToken == L"Number") {
            //Set table headers if a valid table start token is detected
            currentTable.headers.clear();
            std::wstringstream headerSS(trimmedLine);
            std::wstring header;
            while (headerSS >> header) {
                currentTable.headers.push_back(header);
            }
            Logger::logError(L"Table headers set: " + trimmedLine + L" (" + std::to_wstring(currentTable.headers.size()) + L" columns)");
            // If this is the "r/R" table, stop adding to headerText
            if (firstToken != L"r/R") {
                data.headerText += line + L"\r\n";
            }
        }
        //Handle key-value pairs for single values
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
            //Append non-table lines to header text
            data.headerText += line + L"\r\n"; // Add to header text
        }
    }
    return true;
}

//Attempt to open the file and handle errors if the file cannot be opened
bool BEMTOutputParser::parse(OutputData& data) {
    std::wifstream file(filePath);
    if (!file.is_open()) {
        Logger::logError(L"Failed to open file: " + filePath);
        return false;
    }

    //Read each line from the file, log it, and process it, stopping on errors.
    std::wstring line;
    while (std::getline(file, line)) {
        Logger::logError(L"Reading line: " + line);
        if (!processLine(line, data)) {
            file.close();
            return false;
        }
    }
    //Check and finalize the current table, adding it to the data structure if valid.
    if (inTableSection && !currentTable.headers.empty() && !currentTable.rows.empty()) {
        data.tables.push_back(currentTable);
        Logger::logError(L"Final table parsed with " + std::to_wstring(currentTable.rows.size()) + L" rows");
    }

    //Close the file, log the parsing summary, and signal success.
    file.close();
    Logger::logError(L"Parsing completed for " + filePath + L" with " + std::to_wstring(data.tables.size()) + L" tables");
    return true;
}