#include "dictionary_file.h"

// Class Dictionary

// Constructor for the dictionary initializes the entries with default values
Diction_ary::Diction_ary()
{
    // Set the initial size of the dictionary
    size_ = First_Code;

    // Initialize entries with default codes and values
    for (int i = 0; i < size_; ++i)
    {
        entries_[i].codes  = Nil_;
        entries_[i].val = i;
    }
}

// Function to find the index of a given code and value in the dictionary
int Diction_ary::find_Index(const int codes, const int val) const
{
    // If the code is Nil_, return the value directly
    if (codes == Nil_)
    {
        return val;
    }

    // Linear search implementation to find the index of the given code and value
    for (int i = 0; i < size_; ++i)
    {
        if (entries_[i].codes == codes && entries_[i].val == val)
        {
            return i;
        }
    }

    // If not found, return Nil_
    return Nil_;
}

// Function to add a new entry to the dictionary
bool Diction_ary::addition_(const int codes, const int val)
{
    // Check if the dictionary is full
    if (size_ == Max_DictEntries)
    {
        // Output an overflow message and return false
        cout << "The dictionary is overflowed" << endl;
        return false;
    }

    // Add a new entry to the dictionary
    entries_[size_].codes  = codes;
    entries_[size_].val = val;
    ++size_;
    return true;
}

// Function to flush the dictionary and adjust the code bits width if needed
bool Diction_ary::flush_function(int & code_BitsWidth)
{
    // Check if the dictionary is full based on the current code bits width
    if (size_ == (1 << code_BitsWidth))
    {
        // Increase the code bits width
        ++code_BitsWidth;

        // Check if the code bits width exceeds the maximum allowed
        if (code_BitsWidth > Max_DictBits)
        {
            // Clear the dictionary except for the initial 256 byte entries
            code_BitsWidth = Start_Bits;
            size_ = First_Code;
            
            // Return true to indicate a full flush of the dictionary
            return true;
        }
    }

    // Return false if the dictionary is not full
    return false;
}

