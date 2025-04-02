#pragma once
#include <string>
#include <vector>
#include <map>

// This class is storing the output data from XTurb. The BEMTOutputParser class is using this to store the data.
class OutputData {
public:
    std::map<std::wstring, std::wstring> singleValues;
    struct Table {
        std::vector<std::wstring> headers;
        std::vector<std::vector<double>> rows;
    };
    std::vector<Table> tables;
    std::wstring headerText;

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