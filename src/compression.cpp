#include "pch.h"


char* uncompressRawFile(RawFile& rawFile)
{
    char* InflatDat = new char[rawFile.len];
    unsigned long InflatDatSz = rawFile.len;

    int nResult = uncompress((Bytef*)InflatDat, &InflatDatSz, (Bytef*)rawFile.buffer, rawFile.compressedLen);
    if (nResult == Z_OK)
    {
        return InflatDat;
    }
    else
    {
        memset(InflatDat, 0, rawFile.len);
        return InflatDat;
    }
}

std::vector<uint8_t> uncompressData(const char* compressedData, size_t compressedSize)
{
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = compressedSize;
    strm.next_in = reinterpret_cast<Bytef*>( const_cast<char*>( compressedData ) );

    if (inflateInit(&strm) != Z_OK)
    {
        std::cerr << "Failed to initialize zlib for decompression." << std::endl;
    }

    std::vector<uint8_t> uncompressedData;
    size_t             bufferSize = 1024;
    std::vector<uint8_t> buffer(bufferSize);
    int ret;

    do
    {
        strm.avail_out = bufferSize;
        strm.next_out = buffer.data();

        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR)
        {
            inflateEnd(&strm);
            throw std::runtime_error("Failed to decompress data");
        }

        size_t bytesDecompressed = bufferSize - strm.avail_out;
        uncompressedData.insert(uncompressedData.end(), buffer.begin(), buffer.begin() + bytesDecompressed);

    }
    while (ret != Z_STREAM_END); // Continue until all data has been decompressed

    inflateEnd(&strm);

    return uncompressedData;
}

char* compressCharData(const char* inputData, size_t inputSize, size_t& compressedSize)
{
    uLongf maxCompressedSize = compressBound(inputSize);
    Bytef* compressedData = new Bytef[maxCompressedSize];

    int result = compress2(compressedData, &maxCompressedSize, reinterpret_cast<const Bytef*>( inputData ), inputSize,
                           Z_BEST_COMPRESSION);

    if (result != Z_OK)
    {
        delete[] compressedData;
        throw std::runtime_error("Data compression failed");
    }

    compressedSize = maxCompressedSize;
    char* outputData = new char[compressedSize];
    std::memcpy(outputData, compressedData, compressedSize);

    delete[] compressedData;
    return outputData;
}

char* compressData(const std::vector<uint8_t>& inputData, size_t& compressedSize)
{
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    if (deflateInit(&strm, Z_BEST_COMPRESSION) != Z_OK)
    {
        throw std::runtime_error("Failed to initialize zlib");
    }

    strm.avail_in = inputData.size();                      // Input size
    strm.next_in = const_cast<Bytef*>( inputData.data() ); // Input data

    size_t bufferSize = inputData.size() + ( inputData.size() / 10 ) + 12; // Reserve enough space for output
    char* compressedData = new char[bufferSize];

    strm.avail_out = bufferSize;                                // Output size
    strm.next_out = reinterpret_cast<Bytef*>( compressedData ); // Output buffer

    if (deflate(&strm, Z_FINISH) < 0)
    {
        deflateEnd(&strm);
        delete[] compressedData;
        std::cerr << "Failed to compress data." << std::endl;
    }

    deflateEnd(&strm);

    compressedSize = bufferSize - strm.avail_out; 
    return compressedData;
}

std::vector<uint8_t> readFileToByteVector(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Failed to open file");
    }

    auto byteVec = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    byteVec.push_back(0);
    return byteVec;
}
