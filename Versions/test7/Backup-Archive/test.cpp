#include <iostream>
#include <filesystem>

int main() {
    // Get the current directory
    std::filesystem::path currentDir = std::filesystem::current_path();

    // Extract the last directory component
    std::filesystem::path lastDir = currentDir.filename();

    // Print the last directory component
    std::cout << "Last directory: " << lastDir << std::endl;

    return 0;
}