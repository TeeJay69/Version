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
#include <stdexcept>

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
        std::string compileConfig; // for later
        while(std::getline(versiontoolFile, line)){
            if(line == "Files and directories included in version:"){
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
        std::cout << std::endl << "Do You Want to Proceed? (yes=1, no=2): ";
        std::string userResponse;

        // Filter input
        int proceedQuestion = 0;
        while(proceedQuestion != 1){
            // std::cin.clear();
            // std::cin.ignore(SIZE_MAX, '\n');
            std::cin >> userResponse;
            if(userResponse == "1" || userResponse == "2" && userResponse.size() == 1){
                proceedQuestion = 1;
            }
            else{
                std::cerr << "Invalid Input, Please try again: ";
            }
        }
        if(userResponse == "1"){
            int filesystemCheck = 0;
            // Get version name
            std::string versionName; // for later
            while(filesystemCheck != 1){
                std::cout << ANSI_COLOR_GREEN << "Version Name: " << ANSI_COLOR_RESET << std::endl;
                std::cin >> versionName;
                std::string versionNamePath = ".\\Versions\\" + versionName;
                
                if(!std::filesystem::exists(versionNamePath)){
                    // std::cout << "Directory does not exist yet" << std::endl;
                    filesystemCheck = 1;    
                }
                else{
                    std::string versionNameError = "Error: \"" + versionName + "\" already exists \n\n";
                    std::cerr << versionNameError;
                }
            }
            
            // Additional Debug version
            int debugQuestion = 0;
            int debugVersion; // for compile command
            while(debugQuestion != 1){
                std::cout << ANSI_COLOR_GREEN << "Would you like to include a debug version in the release? (no=1, yes=2): " << ANSI_COLOR_RESET << std::endl;
                std::string debugAnswer;
                std::cin >> debugAnswer;
                if(debugAnswer == "2"){
                    debugVersion = 1;
                    debugQuestion = 1;
                }
                else if(debugAnswer == "1"){
                    debugVersion = 0;
                    debugQuestion = 1;
                }
                else{
                    std::cerr << "Error: Invalid Answer, Please try again:" << std::endl;
                }
            }

            const std::filesystem::path currentPath = std::filesystem::current_path();
            const std::filesystem::path currentDir = currentPath.filename();

            // Compile the program
            if(debugVersion == 0){
                const std::string buildReleaseVersion = "cmd /c " + compileConfig + " -o " + currentDir.generic_string() + "-" + versionName + ".exe" + " -O3" + " -march=native";
                
                if(std::system((buildReleaseVersion).c_str()) == 0){
                    std::cout << ANSI_COLOR_GREEN << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                else{
                    throw std::runtime_error("Error: Program failed to compile");
                }
            }
            else if(debugVersion == 1){
                const std::string buildReleaseVersion = "cmd /c " + compileConfig + " -o " + currentDir.generic_string() + "-" + versionName + "_Release.exe" + " -DDebug" + " -O3" + " -march=native";
                const std::string buildDebugVersion = "cmd /c " + compileConfig + " -o " + currentDir.generic_string() + "-" + versionName + "_Debug.exe" " -DDebug" + " -O3" + " -march=native";
                
                if(std::system((buildReleaseVersion).c_str()) == 0 && std::system((buildDebugVersion).c_str()) == 0){
                    std::cout << ANSI_COLOR_GREEN << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                else{
                    throw std::runtime_error("Error: Program failed to compile");
                }
            }
            
            // Create version directory, copy to Backup-Archive and move compiled program to version directory
            const std::string versionDir = "Versions\\" + versionName;
            const std::string versionDirBackup = "Versions\\" + versionName + "\\" + "Backup-Archive";

            try{
                std::filesystem::create_directories(versionDir);
                std::filesystem::create_directories(versionDirBackup);
            }
            catch(std::filesystem::filesystem_error& createDirError){
                throw std::runtime_error(createDirError);
            }
            
            if(debugVersion == 0){
                const std::string exeName = currentDir.generic_string() + "-" + versionName + ".exe";
                try{
                    std::filesystem::copy(exeName, versionDirBackup + "\\" + exeName);

                    std::filesystem::rename(exeName, versionDir + "\\" + exeName);
                }
                catch(std::filesystem::filesystem_error& renameDirError){
                    throw std::runtime_error(renameDirError);
                }
            }
            else if(debugVersion == 1){
                const std::string exeNameRelease = currentDir.generic_string() + "-" + versionName + "_Release.exe";
                const std::string exeNameDebug = currentDir.generic_string() + "-" + versionName + "_Debug.exe";
                try{
                    std::filesystem::copy(exeNameRelease, versionDirBackup + "\\" + exeNameRelease);
                    std::filesystem::copy(exeNameDebug, versionDirBackup + "\\" + exeNameDebug);
                    
                    std::filesystem::rename(exeNameRelease, versionDir + "\\" + exeNameRelease);
                    std::filesystem::rename(exeNameDebug, versionDir + "\\" + exeNameDebug);
                }
                catch(std::filesystem::filesystem_error& renameDirError){
                    throw std::runtime_error(renameDirError);
                }
            }

            // Copy vector items to version directory and to backup-archive
            for(const auto& item : fileVector){
                std::filesystem::path itemAsFsPath(item); // Create fs::path type objects from string objects

                if(std::filesystem::exists(itemAsFsPath)){
                    // Check if relative or absolute path (since we want the structure to remain the same we always need to recreate it, meaning the whole relative path to a file or a dir)
                    if(itemAsFsPath.is_relative()){
                        // Check if item is in a subdirectory
                        if(itemAsFsPath.parent_path().empty()){
                            try{
                                std::filesystem::copy(itemAsFsPath, versionDir + "\\" + itemAsFsPath.string(), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                                std::filesystem::copy(itemAsFsPath, versionDirBackup + "\\" + itemAsFsPath.string(), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                            }
                            catch(std::filesystem::filesystem_error& itemCopyError){
                                std::cerr << "Line 238";
                                throw std::runtime_error(itemCopyError);
                            }
                        }
                        else{
                            try{
                                std::filesystem::create_directories(versionDir + "\\" + itemAsFsPath.parent_path().string()); // Recreate Subdirectories
                                std::filesystem::create_directories(versionDirBackup + "\\" + itemAsFsPath.parent_path().string()); // Recreate Subdirectories

                                std::filesystem::copy(itemAsFsPath, versionDir + "\\" + itemAsFsPath.parent_path().string() + "\\" + itemAsFsPath.filename().string(), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                                std::filesystem::copy(itemAsFsPath, versionDirBackup + "\\" + itemAsFsPath.parent_path().string() + "\\" + itemAsFsPath.filename().string(), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                            }
                            catch(std::filesystem::filesystem_error& itemCopyError){
                                std::cerr << "Line 254";
                                throw std::runtime_error(itemCopyError);
                            }
                        }
                    }
                    else if(itemAsFsPath.is_absolute()){
                        // Get relative part (if empty then its not in the working directory and we dont need to copy the structure)
                        // std::filesystem::path itemRelative = std::filesystem::relative(itemAsFsPath, currentPath);
                        // std::cout << "itemRelative: " << itemRelative << "currentPath: " << currentPath << std::endl;

                        // Check if current directory is contained in item
                        if(item.find(currentPath.string()) == std::string::npos){ // Not found
                            try{
                                std::filesystem::copy(itemAsFsPath, versionDir + "\\" + itemAsFsPath.filename().string(), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                                std::filesystem::copy(itemAsFsPath, versionDirBackup + "\\" + itemAsFsPath.filename().string(), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                            }
                            catch(std::filesystem::filesystem_error& itemCopyError){
                                std::cerr << "Line 269";
                                throw std::runtime_error(itemCopyError);
                            }
                        }
                        else{
                            // Get relative path of item
                            std::filesystem::path itemRelative = std::filesystem::relative(itemAsFsPath, currentPath);
                            
                            if(Debug_Mode){std::cout << "itemRelative: " << itemRelative << "currentPath: " << currentPath << std::endl;}

                            if(itemRelative.parent_path().empty()){ // no subdirs
                                try{
                                    std::filesystem::copy(itemAsFsPath, versionDir + "\\" + itemRelative.string(), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                                    std::filesystem::copy(itemAsFsPath, versionDirBackup + "\\" + itemRelative.string(), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                                }
                                catch(std::filesystem::filesystem_error& itemCopyError){
                                    std::cerr << "Line 279";
                                    throw std::runtime_error(itemCopyError);
                                }
                            }
                            else{ // subdirs
                                try{
                                    std::filesystem::create_directories(versionDir + "\\" + itemRelative.parent_path().string()); // Recreate Subdirectories
                                    std::filesystem::create_directories(versionDirBackup + "\\" + itemRelative.parent_path().string()); // Recreate Subdirectories

                                    std::filesystem::copy(itemAsFsPath, versionDir + "\\" + itemRelative.parent_path().string() + "\\" + itemRelative.filename().string(), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                                    std::filesystem::copy(itemAsFsPath, versionDirBackup + "\\" + itemRelative.parent_path().string() + "\\" + itemRelative.filename().string(), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                                }
                                catch(std::filesystem::filesystem_error& itemCopyError){
                                    std::cerr << "Line 293";
                                    throw std::runtime_error(itemCopyError);
                                }
                            }
                        }
                    }
                }
                else{
                    std::string itemNotExistError = "Fatal Error: Item \"" + item + "\"" + "was not found!";
                    throw std::runtime_error(itemNotExistError);
                }
            }
            std::cout << ANSI_COLOR_MAGENTA << "versiontool finished successfully" << ANSI_COLOR_RESET << std::endl;
        }
        else{
            throw std::runtime_error("User canceled operation");
        }
    }
    else{
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
}