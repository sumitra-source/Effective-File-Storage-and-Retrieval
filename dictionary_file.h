#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cassert>
using namespace std;

// LZW Dictionary Helper

// Constants for special values and limits in the LZW dictionary
constexpr int Nil_            = -1;                       // Indicates a nil value
constexpr int Max_DictBits    = 12;                       // Maximum number of bits for LZW dictionary
constexpr int Start_Bits      = 9;                        // Initial number of bits for LZW dictionary
constexpr int First_Code      = (1 << (Start_Bits - 1));   // First code in the LZW dictionary (256)
constexpr int Max_DictEntries = (1 << Max_DictBits);       // Maximum number of entries in the LZW dictionary (4096)

// Class Diction_ary (LZW Dictionary)

class Diction_ary final
{
public:
    // Structure representing an entry in the LZW dictionary
    struct Entryy
    {
        int codes;   // Code associated with the entry
        int val;     // Value associated with the entry
    };

    // Public members of the LZW dictionary class
    int size_;              // Current size of the LZW dictionary
    Entryy entries_[Max_DictEntries]; // Array representing LZW dictionary entries

    // Constructor for the LZW dictionary class
    Diction_ary();

    // Function to find the index of a given code and value in the LZW dictionary
    int find_Index(int codes, int val) const;

    // Function to add a new entry to the LZW dictionary
    bool addition_(int codes, int val);

    // Function to flush the LZW dictionary and adjust the code bits width if needed
    bool flush_function(int & code_BitsWidth);
};

