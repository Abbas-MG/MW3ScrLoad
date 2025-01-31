#pragma once

char* uncompressRawFile(RawFile& rawFile);

std::vector<uint8_t> uncompressData(const char* compressedData, size_t compressedSize);

char* compressCharData(const char* inputData, size_t inputSize, size_t& compressedSize);

char* compressData(const std::vector<uint8_t>& inputData, size_t& compressedSize);

std::vector<uint8_t> readFileToByteVector(const std::string& filePath);
