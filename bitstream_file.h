#include "dictionary_file.cpp"
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
using namespace std;

// BitStreamWriter Class

class BitStream_Writer final
{
private:
    // Internal initialization function for setting default values
    void internal_Init();

    // Helper function to allocate memory for the bit stream
    static uint8_t* alloc_Bytes(int bytes_Wanted, uint8_t* old_Ptr, int old_Size);

    // Increasing buffer for storing bits.
    uint8_t* data_Stream;

    // The current size of the heap-allocated stream buffer in bytes.
    int bytes_Allocated;

    // The amount of bytesAllocated that multiplies by when auto-resizing in the appendBit().
    int granularity_;

    // The current byte being written to that is from 0 to bytesAllocated - 1.
    int currentBytePos;

    // The bit position within the current byte to access next (0 to 7).
    int nextBitPosition;

    // The number of bits in use from the stream buffer not including byte-rounding padding.
    int num_BitsWritten;

public:
    // Deleted copy constructor and copy assignment operator
    BitStream_Writer(const BitStream_Writer&) = delete;
    BitStream_Writer& operator=(const BitStream_Writer&) = delete;

    // Default constructor
    BitStream_Writer();

    // Constructor with initial size and growth granularity
    explicit BitStream_Writer(int initial_SizeInBits, int growth_Granularity = 2);

    // Function to allocate memory for the bit stream
    void allocate_(int bits_Wanted);

    // Function to set the growth granularity of the bit stream
    void set_Granularity(int growth_Granularity);

    // Function to release the memory allocated for the bit stream and return the pointer
    uint8_t* release_();

    // Function to append a single bit to the bit stream
    void append_Bit(int bit_);

    // Function to append a specified number of bits from a 64-bit unsigned integer to the bit stream
    void append_BitsU64(uint64_t number, int bit_Count);

    // Function to convert the bit stream to a string of '0's and '1's
    string to_BitString() const;

    // Function to append a bit string to the bit stream
    void append_BitString(const string& bit_Str);

    // Function to get the byte count of the bit stream
    int get_ByteCount() const;

    // Function to get the total bit count of the bit stream
    int get_BitCount() const;

    // Function to get a const pointer to the underlying byte array of the bit stream
    const uint8_t* get_BitStream() const;

    // Destructor to free allocated memory
    ~BitStream_Writer();
};

// BitStreamReader class

class BitStream_Reader final
{
private:
    // The pointer to an external bit stream.
    const uint8_t* data_Stream;

    // The size of the stream in bytes.
    const int size_InBytes;

    // The size of the stream in bits.
    const int size_InBits;

    // The current byte that is being read in the stream.
    int currentBytePos;

    // The bit position that is within the current byte to access next (0 to 7).
    int nextBitPosition;

    // The total number of bits that are read from the stream so far.
    int num_BitsRead;

public:
    // Deleted copy constructor and copy assignment operator
    BitStream_Reader(const BitStream_Reader&) = delete;
    BitStream_Reader& operator=(const BitStream_Reader&) = delete;

    // Constructor to create a BitStream_Reader from a BitStream_Writer
    BitStream_Reader(const BitStream_Writer& bitStream_Writer);

    // Constructor to create a BitStream_Reader from a byte array with specified size and bit count
    BitStream_Reader(const uint8_t* bit_Stream, int byte_Count, int bit_Count);

    // Function to check if the end of the bit stream is reached
    bool is_EndOfStream() const;

    // Function to read the next bit from the bit stream
    bool read_NextBit(int& bit_Out);

    // Function to read a specified number of bits (up to 64) from the bit stream and return as a 64-bit unsigned integer
    uint64_t read_BitsU64(int bit_Count);

    // Function to reset the internal state of the BitStream_Reader
    void reset_();
};

