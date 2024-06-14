#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

int readfile() {
    std::ifstream file("lt_pic.jpg", std::ios::binary);
    if (!file) {
        std::cerr << "File opening failed" << std::endl;
        return 1;
    }

    // Read the file into a vector
    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Print the content in hexadecimal format
    for (size_t i = 0; i < buffer.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i] << " ";
        if ((i + 1) % 16 == 0) {
            std::cout << std::endl;
        }
    }

    return 0;
}

