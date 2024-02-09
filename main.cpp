#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cstddef>
#include <vector>

#ifdef DEBUG
#define Debug_Mode 1
#else
#define Debug_Mode 0
#endif
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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

        void setFileName(std::string newName){
            fileName = newName;
        }
        
        void setIdentifier(int id){
            identifier = id;
        }
};

int main(){ 

    if(std::filesystem::exists(".versiontool")){
        std::cout << "ConfigFile is present" << std::endl;

        std::ifstream versiontoolFile(".versiontool");

        std::string line;
        std::vector<std::string> fileVector;
        std::string compileConfig;
        while(std::getline(versiontoolFile, line)){
            if(line == "Files included in release:"){
                int i = 0;
                
                while(std::getline(versiontoolFile, line)){
                    int delimiterHit = 0;

                    if(line == "----------"){ // delimiter
                        delimiterHit = 1;
                        // Read compile parameters
                        while(std::getline(versiontoolFile, line)){
                            if(line == "Compile command:"){
                                while(std::getline(versiontoolFile, line)){
                                    if(line.find("rmdir ") == std::string::npos && line.find("rd ") == std::string::npos && line.find("-o ") == std::string::npos && line.find("del ") == std::string::npos && line.find("rm ") == std::string::npos && line.find("move ") == std::string::npos && line.find("mv ") == std::string::npos){
                                        compileConfig = line;
                                        break;
                                    }
                                    else{
                                        // std::cerr << "Error: Compile Command contains forbidden words!";
                                        throw std::runtime_error("Error: Compile Command contains forbidden words!");
                                    }
                                }
                            }
                            else{
                                throw std::runtime_error("Error compile command not found");
                            }
                        }
                    }
                    else if(delimiterHit == 0){
                        // load filenames into vector
                        fileVector.push_back(line);
                        // std::string includedFile = line;
                        // std::cout << "included Files: " << includedFile << std::endl;
                    }
                }
            }
            else{
                throw std::runtime_error("Error Reading File. Unexpected Line");
            }
        }

        std::cout << ANSI_COLOR_GREEN << "Items included in the Release:" << ANSI_COLOR_RESET << std::endl;
        // for(int i = 0; i < sizeof(filearray) / sizeof(filearray[0]); i++){
        for(int i = 0; i < fileVector.size(); i++){
            std::cout << "[" << i + 1 << "]" << "  " << fileVector[i] << std::endl;
        }
        std::cout << std::endl << "Do You Want to Proceed? (yes=1): ";
        int userResponse;
        std::cin >> userResponse;
        if(userResponse == 1){
            std::cout << ANSI_COLOR_GREEN << "Version Name: " << ANSI_COLOR_RESET << std::endl;
            std::string versionName;
            std::cin >> versionName;
            std::string versionNamePath = ".\\Versions\\" + versionName;
            if(!std::filesystem::exists(versionNamePath)){
                std::cout << "Directory does not exist already";
            }
        }
        else{
            throw std::runtime_error("User Declined!");
        }
    }
    else{
        // std::cout << "ConfigFile is not present";
        // std::ofstream versiontoolFile(".versiontool"); // create file
        
        // if(std::system("cmd /c code .versiontool") != 0){
        //     std::system("cmd /c notepad .versiontool");
        // }

        // std::vector<char> currentdirArray[100];
        std::string currentdirArray[1000];

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