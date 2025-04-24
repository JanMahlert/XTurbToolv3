#pragma once

#include <string>
#include <vector>

class FileCompressor {
public:
    explicit FileCompressor(const std::string& directory);
    bool compressFiles();

private:
    std::string directory_;
    const std::vector<std::string> filenames_ = {
        "XTurb_Output.dat",
        "XTurb_Output1.dat",
        "XTurb_Output2.dat",
        "XTurb_Output3.dat"
    };
    bool compressFile(const std::string& inputFile, const std::string& outputFile);
};