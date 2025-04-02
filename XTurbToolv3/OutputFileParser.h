#pragma once
#include <string>

class OutputData;

// This class is the base class to parse output data from XTurb
class OutputFileParser {
public:
    OutputFileParser(const std::wstring& filePath);
    virtual ~OutputFileParser() = default;
    virtual bool parse(OutputData& data) = 0;

protected:
    std::wstring filePath;
    virtual bool processLine(const std::wstring& line, OutputData& data) = 0;
};