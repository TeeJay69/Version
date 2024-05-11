#include <iostream>
#include <regex>
#include <string>

std::string extractOutputName(const std::string& compileCommand) {
    std::smatch match;
    // Regex to match `-o` followed by optional spaces and capturing file name/path
    std::regex pattern(R"(-o\s*([^\s]+))");
    // Regex for Visual C++ style `/out:` followed by optional spaces and capturing file name/path
    std::regex patternVC(R"(/out:([^\s]+))");

    // Try to match Unix-like pattern
    if (std::regex_search(compileCommand, match, pattern) && match.size() > 1) {
        return match[1];
    }
    // Try to match Visual C++ pattern
    else if (std::regex_search(compileCommand, match, patternVC) && match.size() > 1) {
        return match[1];
    }

    // Return an empty string if no match is found
    return "";
}

int main() {
    std::string command = "g++ -o myprogram.exe source.cpp";
    std::string outputName = extractOutputName(command);
    if (!outputName.empty()) {
        std::cout << "Output executable name: " << outputName << std::endl;
    } else {
        std::cout << "No output name found in the command." << std::endl;
    }
    return 0;
}
