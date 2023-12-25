#include "lzwComp.h"

namespace lzwComp
{

// LZW Compression

// Function to perform LZW encoding
void lzwEncode(const uint8_t * un_compressed, int uncompressed_SizeBytes,
                uint8_t ** compressed, int * compressed_SizeBytes, int * compressed_SizeBits)
{
    // Check for null pointers in input and output
    if (un_compressed == nullptr || compressed == nullptr)
    {
        cout << "lzwEncode() - Null data pointers" << endl;
        return;
    }

    // Check for valid sizes in input and output
    if (uncompressed_SizeBytes <= 0 || compressed_SizeBytes == nullptr || compressed_SizeBits == nullptr)
    {
        cout << "lzwEncode() - Input or output sizes are incorrect" << endl;
        return;
    }

    // LZW encoding variables
    int codes = Nil_;
    int code_BitsWidth = Start_Bits;
    Diction_ary diction_ary;

    // Output bit stream will allocate memory as needed for encoded data
    BitStream_Writer bit_Stream;

    // LZW encoding algorithm
    for (; uncompressed_SizeBytes > 0; --uncompressed_SizeBytes, ++un_compressed)
    {
        const int val = *un_compressed;
        const int index_val = diction_ary.find_Index(codes, val);

        if (index_val != Nil_)
        {
            codes = index_val;
            continue;
        }

        // Writing dictionary code using minimum bitwidth
        bit_Stream.append_BitsU64(codes, code_BitsWidth);

        // Flushing dictionary when full to restart sequences
        if (!diction_ary.flush_function(code_BitsWidth))
        {
            // There is still some space for this sequence
            diction_ary.addition_(codes, val);
        }
        codes = val;
    }

    if (codes != Nil_)
    {
        bit_Stream.append_BitsU64(codes, code_BitsWidth);
    }

    // Pass ownership of compressed data buffer to user pointer
    *compressed_SizeBytes = bit_Stream.get_ByteCount();
    *compressed_SizeBits  = bit_Stream.get_BitCount();
    *compressed          = bit_Stream.release_();
}

// LZW Decompression

// Function to output a single byte during decompression
static bool output_Byte(int codes, uint8_t *& output, int output_SizeBytes, int & bytes_DecodedSoFar)
{
    // Check for buffer overflow
    if (bytes_DecodedSoFar >= output_SizeBytes)
    {
        cout << "The decoder output buffer is very small" << endl;
        return false;
    }

    // Ensure the code is within the valid range [0, 255]
    assert(codes >= 0 && codes < 256);
    *output++ = static_cast<uint8_t>(codes);
    ++bytes_DecodedSoFar;
    return true;
}

// Function to output an entire sequence during decompression
static bool output_Sequence(const Diction_ary & dict, int codes,
                           uint8_t *& output, int output_SizeBytes,
                           int & bytes_DecodedSoFar, int & first_Byte)
{
    // Temporarily store the sequence in reverse order
    int i = 0;
    uint8_t seq_uence[Max_DictEntries];

    // Reconstruct the sequence from the dictionary
    do
    {
        assert(i < Max_DictEntries - 1 && codes >= 0);
        seq_uence[i++] = dict.entries_[codes].val;
        codes = dict.entries_[codes].codes;
    } while (codes >= 0);

    // Set the first byte of the sequence
    first_Byte = seq_uence[--i];

    // Output the sequence in correct order
    for (; i >= 0; --i)
    {
        if (!output_Byte(seq_uence[i], output, output_SizeBytes, bytes_DecodedSoFar))
        {
            return false;
        }
    }
    return true;
}

// Function to perform LZW decoding
int lzwDecode(const uint8_t * compressed, const int compressed_SizeBytes, const int compressed_SizeBits,
              uint8_t * un_compressed, const int uncompressed_SizeBytes)
{
    // Check for null pointers in input and output
    if (compressed == nullptr || un_compressed == nullptr)
    {
        cout << "lzwDecode() - There is a null data pointer" << endl;
        return 0;
    }

    // Check for valid sizes in input and output
    if (compressed_SizeBytes <= 0 || compressed_SizeBits <= 0 || uncompressed_SizeBytes <= 0)
    {
        cout << "lzwDecode() - There are bad input or output sizes" << endl;
        return 0;
    }

    // LZW decoding variables
    int codes          = Nil_;
    int prev_Code      = Nil_;
    int first_Byte     = 0;
    int bytes_Decoded  = 0;
    int code_BitsWidth = Start_Bits;

    // Reconstructing dictionary based on the bit stream codes.
    Diction_ary diction_ary;
    BitStream_Reader bit_Stream(compressed, compressed_SizeBytes, compressed_SizeBits);

    // Checking to avoid overflow of the user buffer.
    while (!bit_Stream.is_EndOfStream())
    {
        assert(code_BitsWidth <= Max_DictBits);
        codes = static_cast<int>(bit_Stream.read_BitsU64(code_BitsWidth));

        if (prev_Code == Nil_)
        {
            if (!output_Byte(codes, un_compressed,
                 uncompressed_SizeBytes, bytes_Decoded))
            {
                break;
            }
            first_Byte = codes;
            prev_Code  = codes;
            continue;
        }

        if (codes >= diction_ary.size_)
        {
            if (!output_Sequence(diction_ary, prev_Code, un_compressed,
                 uncompressed_SizeBytes, bytes_Decoded, first_Byte))
            {
                break;
            }
            if (!output_Byte(first_Byte, un_compressed,
                 uncompressed_SizeBytes, bytes_Decoded))
            {
                break;
            }
        }
        else
        {
            if (!output_Sequence(diction_ary, codes, un_compressed,
                 uncompressed_SizeBytes, bytes_Decoded, first_Byte))
            {
                break;
            }
        }

        diction_ary.addition_(prev_Code, first_Byte);

        // Flush the dictionary if needed or update previous code
        if (diction_ary.flush_function(code_BitsWidth))
        {
            prev_Code = Nil_;
        }
        else
        {
            prev_Code = codes;
        }
    }

    return bytes_Decoded;
}

}  // namespace lzwComp
