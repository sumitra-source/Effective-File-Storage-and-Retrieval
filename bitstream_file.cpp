#include "bitstream_file.h"

// Function to calculate the next power of two greater than or equal to the given number
static int CalculateNextPowerOfTwo(int number)
{
    --number;
    for (size_t i = 1; i < sizeof(number) * 8; i <<= 1)
    {
        number = number | number >> i;
    }
    return ++number;
}

// BitStream_Writer class implementation

// Constructor to initialize a BitStream_Writer with a default size of 8192 bits (1024 bytes)
BitStream_Writer::BitStream_Writer()
{
    internal_Init();
    allocate_(8192);
}

// Constructor to initialize a BitStream_Writer with a specified initial size and growth granularity
BitStream_Writer::BitStream_Writer(const int initial_SizeInBits, const int growth_Granularity)
{
    internal_Init();
    set_Granularity(growth_Granularity);
    allocate_(initial_SizeInBits);
}

// Destructor to free allocated memory
BitStream_Writer::~BitStream_Writer()
{
    if (data_Stream != nullptr)
    {
        free(data_Stream);
    }
}

// Internal initialization function for BitStream_Writer
void BitStream_Writer::internal_Init()
{
    data_Stream = nullptr;
    bytes_Allocated = 0;
    granularity_ = 2;
    currentBytePos = 0;
    nextBitPosition = 0;
    num_BitsWritten = 0;
}

// Function to allocate memory for the bit stream with a specified number of bits
void BitStream_Writer::allocate_(int bits_Wanted)
{
    // Ensure at least a byte is required
    if (bits_Wanted <= 0)
    {
        bits_Wanted = 8;
    }

    // Round upwards if needed
    if ((bits_Wanted % 8) != 0)
    {
        bits_Wanted = CalculateNextPowerOfTwo(bits_Wanted);
    }

    // Check if the required count is already allocated
    const int size_InBytes = bits_Wanted / 8;
    if (size_InBytes <= bytes_Allocated)
    {
        return;
    }

    // Allocate memory for the bit stream
    data_Stream = alloc_Bytes(size_InBytes, data_Stream, bytes_Allocated);
    bytes_Allocated = size_InBytes;
}

// Function to append a single bit to the bit stream
void BitStream_Writer::append_Bit(const int bit_)
{
    const uint32_t masks = uint32_t(1) << nextBitPosition;
    data_Stream[currentBytePos] = (data_Stream[currentBytePos] & ~masks) | (-bit_ & masks);
    ++num_BitsWritten;

    if (++nextBitPosition == 8)
    {
        nextBitPosition = 0;
        if (++currentBytePos == bytes_Allocated)
        {
            allocate_(bytes_Allocated * granularity_ * 8);
        }
    }
}

// Function to append a specified number of bits from a 64-bit unsigned integer to the bit stream
void BitStream_Writer::append_BitsU64(const uint64_t number, const int bit_Count)
{
    assert(bit_Count <= 64);
    for (int bc = 0; bc < bit_Count; ++bc)
    {
        const uint64_t masks = uint64_t(1) << bc;
        const int bit_ = !!(number & masks);
        append_Bit(bit_);
    }
}

// Function to append a bit string to the bit stream
void BitStream_Writer::append_BitString(const string& bit_Str)
{
    for (size_t i = 0; i < bit_Str.length(); ++i)
    {
        append_Bit(bit_Str[i] == '0' ? 0 : 1);
    }
}

// Function to convert the bit stream to a string of '0's and '1's
string BitStream_Writer::to_BitString() const
{
    string bit_String;

    int used_Bytes = num_BitsWritten / 8;
    int left_overs = num_BitsWritten % 8;
    if (left_overs != 0)
    {
        ++used_Bytes;
    }
    assert(used_Bytes <= bytes_Allocated);

    for (int i = 0; i < used_Bytes; ++i)
    {
        const int n_Bits = (left_overs == 0) ? 8 : (i == used_Bytes - 1) ? left_overs : 8;
        for (int j = 0; j < n_Bits; ++j)
        {
            bit_String += (data_Stream[i] & (1 << j) ? '1' : '0');
        }
    }

    return bit_String;
}

// Function to release the memory allocated for the bit stream and return the pointer
uint8_t* BitStream_Writer::release_()
{
    uint8_t* old_Ptr = data_Stream;
    internal_Init();
    return old_Ptr;
}

// Function to set the growth granularity of the bit stream
void BitStream_Writer::set_Granularity(const int growth_Granularity)
{
    granularity_ = (growth_Granularity >= 2) ? growth_Granularity : 2;
}

// Function to get the byte count of the bit stream
int BitStream_Writer::get_ByteCount() const
{
    int used_Bytes = num_BitsWritten / 8;
    int left_overs = num_BitsWritten % 8;
    if (left_overs != 0)
    {
        ++used_Bytes;
    }
    assert(used_Bytes <= bytes_Allocated);
    return used_Bytes;
}

// Function to get the total bit count of the bit stream
int BitStream_Writer::get_BitCount() const
{
    return num_BitsWritten;
}

// Function to get a const pointer to the underlying byte array of the bit stream
const uint8_t* BitStream_Writer::get_BitStream() const
{
    return data_Stream;
}

// Function to allocate memory for the bit stream (helper function)
uint8_t* BitStream_Writer::alloc_Bytes(const int bytes_Wanted, uint8_t* old_Ptr, const int old_Size)
{
    // Allocate new memory
    uint8_t* new_Memory = static_cast<uint8_t*>(malloc(bytes_Wanted));
    memset(new_Memory, 0, bytes_Wanted);

    // Copy old data if it exists
    if (old_Ptr != nullptr)
    {
        memcpy(new_Memory, old_Ptr, old_Size);
        free(old_Ptr);
    }

    return new_Memory;
}

// BitStream_Reader class implementation

// Constructor to create a BitStream_Reader from a BitStream_Writer
BitStream_Reader::BitStream_Reader(const BitStream_Writer& bit_StreamWriter)
    : data_Stream(bit_StreamWriter.get_BitStream())
    , size_InBytes(bit_StreamWriter.get_ByteCount())
    , size_InBits(bit_StreamWriter.get_BitCount())
{
    reset_();
}

// Constructor to create a BitStream_Reader from a byte array with specified size and bit count
BitStream_Reader::BitStream_Reader(const uint8_t* bit_Stream, const int byte_Count, const int bit_Count)
    : data_Stream(bit_Stream)
    , size_InBytes(byte_Count)
    , size_InBits(bit_Count)
{
    reset_();
}

// Function to read the next bit from the bit stream
bool BitStream_Reader::read_NextBit(int& bit_Out)
{
    if (num_BitsRead >= size_InBits)
    {
        return false;
    }

    const uint32_t masks = uint32_t(1) << nextBitPosition;
    bit_Out = !!(data_Stream[currentBytePos] & masks);
    ++num_BitsRead;

    if (++nextBitPosition == 8)
    {
        nextBitPosition = 0;
        ++currentBytePos;
    }
    return true;
}

// Function to read a specified number of bits (up to 64) from the bit stream and return as a 64-bit unsigned integer
uint64_t BitStream_Reader::read_BitsU64(const int bit_Count)
{
    assert(bit_Count <= 64);

    uint64_t number = 0;
    for (int bc = 0; bc < bit_Count; ++bc)
    {
        int bit_;
        if (!read_NextBit(bit_))
        {
            cout << "Difficult to read bits from stream" << endl;
            break;
        }

        const uint64_t masks = uint64_t(1) << bc;
        number = (number & ~masks) | (-bit_ & masks);
    }

    return number;
}

// Function to reset the internal state of the BitStream_Reader
void BitStream_Reader::reset_()
{
    currentBytePos = 0;
    nextBitPosition = 0;
    num_BitsRead = 0;
}

// Function to check if the end of the bit stream is reached
bool BitStream_Reader::is_EndOfStream() const
{
    return num_BitsRead >= size_InBits;
}
