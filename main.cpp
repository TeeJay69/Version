#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
#include <cstdlib>

class ConfigFileNames{
    private:
        std::string fileName;
        int identifier;
    public:
        ConfigFileNames(std::string a, int id) : fileName(a), identifier(id) {} //Constructor

        std::string getFileName() const{
            return fileName;
        }

        int getIdentifier() const{
            return identifier;
        }

        std::string setFileName(std::string newName) {
            fileName = newName;
        }
        
        int setIdentifier(int id){
            identifier = id;
        }
};

int main(){ 

    if(std::filesystem::exists(".versiontool")){
        std::cout << "ConfigFile is present" << std::endl;

        std::ifstream versiontoolFile(".versiontool");

        int i = 0;
        std::string filearray[10];
        while(!versiontoolFile.eof()){
            std::string storedFile;
            
            versiontoolFile >> storedFile;
            filearray[i] = storedFile;
            std::cout << "storedFile: " << storedFile << std::endl;

            ++i;
        }

        for(int i = 0; i < sizeof(filearray) / sizeof(filearray[0]); i++){
            std::cout << "Array Item: " << filearray[i] << std::endl;
        }
    }
    else{
        // std::cout << "ConfigFile is not present";
        // std::ofstream versiontoolFile(".versiontool"); // create file
        
        // if(std::system("cmd /c code .versiontool") != 0){
        //     std::system("cmd /c notepad .versiontool");
        // }

        // std::vector<char> currentdirArray[100];
        std::string currentdirArray[500];

        int i = 1;
        for(const auto& entry : std::filesystem::directory_iterator(".")){
            if(entry.is_regular_file()){
                std::cout << "[" << i << "]" << "  " << entry << std::endl;
                std::string item = entry.path().generic_string();
                currentdirArray[i] = item;
                i++;
            }
            // currentdirArray[i].push_back(entry);
        }
        std::cout << "Array: " << currentdirArray[1] << std::endl << currentdirArray[2] << std::endl << currentdirArray[3] << std::endl;
        std::cout << "Select Files to be included in the Release: ";

        std::string userinput;
        std::cin >> userinput;
    }
    
    return 0;
    // std::this_thread::sleep_for(std::chrono::seconds(2));
}