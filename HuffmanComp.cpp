#include <iostream>
#include <cstdlib>
#include <string.h>
#include <math.h>
#include <unordered_map>
#include <chrono>
#include <filesystem>  // Added for file size information
using namespace std;

// Macros for efficient I/O operations
#define IO                        \
    ios_base::sync_with_stdio(0); \
    cin.tie(0);                   \
    cout.tie(0);

// Hash map to store characters and their corresponding Huffman codes
unordered_map<char, string> char_CodeMap;

// Structure to represent a node in the Huffman tree
struct HuffmanNode
{
    char character_;
    unsigned int frequency_;
    struct HuffmanNode *left_, *right_;
};
typedef struct HuffmanNode *NodePtr;

// Structure to represent a min-heap used during Huffman tree construction
struct MinHeap
{
    unsigned short size_, capacity_;
    NodePtr *array_;
};
typedef struct MinHeap *HeapPtr;

// Function to create a new leaf node
NodePtr newLeafNode(char ch, int freq)
{
    NodePtr tmp_ = (NodePtr)malloc(sizeof(struct HuffmanNode));
    tmp_->character_ = ch;
    tmp_->frequency_ = freq;
    tmp_->left_ = NULL;
    tmp_->right_ = NULL;
    return tmp_;
}

// Function to create a new internal node
NodePtr newInternalNode(int f)
{
    NodePtr tmp_ = (NodePtr)malloc(sizeof(struct HuffmanNode));
    tmp_->character_ = 0;
    tmp_->frequency_ = f;
    tmp_->left_ = NULL;
    tmp_->right_ = NULL;
    return tmp_;
}

// Function to swap two nodes in the min-heap
void swapNodes(NodePtr *a, int i, int j)
{
    NodePtr tmp_ = a[i];
    a[i] = a[j];
    a[j] = tmp_;
}

// Function to heapify the min-heap
void heapify(HeapPtr heap_)
{
    int i, j, k;
    unsigned short n = heap_->size_;
    NodePtr *array_ = heap_->array_;

    for (i = n / 2; i > 0; i--)
    {
        j = (2 * i + 1 <= n) ? (array_[2 * i]->frequency_ < array_[2 * i + 1]->frequency_ ? 2 * i : 2 * i + 1) : 2 * i;

        if (array_[j]->frequency_ < array_[i]->frequency_)
        {
            swapNodes(array_, i, j);
            while (j <= n / 2 && (array_[j]->frequency_ > array_[2 * j]->frequency_ || (2 * j + 1 <= n ? array_[j]->frequency_ > array_[2 * j + 1]->frequency_ : 0)))
            {
                k = (2 * j + 1 <= n) ? (array_[2 * j]->frequency_ < array_[2 * j + 1]->frequency_ ? 2 * j : 2 * j + 1) : 2 * j;
                swapNodes(array_, k, j);
                j = k;
            }
        }
    }
}

// Function to extract the minimum element from the min-heap
NodePtr extractMin(HeapPtr heap_)
{
    NodePtr top_ = heap_->array_[1];
    heap_->array_[1] = heap_->array_[heap_->size_--];
    heapify(heap_);
    return top_;
}

// Function to insert a new node into the min-heap
void insertNode(HeapPtr heap_, NodePtr left_Child, NodePtr right_Child, int freq)
{
    NodePtr tmp_ = (NodePtr)malloc(sizeof(struct HuffmanNode));
    tmp_->character_ = '\0';
    tmp_->frequency_ = freq;
    tmp_->left_ = left_Child;
    tmp_->right_ = right_Child;
    heap_->array_[++heap_->size_] = tmp_;
    heapify(heap_);
}

// Function to build the Huffman tree
void buildHuffmanTree(HeapPtr heap_)
{
    while (heap_->size_ > 1)
    {
        NodePtr left_Child = extractMin(heap_);
        NodePtr right_Child = extractMin(heap_);
        insertNode(heap_, left_Child, right_Child, (left_Child->frequency_ + right_Child->frequency_));
    }
}

// Function to generate Huffman codes for each character in the tree
void generateCodes(NodePtr root, string str)
{
    if (!root->left_ && !root->right_)
    {
        char_CodeMap[root->character_] = str;
    }
    else
    {
        string left_Code = str + "0";
        string right_Code = str + "1";
        generateCodes(root->left_, left_Code);
        generateCodes(root->right_, right_Code);
    }
}

// Function to perform a preorder traversal of the Huffman tree
void preorderTraversal(NodePtr root)
{
    if (root != NULL)
    {
        cout << root->frequency_ << "\t";
        preorderTraversal(root->left_);
        preorderTraversal(root->right_);
    }
}

int main(int argc, char *argv[])
{
    // Setting up for efficient I/O
    IO
    // Starting measuring time
    auto start_time = std::chrono::high_resolution_clock::now();
    // Checking command line arguments
    if (argc != 2)
    {
        cout << "Invalid command line argument. Usage: ./filename.exe input_file\n";
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL)
    {
        cout << "Error opening the input file\n";
        return 2;
    }

    string inp = argv[1];
    string filetype_ = inp.substr(inp.find(".") + 1);
    cout << "File type: " << filetype_ << "\n";

    string outp = inp.substr(0, inp.find(".")) + "-compressed.bin";
    FILE *output_file = fopen(outp.c_str(), "wb");

    if (output_file == NULL)
    {
        cout << "Error creating the output file\n";
        return 3;
    }

    string file_contents = "";
    char buffer[1];
    unsigned short num_of_unique_chars = 0;

    // Writing file type
    int ftsize = filetype_.length();
    char number = ftsize + '0';
    fwrite(&number, 1, 1, output_file);
    for (int i = 0; i < ftsize; i++)
    {
        buffer[0] = filetype_[i];
        fwrite(buffer, 1, 1, output_file);
    }

    // Holding the frequency of each unique character
    int f__size = 0, frequency_[256] = {0};
    while (fread(buffer, 1, 1, input_file))
    {
        if (!frequency_[buffer[0]])
            num_of_unique_chars++;
        frequency_[buffer[0]]++;
        f__size++;
    }
    fclose(input_file);

    FILE *input1_file = fopen(argv[1], "r");

    HeapPtr heap_ = (HeapPtr)malloc(sizeof(struct MinHeap));
    heap_->capacity_ = num_of_unique_chars + 1;
    heap_->size_ = 0;
    heap_->array_ = (NodePtr *)malloc(heap_->capacity_ * sizeof(struct HuffmanNode));

    for (int i = 0; i < 256; i++)
    {
        if (frequency_[i])
        {
            heap_->size_ += 1;
            heap_->array_[heap_->size_] = newLeafNode(i, frequency_[i]);
        }
    }

    heapify(heap_);
    buildHuffmanTree(heap_);
    generateCodes(heap_->array_[heap_->size_], "");

    unordered_map<char, string>::iterator x;

    while (fread(buffer, 1, 1, input1_file))
    {
        file_contents += char_CodeMap[buffer[0]];
    }

    int padding_ = 0;
    if (file_contents.length() % 8 != 0)
        padding_ = 8 - (file_contents.length() % 8);

    for (int i = 0; i < padding_; i++)
    {
        file_contents = "0" + file_contents;
    }

    string codes;
    unordered_map<char, string>::iterator itern;
    for (itern = char_CodeMap.begin(); itern != char_CodeMap.end(); itern++)
    {
        codes = "";
        codes += (itern->first + itern->second);
        fwrite(codes.c_str(), codes.length() + 1, 1, output_file);
    }

    char null = '\0';
    char pad = padding_ + '0';
    fwrite(&null, 1, 1, output_file);
    fwrite(&pad, 1, 1, output_file);
    fwrite(&null, 1, 1, output_file);
    fwrite(&null, 1, 1, output_file);

    char tmp_bin[9];
    int decimal, w;
    for (int j = 0; file_contents[j]; j = j + 8)
    {
        strncpy(tmp_bin, &file_contents[j], 8);
        decimal = 0;
        int i = 7;
        while (i > -1)
        {
            w = tmp_bin[i] == '1' ? 1 : 0;
            decimal += ((int)pow(2, 7 - i)) * w;
            i--;
        }
        buffer[0] = decimal;
        fwrite(buffer, 1, 1, output_file);
    }

    cout << "Compressed File successfully\n";

    free(heap_->array_);
    free(heap_);
    fclose(input1_file);
    fclose(output_file);

    // Getting the size of the original file
    std::filesystem::path originalFilePath(argv[1]);
    uintmax_t originalFileSize = std::filesystem::file_size(originalFilePath);
    cout << "Original file size: " << originalFileSize << " bytes\n";

    // Getting the size of the compressed file
    std::filesystem::path compressedFilePath(outp.c_str());
    uintmax_t compressedFileSize = std::filesystem::file_size(compressedFilePath);
    cout << "Compressed file size: " << compressedFileSize << " bytes\n";

    // Calculating compression ratio
    double compressionRatio = static_cast<double>(originalFileSize) / static_cast<double>(compressedFileSize);
    cout << "Compression ratio: " << compressionRatio << "\n";

    // Stopping measuring time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Printing the time taken for compression
    cout << "Time taken for compression: " << duration.count() << " milliseconds\n";

    return 0;
}
