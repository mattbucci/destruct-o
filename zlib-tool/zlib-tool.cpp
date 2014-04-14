#include <iostream>
#include <fstream>

#include "lodepng.h"

// Simple file operations
bool load_file(std::vector<unsigned char>& data, const std::string& filename)
{
    std::streampos size;
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open())
    {
        size = file.tellg();
        file.seekg(0, std::ios::beg);
        data.resize(size);
        file.read((char *) data.data(), size);
        file.close();
        return true;
    }
    return false;
}

bool save_file(std::vector<unsigned char>& data, const std::string& filename)
{
    std::ofstream file(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    if(file.is_open())
    {
        file.write((char *) data.data(), data.size());
        file.close();
        return true;
    }
    return false;
}

// Simple tool to compress or decompress a file in zlib format
int main (int argc, char** argv)
{
    // Check for the proper number of parameters
    if(argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " c/x source destination" << std::endl;
        return 1;
    }
    
    // Get the path to the file
    std::string source = argv[2];
    std::string destination = argv[3];
    
    // Load the contents of the file
    std::vector<unsigned char> data, data_result;
    std::cout << "--> Loading file: " << source << std::endl;
    if(!load_file(data, source))
    {
        std::cerr << "Unable to load file: " << source << std::endl;
        return 1;
    }
    std::cout << "--> " << source << " loaded (" << data.size() << " bytes)" << std::endl;
    
    // If we are decompressing
    if(argv[1][0] == 'x')
    {
        // Decompress the data
        std::cout << "--> Decompressing Data" << std::endl;
        lodepng::decompress(data_result, data);
    }
    
    // If we are compressing
    else if(argv[1][0] == 'c')
    {
        // Compress the data
        std::cout << "--> Compressing Data" << std::endl;
        lodepng::compress(data_result, data);
    }
    
    // Save the data in a file
    std::cout << "--> Writing file: " << destination << std::endl;
    if(!save_file(data_result, destination))
    {
        std::cerr << "Unable to write to file: " << destination << std::endl;
        return 1;
    }
    std::cout << "--> " << source << " wrote (" << data_result.size() << " bytes)" << std::endl;
    
    // Return success
    return 0;
}