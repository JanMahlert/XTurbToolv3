#pragma once
#include "OutputFileParser.h"
#include "OutputData.h"

class BEMTOutputParser : public OutputFileParser {
public:
    BEMTOutputParser(const std::wstring& filePath);
    bool parse(OutputData& data) override;

private:
    bool processLine(const std::wstring& line, OutputData& data) override;
    bool inTableSection;
    OutputData::Table currentTable;
};