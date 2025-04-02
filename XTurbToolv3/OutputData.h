#pragma once
#include <string>
#include <vector>
#include <map>

class OutputData {
public:
    std::map<std::wstring, std::wstring> singleValues;
    struct Table {
        std::vector<std::wstring> headers;
        std::vector<std::vector<double>> rows;
    };
    std::vector<Table> tables;
    std::wstring headerText; // New field to store header text

    void clear() {
        singleValues.clear();
        tables.clear();
        headerText.clear();
    }

    OutputData() = default;
    OutputData(const OutputData& other) : singleValues(other.singleValues), tables(other.tables), headerText(other.headerText) {}
    OutputData& operator=(const OutputData& other) {
        if (this != &other) {
            singleValues = other.singleValues;
            tables = other.tables;
            headerText = other.headerText;
        }
        return *this;
    }
};