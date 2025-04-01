#pragma once
#include <string>

class OutputData;

class OutputFileParser {
public:
    OutputFileParser(const std::wstring& filePath); // Declaration only
    virtual ~OutputFileParser() = default;
    virtual bool parse(OutputData& data) = 0;

protected:
    std::wstring filePath;
    virtual bool processLine(const std::wstring& line, OutputData& data) = 0;
};