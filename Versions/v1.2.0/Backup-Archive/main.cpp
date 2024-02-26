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
                    if(!line.empty() && line.find("----------") == std::string::npos && line.find("rmdir ") == std::string::npos && line.find("rd ") == std::string::npos && line.find("-o ") == std::string::npos && line.find("del ") == std::string::npos && line.find("rm ") == std::string::npos && line.find("move ") == std::string::npos && line.find("mv ") == std::string::npos){
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

            while(true){
                std::getline(configFile, line);
                if(line == "Alias:"){
                    std::getline(configFile, line);
                    if(!line.empty()){
                        if(line.find("----------") == std::string::npos){
                            aliasList.push_back(line);
                        }
                        else{
                            configFile.close();
                            break;
                        }
                    }
                    else{
                        throw std::runtime_error("Error: Alias config is empty!");
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

int main(int argc, char* argv[]){
    
    std::signal(SIGINT, exitSignalHandler);

    enableColors();
    po::options_description desc("All options");
    desc.add_options()
        ("help", "Produce help message")
        ("config", po::value<std::string>(), "Configfile Path")
        ("compile", po::value<std::string>(), "Compile project")
        ("debug", po::value<std::string>(), "Create debug version")
        ("cmd", po::value<std::string>(), "Compile command [cannot include output name!]")
        ("include", po::value<std::string>(), "Files to include in release [path1,path2])")
        ("release", po::value<std::string>(), "Creates a version release (may be used with \"include\", \"alias\", \"cmd\", \"config\", \"debug\")")
        ("callee", po::value<std::string>(), "alias for a callee [aliasname1,aliasname2]")
        ("alias", po::value<std::string>(), "Creates a batch file to call \"callee\" (overrides configfile)")
        ("version", po::value<std::string>(), "Display version");


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
        return 0;
    }
    if(vm.count("version")){
        std::cout << "v1.2.0" << std::endl;
        return 0;
    }
    
    // Parameter handling
    
    // to skip rest of program when command line parameters are provided
    int argConfigState = 0, argCompileState = 0, argCmdState = 0, argDebugState = 0, 
        argCalleeState = 0, argReleaseState = 0, argIncludeState = 0, argAliasState = 0;
    
    // Global values for specific param
    std::string configPath = ".versiontool";
    std::string compileCommand;
    std::string aliasName;

    std::vector<std::string> pathVector;

    if(vm.count("config")){
        configPath = vm["config"].as<std::string>();
        argConfigState = 1;
    }


    // "compile" parameter
    if(argc >= 2 && std::string(argv[1]) == "compile" || vm.count("compile")){
        std::string debugArgs;

        // Compile command argument
        int cmdArgState = 0; // for differentiating between param and configfile
        if(vm.count("cmd")){
            std::string cmdArg = vm["cmd"].as<std::string>();
            if(!cmdArg.empty() && cmdArg.find("rmdir ") == std::string::npos && cmdArg.find("rd ") == std::string::npos && cmdArg.find("del ") == std::string::npos && cmdArg.find("rm ") == std::string::npos && cmdArg.find("move ") == std::string::npos && cmdArg.find("mv ") == std::string::npos){
                compileCommand = cmdArg;
            }
            else{
                throw std::runtime_error("Error: Compile Command contains forbidden words!");
            }
            cmdArgState = 1;
        }
        else{
            configFileParser parser(configPath);
            compileCommand = parser.readCompileCommand();
        }

        const std::filesystem::path cmdCurrentPath = std::filesystem::current_path();
        const std::filesystem::path cmdCurrentDir = cmdCurrentPath.filename();

        std::cout << ANSI_COLOR_YELLOW << "Compiling program..." << ANSI_COLOR_RESET << std::endl; // Notify user of progress

        // debug version
        if(vm.count("debug")){
            debugArgs = " -DDebug";
            std::string cmdBuildDebugVersion;
            
            // args compile command
            if(cmdArgState){
                cmdBuildDebugVersion = "cmd /c " + vm["cmd"].as<std::string>() + debugArgs;
                
                if(std::system((cmdBuildDebugVersion).c_str()) == 0){
                    std::cout << ANSI_COLOR_YELLOW << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                else{
                    getchar();
                    throw std::runtime_error("Error: Program failed to compile");
                }
            }
            // configfile compile command
            else{
                cmdBuildDebugVersion = "cmd /c" + compileCommand + " -o" + cmdCurrentDir.generic_string() + "_Debug.exe" + debugArgs;
                
                if(std::system((cmdBuildDebugVersion).c_str()) == 0){
                    std::cout << ANSI_COLOR_YELLOW << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
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
                cmdBuildReleaseVersion = "cmd /c " + vm["cmd"].as<std::string>();
                
                if(std::system((cmdBuildReleaseVersion).c_str()) == 0){
                    std::cout << ANSI_COLOR_YELLOW << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
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
                    std::cout << ANSI_COLOR_YELLOW << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                else{
                    throw std::runtime_error("Error: Program failed to compile");
                }
            }
        }

        argCompileState = 1;
        std::exit(EXIT_SUCCESS);
    }

    // "release" parameter
    if(argc >= 2 && std::string(argv[1]) == "release" || vm.count("release")){
        std::string versionName;

        if(std::string(argv[1]) == "release"){
            versionName = argv[2];    
        }
        else if(vm.count("release")){
            versionName = vm["release"].as<std::string>();
        }

        std::string versionNamePath = ".\\Versions\\" + versionName;
        if(std::filesystem::exists(versionNamePath)){
            std::cerr << "Error: Version already exists!" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        int compileParamState = 0, aliasParamState = 0, debugParamState = 0, includeParamState = 0;

        std::string compileParam;

        // get compile command
        if(vm.count("cmd")){
            compileParam = vm["cmd"].as<std::string>();
            compileParamState = 1;           
        }
        else{
            configFileParser parser(configPath);
            compileParam = parser.readCompileCommand();
        }

        // get include param
        if(vm.count("include")){
            std::string pathString = vm["include"].as<std::string>();
            if(pathString.empty()){
                std::cerr << "Error no paths where provided for (include)";
                std::exit(EXIT_FAILURE);
            }

            std::stringstream pathStream(pathString);
            std::string token;
            while(std::getline(pathStream, token, ',')){
                pathVector.push_back(token);
            }

            includeParamState = 1;
        }

        // get alias values if provided
        std::string aliasParams;
        std::vector<std::string> aliasList;
        std::string aliasCalleeName;
        if(vm.count("alias")){
            aliasParams = vm["alias"].as<std::string>();
            if(!aliasParams.empty()){
                aliasParamState = 1;
            }
            
            // Parse alias string
            std::stringstream aliasStream(aliasParams);
            std::string token;
            while(std::getline(aliasStream, token, ',')){
                aliasList.push_back(token);
            }
        }
        else{
            // Get alias from configfile
            configFileParser parser(configPath);
            aliasList = parser.readAlias();
            if(!aliasList.empty()){
                aliasParamState = 1;
            }
        }

        // Debug parameter
        if(vm.count("debug")){
            debugParamState = 1;
        }


        configFileParser parser(configPath);
        std::vector<std::string> includePaths = parser.readIncludePaths();

        std::cout << ANSI_COLOR_GREEN << "Items included in the Release:" << ANSI_COLOR_RESET << std::endl;
        for(int i = 0; i < includePaths.size(); i++){
            std::cout << "[" << i + 1 << "]" << "  " << includePaths[i] << std::endl;
        }
        std::cout << std::endl;
        std::cout << ANSI_COLOR_GREEN << "Version Name: " << std::endl << ANSI_COLOR_RESET << versionName << std::endl;
        
        const std::filesystem::path currentPath = std::filesystem::current_path();
        const std::filesystem::path currentDirName = currentPath.filename();

        std::cout << ANSI_COLOR_YELLOW << "Compiling program..." << ANSI_COLOR_RESET << std::endl; // Notify user of progress

        if(!debugParamState){
            const std::string buildCommand = "cmd /c " + compileParam + " -o " + currentDirName.generic_string() + "-" + versionName + ".exe";
            if(std::system((buildCommand).c_str()) == 0){
                std::cout << ANSI_COLOR_YELLOW << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
            else{
                std::cerr << "Error: Build failed!" << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
        else{
            const std::string buildReleaseCommand = "cmd /c " + compileParam + " -o " + currentDirName.generic_string() + "-" + versionName + "_Release.exe " + "-DDebug";
            const std::string buildDebugCommand = "cmd /c " + compileParam + " -o " + currentDirName.generic_string() + "-" + versionName + "_Debug.exe " + "-DDebug";
            
            if(std::system((buildReleaseCommand).c_str()) == 0 && std::system((buildDebugCommand).c_str()) == 0){
                std::cout << ANSI_COLOR_YELLOW << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
            else{
                std::cerr << "Error: Build failed!" << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }

        // Create version directory
        const std::string versionDir = "Versions\\" + versionName;
        const std::string versionDirBackup = "Versions\\" + versionName + "\\" + "Backup-Archive";
        
        try{
            // std::filesystem::create_directories(versionDir);
            std::filesystem::create_directories(versionDirBackup);
        }
        catch(std::filesystem::filesystem_error& versionDirError){
            std::cerr << "Error creating version directories: Error-Code: (" << versionDirError.what() << ")" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // Copy and move
        if(!debugParamState){
            const std::string programName = currentDirName.generic_string() + "-" + versionName + ".exe";
            try{
                std::filesystem::copy(programName, versionDirBackup + "\\" + programName);
                std::filesystem::rename(programName, versionDir + "\\" + programName);    
            }
            catch(std::filesystem::filesystem_error& copyProgramError){
                std::cerr << "Error copying/moving program! Error-Code: (" << copyProgramError.what() << ")" << std::endl;
                std::exit(EXIT_FAILURE);
            }
            
            aliasCalleeName = programName;
        }
        else{
            const std::string programNameRelease = currentDirName.generic_string() + "-" + versionName + "_Release" + ".exe";
            const std::string programNameDebug = currentDirName.generic_string() + "-" + versionName + "_Debug" + ".exe";
            try{
                std::filesystem::copy(programNameRelease, versionDirBackup + "\\" + programNameRelease);
                std::filesystem::copy(programNameDebug, versionDirBackup + "\\" + programNameDebug);

                std::filesystem::rename(programNameRelease, versionDir + "\\" + programNameRelease);
                std::filesystem::rename(programNameDebug, versionDir + "\\" + programNameDebug);   
            }
            catch(std::filesystem::filesystem_error& copyProgramError){
                std::cerr << "Error copying/moving program! Error-Code: (" << copyProgramError.what() << ")" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            aliasCalleeName = programNameRelease;
        }

        // Create alias files
        if(aliasParamState){
            for(const auto& item : aliasList){
                const std::string aliasPath = versionDir + "\\" + item + ".bat";
                const std::string aliasPathBackup = versionDirBackup + "\\" + item + ".bat";

                std::ofstream aliasCallerFile(aliasPath);
                std::ofstream aliasCallerFileBackup(aliasPathBackup);

                if(aliasCallerFile.is_open() && aliasCallerFileBackup.is_open()){
                    aliasCallerFile << aliasCalleeName << " %*";
                    aliasCallerFileBackup << aliasCalleeName << " %*";

                    aliasCallerFile.close();
                    aliasCallerFileBackup.close();
                }
                else{
                    std::cerr << "Error creating alias file";
                    std::exit(EXIT_FAILURE);
                }
            }
        }
        
        // Get paths from config file when no include param
        if(includeParamState == 0){
            configFileParser parser(configPath);
            pathVector = parser.readIncludePaths();
        }

                
        // Copy vector items
        for(const auto& item : pathVector){
            std::filesystem::path itemAsFsPath(item);

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
                            std::cerr << "Error 238";
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
                            std::cerr << "Error 254";
                            throw std::runtime_error(itemCopyError);
                        }
                    }
                }
                else if(itemAsFsPath.is_absolute()){
                    // Check if current directory is contained in item
                    if(item.find(currentPath.string()) == std::string::npos){ // Not found
                        try{
                            std::filesystem::copy(itemAsFsPath, versionDir + "\\" + itemAsFsPath.filename().string(), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                            std::filesystem::copy(itemAsFsPath, versionDirBackup + "\\" + itemAsFsPath.filename().string(), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                        }
                        catch(std::filesystem::filesystem_error& itemCopyError){
                            std::cerr << "Error 269";
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
                                std::cerr << "Error 279";
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
                                std::cerr << "Error 293";
                                throw std::runtime_error(itemCopyError);
                            }
                        }
                    }
                }
            }
            else{
                std::cerr << "Error: Path \"" << item << "\" was not found!" << std::endl;
                return 1;
            }
        }
        std::cout << ANSI_COLOR_GREEN << "Version [" << versionName << "] was released" << ANSI_COLOR_RESET << std::endl;
        std::cout << ANSI_COLOR_GREEN << "Versiontool finished successfully!" << ANSI_COLOR_RESET << std::endl;
        argReleaseState = 1;

        return 0;
        
    }


    // "cmd" parameter
    if(vm.count("cmd")){
        compileCommand = vm["cmd"].as<std::string>();

        if(compileCommand.empty() || !(compileCommand.find("-o ") == std::string::npos) || !(compileCommand.find("rmdir ") == std::string::npos) || !(compileCommand.find("rd ") == std::string::npos) || !(compileCommand.find("del ") == std::string::npos) || !(compileCommand.find("rm ") == std::string::npos) || !(compileCommand.find("move ") == std::string::npos) || !(compileCommand.find("mv ") == std::string::npos)){
            throw std::runtime_error("Error: Compile Command is empty or contains forbidden elements!");
        }

        argCmdState = 1;     
    }

    // "include" parameter
    if(vm.count("include")){
        std::string pathString(vm["include"].as<std::string>());
        std::stringstream pathStream(pathString);
        std::string token;
        if(pathString.empty()){
            std::cerr << "Error no paths where provided for (include)" << std::endl;
            return 1;
        }
        while(std::getline(pathStream, token, ',')){
            pathVector.push_back(token);
        }
        
        argIncludeState = 1;
    }

    // "alias" parameter
    if(vm.count("alias")){
        aliasName = vm["alias"].as<std::string>();
        if(aliasName.empty()){
            std::cerr << "Error: \"alias\" parameter is empty" << std::endl;
            return 1;
        }

        argAliasState = 1;
    }
    
    // get alias values if provided
    std::string aliasParams;
    std::vector<std::string> aliasList;
    std::string aliasCalleeName;
    if(vm.count("alias")){
        aliasParams = vm["alias"].as<std::string>();
        if(!aliasParams.empty()){
            argAliasState = 1;
        }
        
        // Parse alias string
        std::stringstream aliasStream(aliasParams);
        std::string token;
        while(std::getline(aliasStream, token, ',')){
            aliasList.push_back(token);
        }
    }

    // "debug" parameter
    if(vm.count("debug")){
        argDebugState = 1;
    }

    // "callee" parameter:
    if(vm.count("callee")){
        std::string calleeName = vm["callee"].as<std::string>();
        if(calleeName.empty()){
            std::cerr << "Error: no name was provided for (callee)" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        if(argAliasState != 1){
            std::cerr << "Error: no (alias) was provided to call (callee)" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // Create alias files
        for(const auto& aliasName : aliasList){
            std::ofstream aliasFile(aliasName + ".bat");
            if(aliasFile.is_open()){
                aliasFile << calleeName << " %*";
                aliasFile.close();
            }
            else{
                std::cerr << "Error creating alias file for callee" << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }

        std::exit(EXIT_SUCCESS);
    }


    // Only few select command line parameters (config) (alias) (cmd) (include) (debug) // easier to maintain
    if(argCompileState == 0 && argCalleeState == 0 && argReleaseState == 0){
        configFileParser parser(configPath);
        // Get include files from config file
        if(argIncludeState != 1){
            pathVector = parser.readIncludePaths();
            if(pathVector.empty()){
                std::cerr << "Error: no files are included" << std::endl;
                return 1;
            }
        }

        // Get compile command from config file
        if(argCmdState != 1){
            compileCommand = parser.readCompileCommand();
            if(compileCommand.empty() || !(compileCommand.find("-o ") == std::string::npos) || !(compileCommand.find("rmdir ") == std::string::npos) || !(compileCommand.find("rd ") == std::string::npos) || !(compileCommand.find("del ") == std::string::npos) || !(compileCommand.find("rm ") == std::string::npos) || !(compileCommand.find("move ") == std::string::npos) || !(compileCommand.find("mv ") == std::string::npos)){
                throw std::runtime_error("Error: Compile Command is empty or contains forbidden elements!");
            }
        }
        
        // Get alias from config file
        int aliasFeature = 0;
        if(argAliasState == 1){
            // Alias argument provided
            aliasFeature = 1;
        }
        else{
            // Alias argument not provided
            aliasList = parser.readAlias();

            if(!aliasList.empty()){
                aliasFeature = 1;
            }
            else{
                aliasFeature = 0;
            }
        }

        std::cout << ANSI_COLOR_GREEN << "Items included in the Release:" << ANSI_COLOR_RESET << std::endl;

        for(int i = 0; i < pathVector.size(); i++){
            std::cout << "[" << i + 1 << "]" << "  " << pathVector[i] << std::endl;
        }
        std::cout << std::endl;


        int userResponse = 1;
        if(userResponse == 1){
            int filesystemCheck = 0;
            // Get version name
            std::string versionName; // for later
            while(filesystemCheck != 1){
                std::cout << ANSI_COLOR_GREEN << "Version Name: " << ANSI_COLOR_RESET << std::endl;
                // std::cin.clear();
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
                    std::cerr << versionNameError << std::endl;
                }
            }
            
            // Additional Debug version
            int debugVersion = 0; // for compile command
            if(argDebugState == 0){ // Without "debug" param
                int debugQuestion = 0;
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
            }
            else{
                debugVersion = 1;
            }

            const std::filesystem::path currentPath = std::filesystem::current_path();
            const std::filesystem::path currentDir = currentPath.filename();
            
            std::cout << ANSI_COLOR_YELLOW << "Compiling program..." << ANSI_COLOR_RESET << std::endl; // Notify user of progress

            // Compile the program
            if(debugVersion == 0){
                const std::string buildReleaseVersion = "cmd /c " + compileCommand + " -o " + currentDir.generic_string() + "-" + versionName + ".exe"; // " -O3" + " -march=native"
                
                if(std::system((buildReleaseVersion).c_str()) == 0){
                    std::cout << ANSI_COLOR_YELLOW << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                else{
                    throw std::runtime_error("Error: Program failed to compile");
                }
            }
            else if(debugVersion == 1){
                const std::string buildReleaseVersion = "cmd /c " + compileCommand + " -o " + currentDir.generic_string() + "-" + versionName + "_Release.exe" + " -DDebug"; // " -O3" + " -march=native"
                const std::string buildDebugVersion = "cmd /c " + compileCommand + " -o " + currentDir.generic_string() + "-" + versionName + "_Debug.exe" + " -DDebug"; // " -O3" + " -march=native"
                
                if(std::system((buildReleaseVersion).c_str()) == 0 && std::system((buildDebugVersion).c_str()) == 0){
                    std::cout << ANSI_COLOR_YELLOW << "Program Compiled Successfully!" << ANSI_COLOR_RESET << std::endl;
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
                exeNameForAlias = exeNameRelease;
            }

            // Create alias files
            if(aliasFeature == 1){
                for(const auto& aliasName : aliasList){
                    const std::string aliasPath = versionDir + "\\" + aliasName + ".bat";
                    const std::string aliasPathBackup = versionDirBackup + "\\" + aliasName + ".bat";

                    std::ofstream aliasCallerFile(aliasPath);
                    std::ofstream aliasCallerFileBackup(aliasPathBackup);

                    if(aliasCallerFile.is_open() && aliasCallerFileBackup.is_open()){
                        aliasCallerFile << exeNameForAlias << " %*";
                        aliasCallerFileBackup << exeNameForAlias << " %*";

                        aliasCallerFile.close();
                        aliasCallerFileBackup.close();
                    }
                    else{
                        std::cerr << "Error creating alias file";
                        std::exit(EXIT_FAILURE);
                    }
                }
            }

            // Copy vector items to version directory and to backup-archive
            for(const auto& item : pathVector){
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
            std::cout << ANSI_COLOR_GREEN << "Version [" << versionName << "] released" << ANSI_COLOR_RESET << std::endl;
            std::cout << ANSI_COLOR_GREEN << "Versiontool finished successfully!" << ANSI_COLOR_RESET << std::endl;
        }
        else{
            std::cout << "User canceled program" << std::endl;
            std::exit(EXIT_SUCCESS);
        }
    }

    return 0;
}