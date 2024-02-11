#include <iostream>
#include <filesystem>
#include <string>
#include <iostream>


void removeSubstrs(std::string& s, std::string& p) { 
  std::string::size_type n = p.length();
  for (std::string::size_type i = s.find(p);
      i != std::string::npos;
      i = s.find(p))
      s.erase(i, n);
}

int main() {
    // Get the current directory
    std::filesystem::path currentDir = std::filesystem::current_path();

    // Extract the last directory component
    std::filesystem::path lastDir = currentDir.filename();

    // Print the last directory component
    std::cout << "Last directory: " << lastDir << std::endl;

    std::filesystem::path item = "C:\\Data\\TW\\Software\\Coding\\versiontool\\testdir\\copythis";
    std::filesystem::path base = "C:\\Data\\TW\\Software\\Coding\\versiontool";
    std::filesystem::path relativeResult = std::filesystem::relative(item, base);
    std::cout << "Path exists: " << std::filesystem::exists(item) << std::endl;
    std::cout << "weakly_canonical relativeResult: " << std::filesystem::weakly_canonical(relativeResult) << std::endl;
    std::cout << "relativeResult: " << relativeResult << std::endl;
    std::cout << "filename(): " << item.filename() << std::endl;
    std::cout << "parent_path: " << item.parent_path() << std::endl;
    std::cout << "parent_path.filename(): " << item.parent_path().filename() << std::endl;
    
    std::string itemString = item.string();
    std::string baseString = base.string() + "\\";
    removeSubstrs(itemString, baseString);
    std::cout << itemString << std::endl;

    return 0;
}