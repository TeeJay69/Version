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

#define VERSION "v2.2.0"

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

static int argcmp(char** argv, int argc, const char* cmp){
    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i], cmp) == 0){
            return 1;
        }
    }
    return 0;
}

static int argfcmp(std::string& buff, char** argv, int argc, const char* cmp){
    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i], cmp) == 0){
            if(i + 1 < argc){
                buff = argv[i+1]; // I dunno if we have to resize buff before assigning, we'll see if it fails...
                return 1;
            }
        }
    }
    return 0;
}
void exitSignalHandler(int signal){
    
    std::cout << "Ctrl+C caught, initiating exit procedure" << ANSI_COLOR_RESET << std::endl << std::flush;
    std::cout << "Exit procedure finished!" << std::endl << std::flush;
    std::exit(signal);
}

void assignGlobal(const std::string& pLocFull){
    pLocat = std::filesystem::path(pLocFull).parent_path().string();
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

    // Return a.exe if no match is found
    return "";
}

inline void handleCompileOption(char* argv[], int argc){
    if(argv[2] == NULL){
        configFileParser CP(".versiontool");
        const std::string com = CP.readCompileCommand();
        if(checkCompileCom(com) != 1){
            return;
        }
        std::cout << com << std::endl;
        std::cout << ANSI_COLOR_YELLOW << "Compiling source code..." << ANSI_COLOR_RESET << std::endl;
        if(std::system(com.c_str()) != 0){
            std::cout << "Compilation failed with errors." << std::endl;
            return;
        }
        else{
            std::cout << ANSI_COLOR_GREEN << "Source code compiled successfully." << ANSI_COLOR_RESET << std::endl;
        }
    }
    else if(argcmp(argv, argc, "--run") || argcmp(argv, argc, "-r")){
        std::string com;
        if(std::string(argv[2]) == "--run" || std::string(argv[2]) == "-r"){
            if(argv[3] == NULL || std::string(argv[3]) == "--args" || std::string(argv[3]) == "-a"){
                configFileParser CP(".versiontool");
                com = CP.readCompileCommand();
            }
            else if(std::string(argv[3]) != "--args" && std::string(argv[3]) != "-a" && checkCompileCom(std::string(argv[3])) == 1){
                com = std::string(argv[3]);
            }
            else{
                std::cerr << "Fatal: Provided command is invalid";
            }
        }
        else if(checkCompileCom(std::string(argv[2])) == 1){
            com = std::string(argv[2]);
        }

        if(checkCompileCom(com) != 1){
            return;
        }
        std::cout << com << std::endl;
        std::cout << ANSI_COLOR_YELLOW << "Compiling source code..." << ANSI_COLOR_RESET << std::endl;
        if(std::system(com.c_str()) != 0){
            std::cout << "Compilation failed with errors." << std::endl;
            return;
        }
        else{
            std::cout << ANSI_COLOR_GREEN << "Source code compiled successfully." << ANSI_COLOR_RESET << std::endl;
        }
        std::string outName = extractOutputName(com); 
        if(outName.empty()){
            outName = "a.exe";
        }

        std::string args;
        std::string runCom;
        if(argfcmp(args, argv, argc, "--args") || argfcmp(args, argv, argc, "-a")){
            std::cout << "Args: '" << args << "'" << std::endl;
            runCom = "cmd /c \".\\" + outName + " " + args;
        }
        else{
            runCom = "cmd /c \".\\" + outName;
        }
        int retVal = std::system(runCom.c_str());
        std::cout << ANSI_COLOR_GREEN << "Program executed." << ANSI_COLOR_RESET << std::endl;
        std::cout << "Return value: " << ANSI_COLOR_169 << retVal << ANSI_COLOR_RESET << std::endl;
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
    std::regex versionPattern(R"(#define MyAppVersion \"v?\d+\.\d+\.\d+\")");

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



inline int handleReleaseOption(char* argv[], int argc){
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
    if(pname.empty()){
        std::cerr << "Fatal: output name is missing." << std::endl;
        return 1;
    }
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
        const std::string setupFileDest = vDir + "\\" + setupFile;
        const std::string setupFileDestBackup = vDirBackup + "\\" + setupFile;
        if(!std::filesystem::exists(setupFileDest)){
            std::filesystem::copy(setupFile, setupFileDest);
        }
        else{
            std::filesystem::remove(setupFileDest);
            std::filesystem::copy(setupFile, setupFileDest);
        }
        if(!std::filesystem::exists(setupFileDestBackup)){
            std::filesystem::copy(setupFile, setupFileDestBackup);
        }
        else{
            std::filesystem::remove(setupFileDestBackup);
            std::filesystem::copy(setupFile, setupFileDestBackup);
        }
    }
    
    std::cout << ANSI_COLOR_GREEN << vname << " released." << ANSI_COLOR_RESET << std::endl;

    return 0;
}

inline void handleConfigOption(char* argv[], int argc){
    if(argv[2] == NULL){
        std::ifstream in(".versiontool");
        if(!in){
            std::cerr << "Fatal: Failed to open file." << std::endl;
            return;
        }
        std::stringstream buff;
        buff << in.rdbuf();
        std::cout << buff.str() << std::endl;    
    }
    else if(std::string(argv[2]) == "--raw" || std::string(argv[2]) == "-r"){
        if(std::system("cmd /c code .versiontool") != 0){
            if(std::system("cmd /c vim .versiontool") != 0){
                if(std::system("cmd /c notepad .versiontool") != 0){
                    std::cerr << "Fatal: Failed to open config in editor." << std::endl;
                }
            }
        }
    }
    else{
        std::cerr << "Fatal: Invalid option." << std::endl;
    }
}

inline void handleRunOption(char* argv[], int argc){
    if(argv[2] == NULL){
        if(std::filesystem::exists(".versiontool")){
            std::cout << "Running project..." << std::endl;
            configFileParser CP(".versiontool");
            CP.configCheck();
            const std::string com = CP.readCompileCommand();
            std::string outName = extractOutputName(com);
            if(outName.empty()){
                outName = "a.exe";
            }

            const std::string runCom = "cmd /c \".\\" + outName;
            int retVal = std::system(runCom.c_str());
            std::cout << ANSI_COLOR_GREEN << "Program executed." << ANSI_COLOR_RESET << std::endl;
            std::cout << "Return value: " << ANSI_COLOR_169 << retVal << ANSI_COLOR_RESET << std::endl;
        }
        else{
            std::cerr << "Either provide a path or run without path inside the repo to execute the current project.";
        }
    }
    else if(std::filesystem::exists(std::string(argv[2]))){
        std::cout << "File: " << std::string(argv[2]) << std::endl;
        std::string runCom;
        if(argv[4] == NULL){
            runCom = "cmd /c \"" + std::string(argv[2]) + "\"";
        }
        else{
            runCom = "cmd /c \"" + std::string(argv[2]) + "\" " + std::string(argv[3]);
            std::cout << "Args: '" << std::string(argv[3]) << "'" << std::endl;
        }
        int retVal = std::system(runCom.c_str());
        std::cout << ANSI_COLOR_GREEN << "Program executed." << ANSI_COLOR_RESET << std::endl;
        std::cout << "Return value: " << ANSI_COLOR_169 << retVal << ANSI_COLOR_RESET << std::endl;
    }
    else if(std::filesystem::exists(".versiontool")){
        std::cout << "Running project with arguments..." << std::endl;
        configFileParser CP(".versiontool");
        const std::string com = CP.readCompileCommand();
        std::string outName = extractOutputName(com);
        if(outName.empty()){
            outName = "a.exe";
        }
        std::cout << "File: " << outName << std::endl;
        std::cout << "Args: '" << std::string(argv[2]) << "'" << std::endl;
        std::cout << "Output:" << std::endl;
        const std::string runCom = "cmd /c \"" + outName + "\" " + std::string(argv[2]);
        int retVal = std::system(runCom.c_str());
        std::cout << ANSI_COLOR_GREEN << "Program executed." << ANSI_COLOR_RESET << std::endl;
        std::cout << "Return value: " << ANSI_COLOR_169 << retVal << ANSI_COLOR_RESET << std::endl;
    }
    else{
        std::cout << "File: " << std::string(argv[2]) << std::endl;
        std::string runCom;
        if(argv[3] == NULL){
            runCom = "cmd /c \"" + std::string(argv[2]) + "\"";
        }
        else{
            std::cout << "Args: '" << std::string(argv[3]) << "'" << std::endl;
            runCom = "cmd /c \"" + std::string(argv[2]) + "\" " + std::string(argv[3]);
        }
        std::cout << "Output:" << std::endl;
        int retVal = std::system(runCom.c_str());
        std::cout << ANSI_COLOR_GREEN << "Program executed." << ANSI_COLOR_RESET << std::endl;
        std::cout << "Return value: " << ANSI_COLOR_169 << retVal << ANSI_COLOR_RESET << std::endl;
    }
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
    else if(std::string(argv[1]) == "--version" || std::string(argv[1]) == "version" || std::string(argv[1]) == "-v"){
        std::cout << "Version (JW-CoreUtils) " << ANSI_COLOR_36 <<  VERSION  ANSI_COLOR_RESET << std::endl;
        std::cout << "Copyright (C) 2024 - Jason Weber. All rights reserved." << std::endl;
    }
    else if(std::string(argv[1]) == "--help" || std::string(argv[1]) == "help" || std::string(argv[1]) == "-h"){
        std::cout << "Version (JW-CoreUtils) " << VERSION << std::endl;
        std::cout << "Copyright (C) 2024 - Jason Weber. All rights reserved." << std::endl;
        std::cout << "usage: version [OPTIONS]... [OPERAND]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "compile, -c [COMMAND]                     Compile project using either provided command or file (.versiontool).\n";
        std::cout << "    --run, -r                             Executes the program after building.\n";
        std::cout << "        --args, -a                        Specific arguments for '--run'.\n";
        std::cout << "run, -e [PATH] [ARGUMENTS]                Executes the program with or without arguments or when not specified, the program from the file (.versiontool).\n";
        std::cout << "release, -r [VERSION]                     Create a release (+ Setup.exe if ISS.iss file present (changes version name)).\n";
        std::cout << "config                                    Display config.\n";
        std::cout << "    --raw, -r                             Open config file in editor.\n";
        std::cout << "--help, -h                                Show this help message.\n";                                 
        std::cout << "--version, -v                             Display version.\n";
    }
    else if(std::string(argv[1]) == "compile" || std::string(argv[1]) == "-c"){
        handleCompileOption(argv, argc);
    }
    else if(std::string(argv[1]) == "release" || std::string(argv[1]) == "-r"){
        return handleReleaseOption(argv, argc);
    }
    else if(std::string(argv[1]) == "run" || std::string(argv[1]) == "-e"){
        handleRunOption(argv, argc);
    }
    else if(std::string(argv[1]) == "config"){
        handleConfigOption(argv, argc);
    }
    else{
        std::cerr << "Fatal: '" << argv[1] << "' is not a version command." << std::endl;
        return 1;
    }
    
    return 0;
}