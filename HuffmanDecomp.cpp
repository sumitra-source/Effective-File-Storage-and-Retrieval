#include<iostream> 
#include<unordered_map> 
#include <chrono>
#include <filesystem>
// Added for file size information
namespace fs = std::filesystem;  
using namespace std;

// Function to convert decimal to binary string
string decimal_to_binary(int decimal) {
    string binary_str = "";
    for(int i = 7; i >= 0; i--) {
        // Bitwise shift and OR operation to check each bit
        if (decimal >> i & 1) {
            binary_str += '1';
        } else {
            binary_str += '0';
        }
    }
    return binary_str;
}

int main(int argc, char *argv[]) {
    // Checking command line arguments
    if (argc != 2) {
        cout << "Invalid command line argument.\n";
        return 1;
    }

    // Start measuring time
    auto start_time = chrono::high_resolution_clock::now();

    // Open input file in binary mode
    FILE *input_file = fopen(argv[1], "rb");  
    int num_of_unique_chars = 0;

    if (input_file == NULL) {
        cout << "Error in opening input file.\n";
        return 2;
    }

    // Get input and output file names
    string input_filename = argv[1];
    string output_filename = input_filename.substr(0, input_filename.find("compressed")) + "decompressed";
    char buffer[1];

    // Reading file extension length and extension itself
    fread(buffer, 1, 1, input_file);
    int extension_length = buffer[0] - '0';  

    while (extension_length--) {
        // Reading each character of the file extension
        fread(buffer, 1, 1, input_file);
        // Adding it to the output file name  
        output_filename += buffer[0]; 
    }

    // Opening the output file for writing
    FILE *output_file = fopen(output_filename.c_str(), "w");  

    if (output_file == NULL) {
        cout << "Error creating output file.\n";
        return 3;
    }

    // HashMap to store character mappings for decoding
    unordered_map<string, char> decode_map; 
    // Current code being read
    string current_code = ""; 
    // Flag to keep track of the unique characters section in the compressed file 
    int flag = 0; 

    // Reading and filling hashmap to decode later
    while (fread(buffer, 1, 1, input_file)) {
        if (buffer[0] != '\0') {
            flag = 0;
            // Appending character to current code
            current_code += buffer[0]; 
        } else {
            num_of_unique_chars++;
            flag++;
            if (flag == 2) break;
            // Storing character and its code in the hashmap
            decode_map[current_code.substr(1, current_code.length() - 1)] = current_code[0]; 
            current_code = "";
        }
    }

    // Reading and storing the padding as well as reading extra null chars 
    fread(buffer, 1, 1, input_file);
    int padding = buffer[0] - '0';
    fread(buffer, 1, 1, input_file);  // This is null
    fread(buffer, 1, 1, input_file);  // This is null

    // The actual binary code begins from here
    int decimal;
    current_code = "";

    while (fread(buffer, 1, 1, input_file)) {
        // Getting decimal (ASCII) equivalent of character
        decimal = buffer[0];
        // Appending binary equivalent to current_code
        current_code += decimal_to_binary(decimal);
    }

    // Ignoring the padding and converting binary code back to the original text file
    int start;    
    if (padding == 0) {
        current_code = "0" + current_code;
        start = 1;
    } else {
        start = padding;
    }

    for (int i = 1; current_code[start] != '\0'; i++) {
        if (decode_map.find(current_code.substr(start, i)) != decode_map.end()) {
            fwrite(&(decode_map[current_code.substr(start, i)]), 1, 1, output_file);
            start = start + i;
            i = 0;
        }
    }

    cout << "File has been decompressed successfully.\n";

    // Close file streams
    fclose(input_file);
    fclose(output_file);

    // Stop measuring time
    auto end_time = chrono::high_resolution_clock::now();
    // Calculate the duration
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    // File size information
    fs::path compressed_file_path(argv[1]);
    fs::path decompressed_file_path(output_filename); 
    cout << "Compressed file size: " << fs::file_size(compressed_file_path) << " bytes\n";
    cout << "Decompressed file size: " << fs::file_size(decompressed_file_path) << " bytes\n";
    
    // Print the time taken for decompression
    cout << "Time taken for decompression: " << duration.count() << " milliseconds\n";

    return 0;
}
