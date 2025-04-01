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

    void clear() {
        singleValues.clear();
        tables.clear();
    }

    OutputData() = default;
    OutputData(const OutputData& other) : singleValues(other.singleValues), tables(other.tables) {}
    OutputData& operator=(const OutputData& other) {
        if (this != &other) {
            singleValues = other.singleValues;
            tables = other.tables;
        }
        return *this;
    }
};