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
#include <csignal>
#include <regex>
#include "..\ConfigSync\ANSIcolor.hpp"

#ifdef DEBUG
#define Debug_Mode 1
#else
#define Debug_Mode 0
#endif

#define VERSION "v1.2.1"

#ifdef _WIN32 

#include <Windows.h>


void enableColors()
{
    DWORD consoleMode;
    HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleMode(outputHandle, &consoleMode))
    {
        SetConsoleMode(outputHandle, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}

#endif


std::string pLocat;

class configFileParser{
    private:
        std::string fileName;
    public:
        configFileParser(const std::string& file) : fileName(file) {} // Constructor
    
        void configCheck(){
            if(!std::filesystem::exists(fileName)){ // Check config file
                std::string existError = "Error! file \"" + fileName + "\" passed to class (configFileParser) does not exist";
                std::cerr << existError << std::endl;
                
                std::cout << "Create config file? (yes/no) [default: yes]" << std::endl;
                std::string createConfigAnswer;
                std::getline(std::cin, createConfigAnswer);

                if(createConfigAnswer.empty() || createConfigAnswer == "yes"){
                    std::ofstream configFile(".versiontool");
                    if(configFile.is_open()){
                        configFile << "Files and directories included in version:\n\n----------\nCompile command:\n\n----------\nAlias:\n\n----------";        
                        configFile.close();
                        
                        if(std::system("cmd /c code .versiontool") != 0){
                            std::system("cmd /c notepad .versiontool");
                        }
                    }
                    else{
                        throw std::runtime_error("Error creating config file");
                    }
                }
                else{
                    std::exit(EXIT_SUCCESS);
                }
            }
        }
        
        std::vector<std::string> readIncludePaths(){
            configCheck();

            std::ifstream configFile(fileName);
            std::vector<std::string> includePaths;
            std::string line;

            std::getline(configFile, line);
            if(line == "Files and directories included in version:"){
                int lineParser = 1;
                while(lineParser == 1){
                    std::getline(configFile, line);
                    
                    if((line != "----------")){
                        includePaths.push_back(line);
                    }
                    else{
                        configFile.close();
                        lineParser = 0;
                        break;
                    }
                }
            }
            return includePaths;
        }

        std::string readCompileCommand(){
            configCheck();

            std::string compileCommand;
            std::string line;
            std::ifstream configFile(fileName);

            while(true){
                std::getline(configFile, line);
                if(line == "Compile command:"){
                    std::getline(configFile, line);
                    if(!line.empty() && line.find("----------") == std::string::npos && line.find("rmdir ") == std::string::npos && line.find("rd ") && line.find("del ") == std::string::npos && line.find("rm ") == std::string::npos && line.find("move ") == std::string::npos && line.find("mv ") == std::string::npos){
                        compileCommand = line;
                        configFile.close();
                        break;
                    }
                    else{
                        throw std::runtime_error("Error: Compile Command is empty or contains forbidden words!");
                    }
                }
            }
            return compileCommand;            
        }

        std::vector<std::string> readAlias(){
            configCheck();

            std::vector<std::string> aliasList;
            std::ifstream configFile(fileName);
            std::string line;

            int getter = 1; 
            while(getter){
                std::getline(configFile, line);
                if(line == "Alias:"){
                    while(true){
                        std::getline(configFile, line);
                        if(!line.empty() && line.find("----------") == std::string::npos){
                            aliasList.push_back(line);
                        }
                        else{
                            configFile.close();
                            getter = 0;
                            break;
                        }
                    }
                }
            }
            return aliasList;
        }
};

void exitSignalHandler(int signal){
    
    std::cout << "Ctrl+C caught, initiating exit procedure" << ANSI_COLOR_RESET << std::endl << std::flush;
    std::cout << "Exit procedure finished!" << std::endl << std::flush;
    std::exit(signal);
}

/**
 * @brief Recursively copy a directory or a single file
 * @param source Source path
 * @param destination Destination path
 * @copyright Credit goes to synchronizer.hpp from ConfigSync!
 */
inline int recurse_copy(const std::filesystem::path& source, const std::string& destination){

    if(std::filesystem::is_directory(source)){
        for(const auto& entry : std::filesystem::directory_iterator(source)){
            if(entry.is_directory()){
                const std::string dst = destination + "\\" + entry.path().filename().string();
                if(!std::filesystem::exists(dst)){
                    std::filesystem::create_directories(dst);
                }
                recurse_copy(entry.path(), dst);
            }
            else{
                try{
                    if(!std::filesystem::exists(destination)){
                        std::filesystem::create_directories(destination);
                    }
                    const std::string dstPath = destination + "\\" + entry.path().filename().string(); // full destination
                    std::filesystem::copy_file(entry.path(), dstPath, std::filesystem::copy_options::overwrite_existing);
                }
                catch(const std::filesystem::filesystem_error& err){
                    std::cerr << "Error in recurse_copy:" << err.what() << std::endl;
                    return 0;
                }
            }
        }
    }
    else{
        try{
            if(!std::filesystem::exists(destination)){
                std::filesystem::create_directories(destination);
            }
            const std::string dstPath = destination + "\\" + std::filesystem::path(source).filename().string(); // full destination
            std::filesystem::copy_file(source, dstPath, std::filesystem::copy_options::overwrite_existing);
        }
        catch(const std::filesystem::filesystem_error& err){
            std::cerr << "Error in recurse_copy:" << err.what() << std::endl;
            return 0;
        }
    }

    
    return 1;
}

inline int checkCompileCom(const std::string& com){
    if(com.empty()){
        std::cout << "Fatal: No command provided." << std::endl;
        return 0;
    }
    else if(com.find("rmdir ") != std::string::npos && com.find("rd ") != std::string::npos && com.find("del ") != std::string::npos && com.find("rm ") != std::string::npos && com.find("move ") != std::string::npos && com.find("mv ") != std::string::npos){
        std::cout << "Fatal: Illegal strings inside command." << std::endl;
        return 0;
    }
    return 1;
}

inline int compile(const std::string& com){
    std::cout << ANSI_COLOR_YELLOW << "Compiling source code..." << ANSI_COLOR_RESET << std::endl;
    if(std::system(com.c_str()) != 0){
        std::cout << "Compilation failed with errors." << std::endl;
        return 0;
    }
    else{
        std::cout << ANSI_COLOR_GREEN << "Source code compiled successfully." << ANSI_COLOR_RESET << std::endl;
    }
    return 1;
}

inline void handleCompileOption(char* argv[], int argc){
    if(argv[2] == NULL){
        configFileParser CP(".versiontool");
        const std::string com = CP.readCompileCommand();
        if(checkCompileCom(com) != 1){
            return;
        }
        std::cout << ANSI_COLOR_YELLOW << "Compiling source code..." << ANSI_COLOR_RESET << std::endl;
        if(std::system(com.c_str()) != 0){
            std::cout << "Compilation failed with errors." << std::endl;
            return;
        }
        else{
            std::cout << ANSI_COLOR_GREEN << "Source code compiled successfully." << ANSI_COLOR_RESET << std::endl;
        }
    }
}


inline std::string getOutputName(const std::string& com){
    const std::string flag = "-o";
    // Find position of "-o"
    size_t pos = com.find(flag);
    
    if (pos != std::string::npos) {
        // Find the start of the output file name
        pos += flag.length() + 1; // Add  1 to skip the space after "-o"
        
        // Find the end of the output file name (space or end of string)
        size_t end = com.find(' ', pos);
        
        // Extract the output file name
        return com.substr(pos, end - pos);
    }

    return {};
}

inline void updateVersion(const std::string& filepath, const std::string& newVersion) {
    std::ifstream fileIn(filepath);
    std::string content = "";
    std::string line;

    if (!fileIn.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return;
    }

    // Use regex to find and replace the version string
    std::regex versionPattern(R"(#define MyAppVersion "\d+\.\d+\.\d+")");

    while (getline(fileIn, line)) {
        // Replace the existing version with the new version
        line = std::regex_replace(line, versionPattern, "#define MyAppVersion \"" + newVersion + "\"");
        content += line + "\n";
    }

    fileIn.close();

    // Write the updated content back to the file
    std::ofstream fileOut(filepath);
    if (fileOut.is_open()) {
        fileOut << content;
        fileOut.close();
        std::cout << "Version updated successfully." << std::endl;
    } else {
        std::cerr << "Failed to write to file." << std::endl;
    }
}

void assignGlobal(const std::string& pLocFull){
    pLocat = std::filesystem::path(pLocFull).parent_path().string();
}

int main(int argc, char* argv[]){
    WCHAR wchr[ MAX_PATH ];
    GetModuleFileNameW(NULL, wchr, MAX_PATH);
    const std::wstring wstr(wchr);
    std::string pLocatFull;
    pLocatFull.resize(wstr.length());
    transform(wstr.begin(), wstr.end(), pLocatFull.begin(), [] (wchar_t c) {
    return static_cast<char>(c);
    });

    enableColors();
    assignGlobal(pLocatFull);
    std::signal(SIGINT, exitSignalHandler);

    if(argv[1] == NULL){
        std::cout << "Version (JW-CoreUtils) " << VERSION << std::endl;
        std::cout << "Copyright (C) 2024 - Jason Weber. All rights reserved." << std::endl;
        std::cout << "Software release tool." << std::endl;  
        std::cout << "See 'version --help' for usage." << std::endl;
    }
    else if(argc >= 2 && std::string(argv[1]) == "--version" || std::string(argv[1]) == "version" || std::string(argv[1]) == "-v"){
        std::cout << VERSION << std::endl;
    }
    else if(std::string(argv[1]) == "compile" || std::string(argv[1]) == "-c"){
        handleCompileOption(argv, argc);
    }
    else if(std::string(argv[1]) == "release" || std::string(argv[1]) == "-r"){
        std::string vname;
        if(argv[2] == NULL){
            std::cout << ANSI_COLOR_GREEN << "Enter version name:";
            std::getline(std::cin, vname);
            if(vname.empty()){
                std::cerr << "Fatal: missing version name." << std::endl;
                return 1;
            }
        }
        else{
            vname = std::string(argv[2]);
        }

        const std::string vDir = "Versions\\" + vname;
        const std::string vDirBackup = "Versions\\" + vname + "\\backup";

        if(std::filesystem::exists(vDir)){
            std::cerr << "Fatal: Version exists already." << std::endl;
            return 1;
        }
        else{
            std::filesystem::create_directories(vDir);
            std::filesystem::create_directories(vDirBackup);
        }

        configFileParser P(".versiontool");
        P.configCheck();
        const std::string com = P.readCompileCommand();
        if(checkCompileCom(com) != 1){
            return 1;
        }
        
        if(compile(com) != 1){
            return 1;
        }

        const std::string pname = getOutputName(com);
        recurse_copy(pname, vDir);
        recurse_copy(pname, vDirBackup);

        for(const auto& elem : P.readIncludePaths()){
            recurse_copy(elem, vDir);
            recurse_copy(elem, vDirBackup);
        }
        for(const auto& elem : P.readAlias()){
            const std::string f = elem + ".bat";
            std::ofstream of(f);
            of << pname << " %*";
            of.close();
            std::filesystem::copy(f, vDir + "\\" + f);
            std::filesystem::copy(f, vDirBackup + "\\" + f);
        }

        const std::string iss = "ISS.iss";
        if(std::filesystem::exists(iss)){
            updateVersion(iss, vname);
            const std::string isscom = "cmd /c iscc " + iss;
            if(std::system(isscom.c_str()) != 0){
                std::cerr << "Failed to compile pascal file." << std::endl;
                std::cout << "Try adding 'C:\\Program Files (x86)\\Inno Setup 6' to your PATH environment variable." << std::endl;
                return 1;
            }

            std::string pname_noext = pname;
            std::string ext = ".exe";
            pname_noext.erase(pname_noext.find(ext), ext.length()); 
            const std::string setupFile = pname_noext + "-Setup.exe";
            std::filesystem::copy(setupFile, vDir + "\\" + setupFile);
            std::filesystem::copy(setupFile, vDirBackup + "\\" + setupFile);
        }
        
        std::cout << ANSI_COLOR_GREEN << vname << " released." << ANSI_COLOR_RESET << std::endl;
    }
    else{
        std::cerr << "Fatal: '" << argv[1] << "' is not a vtool command." << std::endl;
        return 1;
    }
    
    return 0;
}