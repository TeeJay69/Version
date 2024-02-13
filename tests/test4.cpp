#include <iostream>

int main(int argc, char* argv[]) {
    // argc stores the number of arguments passed
    std::cout << "Number of arguments: " << argc << std::endl;

    // Loop through each argument and print its value
    for (int i = 0; i < argc; ++i) {
        std::cout << "Argument " << i << ": " << argv[i] << std::endl;
    }

    return 0;
}