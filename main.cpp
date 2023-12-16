#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>

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
    std::string a = "Hello";
    std::cout << "This is a test " << a << "\n";
    
    if(std::filesystem::exists(".versiontool")){
        std::cout << "ConfigFile is present";

        std::ifstream fin(".versiontool");

        ConfigFileNames *objects[50];
        int i = 0;
        while(!fin.eof()){
            std::string storedFile;
            
            fin >> storedFile;
            
            std::cout << "storedFile: " << storedFile << std::endl;
            
            objects[i] = new ConfigFileNames(storedFile, i);

            std::cout << "array object: " << objects[i] << std::endl;
            i++;
        }
        std::cout << "Full array" << objects << std::endl;
    }
    else{
        std::cout << "ConfigFile is not present";
    }
    return 0;
    // std::this_thread::sleep_for(std::chrono::seconds(2));
}