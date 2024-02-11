#include <iostream>
#include <string>

int main() {
    std::string input;

    // Loop indefinitely until Enter is pressed
    while (true) {
        // Read a line from cin
        std::getline(std::cin, input);

        // Check if the input is empty (only Enter was pressed)
        if (input.empty()) {
            std::cout << "Enter was pressed!" << std::endl;
            break; // Exit the loop
        } else {
            // Print the entered line
            std::cout << "You entered: " << input << std::endl;
        }
    }

    return 0;
}