#include "BEMTOutputParser.h"
#include "Logger.h"

int main() {
    BEMTOutputParser parser(L"C:\\path\\to\\your\\output1.dat");
    OutputData data;
    if (parser.parse(data)) {
        for (const auto& [key, value] : data.singleValues) {
            Logger::logError(key + L": " + value);
        }
        for (const auto& table : data.tables) {
            Logger::logError(L"Table:");
            std::wstring headers;
            for (const auto& header : table.headers) {
                headers += header + L"\t";
            }
            Logger::logError(headers);
            for (const auto& row : table.rows) {
                std::wstring rowStr;
                for (double val : row) {
                    rowStr += std::to_wstring(val) + L"\t";
                }
                Logger::logError(rowStr);
            }
        }
    } else {
        Logger::logError(L"Failed to parse file");
    }
    return 0; // No pause needed; check Output window in VS
}