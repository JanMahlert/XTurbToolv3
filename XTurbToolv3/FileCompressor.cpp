#include "FileCompressor.h"
#include <zlib.h>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

FileCompressor::FileCompressor(const std::string& directory) : directory_(directory) {
    if (!directory_.empty() && directory_.back() != '/' && directory_.back() != '\\') {
        directory_ += '/';
    }
}

bool FileCompressor::compressFiles() {
    bool allSuccess = true;
    for (const auto& filename : filenames_) {
        std::string inputPath = directory_ + filename;
        std::string outputPath = inputPath + ".gz";

        if (!fs::exists(inputPath)) {
            std::cerr << "Error: File " << inputPath << " does not exist.\n";
            allSuccess = false;
            continue;
        }

        if (!compressFile(inputPath, outputPath)) {
            std::cerr << "Error: Failed to compress " << inputPath << ".\n";
            allSuccess = false;
        }
        else {
            std::cout << "Successfully compressed " << inputPath << " to " << outputPath << ".\n";
        }
    }
    return allSuccess;
}

bool FileCompressor::compressFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error: Cannot open input file " << inputFile << ".\n";
        return false;
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(inFile)),
        std::istreambuf_iterator<char>());
    inFile.close();

    z_stream zs{};
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;

    if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        std::cerr << "Error: zlib initialization failed for " << inputFile << ".\n";
        return false;
    }

    zs.avail_in = buffer.size();
    zs.next_in = reinterpret_cast<unsigned char*>(buffer.data());

    std::vector<unsigned char> output(compressBound(buffer.size()));
    zs.avail_out = output.size();
    zs.next_out = output.data();

    if (deflate(&zs, Z_FINISH) != Z_STREAM_END) {
        std::cerr << "Error: Compression failed for " << inputFile << ".\n";
        deflateEnd(&zs);
        return false;
    }

    size_t compressedSize = zs.total_out;
    deflateEnd(&zs);

    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Cannot open output file " << outputFile << ".\n";
        return false;
    }

    outFile.write(reinterpret_cast<char*>(output.data()), compressedSize);
    outFile.close();

    return true;
}