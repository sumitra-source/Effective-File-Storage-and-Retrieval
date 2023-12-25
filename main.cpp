#include "lzwComp.cpp"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>

using namespace std;
using namespace lzwComp;

// Function to perform LZW compression and decompression on sample data
static void LZWComp(const uint8_t * sample_Data, const int sample_Size)
{
    // Record the start time for compression
    auto start_time = chrono::high_resolution_clock::now();

    // Variables for storing compressed data and sizes
    int compressed_SizeBytes = 0;
    int compressed_SizeBits = 0;
    uint8_t *compressed_Data = nullptr;

    // Vector to store uncompressed data
    vector<uint8_t> uncompressed_Buffer(sample_Size, 0);

    // Perform LZW compression
    lzwEncode(sample_Data, sample_Size, &compressed_Data, &compressed_SizeBytes, &compressed_SizeBits);

    // Record the end time for compression
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    // Display compression results
    cout << "LZW Compression Time: " << duration.count() << " milliseconds\n";
    cout << "The size in bytes before LZW compression is= " << sample_Size << "\n";
    cout << "The size in bytes after LZW compression is= " << compressed_SizeBytes << "\n\n";

    // Calculate compression ratio
    double compressionRatio = static_cast<double>(sample_Size) / static_cast<double>(compressed_SizeBytes);
    cout << "Compression Ratio: " << compressionRatio << "\n";

    // Write compressed data to a file
    ofstream out("EncodedFile.txt");
    for (int i = 0; i < compressed_SizeBytes; i++)
    {
        out << compressed_Data[i];
    }
    out.close();

    // Record the start time for decompression
    start_time = chrono::high_resolution_clock::now();

    // Perform LZW decompression
    const int uncompressed_Size = lzwDecode(compressed_Data, compressed_SizeBytes, compressed_SizeBits, uncompressed_Buffer.data(), uncompressed_Buffer.size());

    // Write decompressed data to a file
    ofstream out2("DecodedFile.txt");
    for (int i = 0; i < uncompressed_Buffer.size(); i++)
    {
        out2 << uncompressed_Buffer[i];
    }
    out2.close();

    // Record the end time for decompression
    end_time = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    // Display decompression results
    cout << "LZW Decompression Time: " << duration.count() << " milliseconds\n";
    cout << "The size of LZW decompressed file in bytes is= " << uncompressed_Buffer.size() << "\n\n";

    // Calculate decompression ratio
    double decompressionRatio = static_cast<double>(uncompressed_Buffer.size()) / static_cast<double>(sample_Size);
    cout << "Decompression Ratio: " << decompressionRatio << "\n";

    // Check if the decompressed size matches the original size
    bool correct_execution = true;
    if (uncompressed_Size != sample_Size)
    {
        cout << "There is an error in LZW compression. The sizes do not match\n";
        correct_execution = false;
    }

    // Compare data for correctness
    if (memcmp(uncompressed_Buffer.data(), sample_Data, sample_Size) != 0)
    {
        cout << "Files are different. There is corrupted data.\n";
        correct_execution = false;
    }

    // Display final result based on correctness
    if (correct_execution)
    {
        cout << "There is successful LZW execution!\n";
    }

    // Free allocated memory for compressed data
    free(compressed_Data);
}

// Main function
int main()
{
    string strng, file_name;

    // Input file name for compression
    cout << "Please, enter the name of the file without extension for compression: ";
    cin >> file_name;

    // Read data from the file
    ifstream in(file_name + ".txt");
    if (in)
    {
        char ch = in.get();
        while (!in.eof())
        {
            cout << "Please wait..." << endl;
            strng += ch;
            ch = in.get();
        }
        in.close();
    }

    // Display the total bytes read from the file
    cout << "The total bytes read from the file is= " << strng.size() << endl;

    // Convert string data to a vector of uint8_t
    vector<uint8_t> my_Vector(strng.begin(), strng.end());
    static const uint8_t *str_0 = &my_Vector[0];

    // Call the LZWComp function for compression and decompression
    LZWComp(str_0, strng.size());

    return 0;
}
