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
#include <boost/program_options.hpp>

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

#ifdef _WIN32

#include <Windows.h>

namespace po = boost::program_options;

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


class configFileParser{
    private:
        std::string fileName;
    public:
        configFileParser(const std::string& file) : fileName(file) {} // Constructor
    
        std::vector<std::string> readIncludePaths(std::string file){
            if(!std::filesystem::exists(file)){
                std::string existError = "Error! file \"" + file + "\" passed to class (configFileParser) does not exist";
                throw std::runtime_error(existError);
            }
            
            std::ifstream configFile(file);
            std::vector<std::string> includePaths;
            std::string line;

            std::getline(configFile, line);
            if(line == "Files and directories included in version:"){
                int lineParser = 1;
                while(lineParser){
                    std::getline(configFile, line);
                    
                    if(!(line == "----------")){
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

        std::string readCompileCommand(std::string file){
            if(!std::filesystem::exists(file)){
                std::string existError = "Error! file \"" + file + "\" passed to class (configFileParser) does not exist";
                throw std::runtime_error(existError);
            }

            std::string compileCommand;
            std::string line;
            std::ifstream configFile(file);

            while(true){
                std::getline(configFile, line);
                if(line == "Compile command:"){
                    std::getline(configFile, line);
                    if(!line.empty() && line.find("rmdir ") == std::string::npos && line.find("rd ") == std::string::npos && line.find("-o ") == std::string::npos && line.find("del ") == std::string::npos && line.find("rm ") == std::string::npos && line.find("move ") == std::string::npos && line.find("mv ") == std::string::npos){
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
};


int main(int argc, char* argv[]){
    
    // if(Debug_Mode){std::cout << compileCmdConfigfile(".versiontool") << std::endl;getchar(".versiontool");}

    enableColors();
    po::options_description desc("All options");
    desc.add_options()
        ("help", "Produce help message")
        ("config", po::value<std::string>(), "Configfile Path")
        ("compile", po::value<std::string>(), "Compile project (may be used with \"cmd\")")
        ("debug", po::value<std::string>(), "Compile as debug version (can only be used with \"compile\"")
        ("cmd", po::value<std::string>(), "Compile command (overrides value from .versiontool)")
        ("release", po::value<std::string>(), "Creates a version release (may be used with \"include\", \"alias\", \"cmd\", \"config\")")
        ("file", po::value<std::string>(), "executable for alias")
        ("alias", po::value<std::string>(), "Creates a batch file to call \"file\" (overrides value from .versiontool)");
        //("include", po::value<std::vector<std::string>>()->multitoken(), "\",\" separated list of paths to include in release. (overrides values from .versiontool)")

    // po::options_description configFileOptions("Config file options");
    // configFileOptions.add_options()
    //     ("config-file", po::value<std::string>(), "Config file");

    po::options_description allOptions("All options"); // Initialize allOptions
    allOptions.add(desc); // Add desc to allOptions

    po::variables_map vm;

    po::store(po::parse_command_line(argc, argv, allOptions), vm);
    po::notify(vm);

    if(vm.count("help")){
        std::cout << desc << std::endl;
    }
    
    int argConfigState, argCompileState, argCmdState, argFileState, argReleaseState, argAliasState; // to skip rest of program when command line parameters are provided
    
    std::string configPath = ".versiontool";
    
    if(vm.count("config")){
        configPath = vm["config"].as<std::string>();
        argConfigState = 1;
    }

    // "compile" args parameter
    if(argv[1] == "compile" || vm.count("compile")){
        
        std::string compileCommand = compileCmdConfigfile(configPath);
        std::string debugArgs;

        // // File argument
        // if(vm.count("file")){
        //     sourceFile = vm["file"].as<std::string>();
        // }
        
        // Compile command argument
        int cmdArgState = 0;
        if(vm.count("cmd")){
            std::string cmdArg = vm["cmd"].as<std::string>();
            if(cmdArg.find("rmdir ") == std::string::npos && cmdArg.find("rd ") == std::string::npos && cmdArg.find("del ") == std::string::npos && cmdArg.find("rm ") == std::string::npos && cmdArg.find("move ") == std::string::npos && cmdArg.find("mv ") == std::string::npos){
                compileCommand = cmdArg; 
            }
            else{
                throw std::runtime_error("Error: Compile Command contains forbidden words!");
            }
            cmdArgState = 1;
        }

        const std::filesystem::path cmdCurrentPath = std::filesystem::current_path();
        const std::filesystem::path cmdCurrentDir = cmdCurrentPath.filename();

        // debug version
        if(vm.count("debug")){
            debugArgs = " -DDebug";
            std::string cmdBuildDebugVersion;
            
            // args compile command
            if(cmdArgState){
                cmdBuildDebugVersion = "cmd /c" + vm["cmd"].as<std::string>() + debugArgs;
                
                if(std::system((cmdBuildDebugVersion).c_str()) == 0){
                    std::cout << ANSI_COLOR_GREEN << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                else{
                    throw std::runtime_error("Error: Program failed to compile");
                }
            }
            // configfile compile command
            else{
                cmdBuildDebugVersion = "cmd /c" + compileCommand + " -o" + cmdCurrentDir.generic_string() + "_Debug.exe" + debugArgs;
                
                if(std::system((cmdBuildDebugVersion).c_str()) == 0){
                    std::cout << ANSI_COLOR_GREEN << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                else{
                    throw std::runtime_error("Error: Program failed to compile");
                }
            }
        }
        // Non debug version
        else{
            std::string cmdBuildReleaseVersion;

            // args compile command
            if(cmdArgState){
                cmdBuildReleaseVersion = "cmd /c" + vm["cmd"].as<std::string>();
                
                if(std::system((cmdBuildReleaseVersion).c_str()) == 0){
                    std::cout << ANSI_COLOR_GREEN << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                else{
                    throw std::runtime_error("Error: Program failed to compile");
                }
            }
            // configfile compile command
            else{
                cmdBuildReleaseVersion = "cmd /c" + compileCommand + " -o" + cmdCurrentDir.generic_string() + ".exe";
                
                if(std::system((cmdBuildReleaseVersion).c_str()) == 0){
                    std::cout << ANSI_COLOR_GREEN << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                else{
                    throw std::runtime_error("Error: Program failed to compile");
                }
            }
        }

        argCompileState = 1;
    }

    // "release" command line parameter
    if(argv[1] == "release" || vm.count("release")){
        std::string versionName;

        if(argv[1] == "release"){
            versionName = argv[2];    
        }
        else if(vm.count("release")){
            versionName = vm["release"].as<std::string>();
        }
        
        int compileParamState = 0;
        std::string compileParam;
        std::string aliasParam;

        // get compile command value if provided as parameter
        if(vm.count("cmd")){
            compileParam = vm["cmd"].as<std::string>();
            compileParamState = 1;           
        }
        else{
            compileParam = compileCmdConfigfile(configPath);
        }
        
        // get alias value if provided
        if(vm.count("alias")){
            aliasParam = vm["alias"].as<std::string>();
        }

        argReleaseState = 1;
    }
    
    if(std::filesystem::exists(configPath)){
        if(Debug_Mode){std::cout << "ConfigFile is present" << std::endl;}

        std::ifstream versiontoolFile(configPath);

        std::string line;
        std::vector<std::string> fileVector;
        std::string compileConfig; // for later
        std::string aliasConfig; // for later

        int configParser = 1;
        while(configParser == 1){
            std::getline(versiontoolFile, line);
            if(line == "Files and directories included in version:"){
                while(std::getline(versiontoolFile, line)){
                    int delimiterHit = 0;

                    if(line == "----------"){ // delimiter
                        delimiterHit  = 1; // switches to this if statement in all future iterations
                        
                        // Get compile parameters
                        std::getline(versiontoolFile, line);
                        
                        if(line == "Compile command:"){
                            std::getline(versiontoolFile, line);

                            if(line.find("rmdir ") == std::string::npos && line.find("rd ") == std::string::npos && line.find("-o ") == std::string::npos && line.find("del ") == std::string::npos && line.find("rm ") == std::string::npos && line.find("move ") == std::string::npos && line.find("mv ") == std::string::npos){
                                if(line.empty()){
                                    throw std::runtime_error("Error: Compile command is empty!");
                                }

                                compileConfig = line;
                            }
                            else{
                                // std::cerr << "Error: Compile Command contains forbidden words!";
                                throw std::runtime_error("Error: Compile Command contains forbidden words!");
                            }
                        }
                        else{
                            throw std::runtime_error("Error compile command not found");
                        }

                        // Get alias name
                        std::getline(versiontoolFile, line);
                        if(line == "----------"){
                            std::getline(versiontoolFile, line);
                            if(line == "Alias:"){
                                std::getline(versiontoolFile, line);
                                aliasConfig = line;

                                versiontoolFile.close();
                                configParser = 0;
                                break;
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
        std::cout << std::endl;
        // std::cout << std::endl << "Do You Want to Proceed? (yes=1, no=2): ";
        // std::string userResponse;

        // // Filter input
        // int proceedQuestion = 0;
        // while(proceedQuestion != 1){
        //     // std::cin.clear();
        //     // std::cin.ignore(SIZE_MAX, '\n');
        //     std::cin >> userResponse;
        //     if(userResponse == "1" || userResponse == "2" && userResponse.size() == 1){
        //         proceedQuestion = 1;
        //     }
        //     else{
        //         std::cerr << "Invalid Input, Please try again: ";
        //     }
        // }
        int userResponse = 1;
        if(userResponse == 1){
            int filesystemCheck = 0;
            // Get version name
            std::string versionName; // for later
            while(filesystemCheck != 1){
                std::cout << ANSI_COLOR_GREEN << "Version Name: " << ANSI_COLOR_RESET << std::endl;
                std::getline(std::cin, versionName);
                std::string versionNamePath = ".\\Versions\\" + versionName;
                
                if(!std::filesystem::exists(versionNamePath)){
                    // std::cout << "Directory does not exist yet" << std::endl;
                    filesystemCheck = 1;    
                }
                else{
                    if(versionName.empty()){
                        std::cerr << "Error: Please Enter a Version Name!" << std::endl;
                        continue;                       
                    }
                    std::string versionNameError = "Error: \"" + versionName + "\" already exists \n\n";
                    std::cerr << versionNameError;
                }
            }
            
            // Additional Debug version
            int debugQuestion = 0;
            int debugVersion; // for compile command
            while(debugQuestion != 1){
                std::cout << ANSI_COLOR_GREEN << "Include a debug version in the release? (no=1, yes=2) [default: no]: " << ANSI_COLOR_RESET << std::endl;
                std::string debugAnswer;
                std::getline(std::cin, debugAnswer);
                if(debugAnswer == "2" | debugAnswer == "yes"){
                    debugVersion = 1;
                    debugQuestion = 1;
                }
                else if(debugAnswer == "1" || debugAnswer.empty() || debugAnswer == "no"){
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
                const std::string buildReleaseVersion = "cmd /c " + compileConfig + " -o " + currentDir.generic_string() + "-" + versionName + ".exe"; // " -O3" + " -march=native"
                
                if(std::system((buildReleaseVersion).c_str()) == 0){
                    std::cout << ANSI_COLOR_GREEN << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                else{
                    throw std::runtime_error("Error: Program failed to compile");
                }
            }
            else if(debugVersion == 1){
                const std::string buildReleaseVersion = "cmd /c " + compileConfig + " -o " + currentDir.generic_string() + "-" + versionName + "_Release.exe" + " -DDebug"; // " -O3" + " -march=native"
                const std::string buildDebugVersion = "cmd /c " + compileConfig + " -o " + currentDir.generic_string() + "-" + versionName + "_Debug.exe" + " -DDebug"; // " -O3" + " -march=native"
                
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
            std::string exeNameForAlias;
            std::string exeNameReleaseForAlias;

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

                    std::filesystem::rename(exeName, versionDir + "\\" + exeName); // move
                }
                catch(std::filesystem::filesystem_error& renameDirError){
                    throw std::runtime_error(renameDirError);
                }
                exeNameForAlias = exeName;
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
                exeNameReleaseForAlias = exeNameRelease;
            }

            // Create alias file
            if(!aliasConfig.empty()){
                std::string aliasFileVersionPath = versionDir + "\\" + aliasConfig + ".bat";
                std::string aliasFileBackupPath = versionDirBackup + "\\" + aliasConfig + ".bat";
                std::ofstream aliasFile(aliasFileVersionPath);
                std::ofstream aliasFileBackup(aliasFileBackupPath);
                
                if(aliasFile.is_open() && aliasFileBackup.is_open()){
                    if(debugVersion == 0){
                        aliasFile << exeNameForAlias << " %*";
                        aliasFileBackup << exeNameForAlias << " %*";
                    }
                    else{
                        aliasFile << exeNameReleaseForAlias << " %*";
                        aliasFileBackup << exeNameReleaseForAlias << " %*";
                    }
                    
                    aliasFile.close();
                    aliasFileBackup.close();
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
        std::ofstream configFile(".versiontool"); // create file
        if(configFile.is_open()){
            configFile << "Files and directories included in version:\n\n----------\nCompile command:\n\n----------\nAlias:\n\n----------";
            configFile.close();

            if(std::system("cmd /c code .versiontool") != 0){
                    std::system("cmd /c notepad .versiontool");
            }
        }
    }

    return 0;
}