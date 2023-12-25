#include "bitstream_file.cpp"
#include<iostream>
#include <cstdint>
#include <cstdlib>
#include <string>

using namespace std;

namespace lzwComp
{
    // Forward declarations for LZW compression and decompression functions

    // LZW Encoding
    void lzwEncode(const uint8_t * uncompressed, int uncompressed_SizeBytes, uint8_t ** compressed, int * compressed_SizeBytes, int * compressed_SizeBits);

    // LZW Decoding
    int lzwDecode(const uint8_t * compressed, int compressed_SizeBytes, int compressed_SizeBits, uint8_t * uncompressed, int uncompressed_SizeBytes);

}  // namespace lzwComp
