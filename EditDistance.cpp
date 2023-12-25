#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <limits>
#include <chrono>

// Function to calculate the Levenshtein distance between two strings
size_t levenshteinDistance(const std::string& s1, const std::string& s2) {
    const size_t m = s1.size();
    const size_t n = s2.size();

    // Dynamic programming vectors for storing the distances
    std::vector<size_t> dpPrev(n + 1);
    std::vector<size_t> dpCurr(n + 1);

    // Initialization of the first row
    for (size_t i = 0; i <= n; ++i) {
        dpPrev[i] = i;
    }

    // Calculate the Levenshtein distance
    for (size_t i = 0; i < m; ++i) {
        dpCurr[0] = i + 1;

        for (size_t j = 0; j < n; ++j) {
            // Calculate the cost of substitution
            size_t cost = (s1[i] != s2[j]);
            dpCurr[j + 1] = std::min({ dpCurr[j] + 1, dpPrev[j + 1] + 1, dpPrev[j] + cost });
        }

        // Swap the vectors to prepare for the next iteration
        std::swap(dpPrev, dpCurr);
    }

    // The Levenshtein distance is stored in the last cell of the calculated matrix
    return dpPrev[n];
}

// Comparison function for sorting files based on Levenshtein distance
bool compareByLevenshteinDistance(const std::pair<std::string, size_t>& a, const std::pair<std::string, size_t>& b) {
    return a.second < b.second;
}

int main() {
    const std::string directoryPath = "C:\\Users\\Dell\\Desktop\\data"; // Path to your directory
    std::string inputFilePath;

    // Get the path of the input file from the user
    std::cout << "Enter the path of the input file: ";
    std::getline(std::cin, inputFilePath);

    // Open the input file
    std::ifstream inputFileStream(inputFilePath);
    if (!inputFileStream.is_open()) {
        std::cerr << "Error: Unable to open the input file." << std::endl;
        return 1;
    }

    // Read the contents of the input file
    std::string inputContent((std::istreambuf_iterator<char>(inputFileStream)), {});
    inputFileStream.close();

    // Vector to store similar files and their Levenshtein distances
    std::vector<std::pair<std::string, size_t>> similarFiles;

    // Measure the start time of the file comparison process
    auto start = std::chrono::steady_clock::now();

    // Iterate through the files in the specified directory
    for (const auto& file : std::filesystem::directory_iterator(directoryPath)) {
        if (file.is_regular_file() && file.path().string() != inputFilePath) {
            // Open each file for comparison
            std::ifstream fileStream(file.path().string());
            if (fileStream) {
                // Read the contents of each file
                std::string fileContent((std::istreambuf_iterator<char>(fileStream)), {});
                fileStream.close();
                // Calculate the Levenshtein distance between input and current file
                size_t distance = levenshteinDistance(inputContent, fileContent);
                // Add the file and its distance to the vector
                similarFiles.emplace_back(file.path().string(), distance);
            } else {
                std::cerr << "Error: Could not open file '" << file.path() << "'" << std::endl;
            }
        }
    }

    // Measure the end time of the file comparison process
    auto end = std::chrono::steady_clock::now();
    // Calculate the duration of the file comparison process
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Sort the similarFiles vector based on Levenshtein distance
    std::sort(similarFiles.begin(), similarFiles.end(), compareByLevenshteinDistance);

    // Get the number of similar files to display from the user
    int numFilesToDisplay;
    std::cout << "Enter the number of similar files to display: ";
    std::cin >> numFilesToDisplay;

    // Display the top similar files with their Levenshtein distances
    std::cout << "Top " << numFilesToDisplay << " similar files to " << inputFilePath<< ": based on Levenshtein distance:\n";
 for (size_t i = 0; i < numFilesToDisplay && i < similarFiles.size(); ++i) 
  {
     std::cout << "File: " << similarFiles[i].first << " - Levenshtein Distance: " << similarFiles[i].second << '\n'; 
     }
     // Display the execution time
std::cout << "Execution time: " << duration.count() << " milliseconds" << std::endl;

return 0;
}