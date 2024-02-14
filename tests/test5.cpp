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


int main(int argc, char* argv[]){
    
    enableColors();
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Produce help message")
        ("config", po::value<std::string>(), "Configfile Path")
        ("compile", po::value<std::string>(), "Compile project (may be used with \"file\", \"cmd\")");
        ("cmd", po::value<std::string>(), "Compile command (overrides value from .versiontool)");
        ("file", po::value<std::string>(), ".cpp source file / executable for alias");
        ("release", po::value<std::string>(), "Creates a version release (may be used with \"include\", \"file\", \"alias\", \"cmd\", \"config\")");
        ("include", po::value<std::string>(), "\",\" separated list of paths to include in release. (overrides values from .versiontool)");
        ("alias", po::value<std::string>(), "Creates a batch file to call \"file\" (overrides value from .versiontool)");

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

    if (vm.count("config")) {
    std::cout << "config: " << vm["config"].as<std::string>() << std::endl;
    }

    if (vm.count("include")) {
    std::cout << "include: " << vm["include"].as<std::string>() << std::endl;
    }
    
    std::string compileCommand = "g++ main.cpp -o main.exe all";
    if(compileCommand.empty() || !(compileCommand.find("rmdir ") == std::string::npos) || !(compileCommand.find("rd ") == std::string::npos) || !(compileCommand.find("del ") == std::string::npos) || !(compileCommand.find("rm ") == std::string::npos) || !(compileCommand.find("move ") == std::string::npos) || !(compileCommand.find("mv ") == std::string::npos)){
        std::cout << "If statement true" << std::endl;
    }
    else{
        std::cout << "If statement false" << std::endl;
    }
}