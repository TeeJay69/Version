All filepaths are in array
Ask for confirmation on the files that are included in the release
    - List the files
    - Prompt for confirmation
    - Search for main.cpp and main.c, if both found return error //when compile params already have that info, we can skip this step
    - Store programming language info in variable later use (e.g. Compile command) 
If confirmed ask for the version name
    - Whats the name of this version?
    - Check if directory for this version already exists
        - if empty continue
        - if not empty throw error
Ask if an additional Debug version shall be created as well or not
    - Yes --> Add -DDebug parameter to compile command
        - Create additional subfolders for debug and non debug version
    - No --> Nothing changes
    
Read out the compile command from the config file
    - Ensure that parameter -o is not specified
    - Or use value from -o instead of asking the user for providing the name
Add the output file name parameter according to version name
Compile the program and move all files from filearray + the compiled program to the Named directory in the Versions directory.
    - Call to command line
    - 
Copy everything to a backup directory inside the release directory
- Check if item is a path or a filename
    - If its a directory
        - Copy the directory
    - If its a file, copy the file


TODO:
- When .versiontool does not exist yet
- Editing of the included files
- command line parameter support
    - add to add a specified path to list of included files
- Add alias parser function to my class
Command line support in new branch
vtool


Command line arguments:
    - config (path to .versiontool file)
    - compile (compiles "file" if provided, otherwise it will compile main.cpp)
    - cmd (specify compile command, will not use .versiontool command)
    - file (specify a C++ source file as a source for "release" or "compile") / an executable, for the alias
    - release (create a version release. Can be followed by "include", "alias", "file", "cmd", "config")
    - include (specify filepaths to include, overrides values from configfile)
    - alias (in combination with "file", creates batch file to call "file") / in combination with a release, to specify the alias. If not available, use .versiontool file alias, if not provided, no alias is created.

Include development will be in future version (see: "Z:\Archives\websites\Websites\Stackoverflow\C++_ Boost program_options_ Multiple lists of arguments - Stack Overflow.html")