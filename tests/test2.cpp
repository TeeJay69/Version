#include <fstream>
#include <iostream>


int main(){
    std::ofstream configFile(".versiontool"); // create file
    
    if(configFile.is_open()){
        configFile << "Files and directories included in version:\n\n----------\nCompile command:\n";
        configFile.close();

        if(std::system("cmd /c sss .versiontool") != 0){
                std::system("cmd /c notepad .versiontool");
        }
    }

    return 0;
}