v1.1.1
    - Reduced dependency on dynamic libraries
    - boost program options now linked as a static instead of shared library
    - Cosmetic changes: Compile indicator, changed message colors
v1.1.0
    - Added command line parameter support
    - Integration with previous user interaction approach
    - Completely refactored program flow
    - Created class for parsing of config file
    - A ton of new features
    - see commit message (db7c9bc7)
v1.0.0
    - First official release.
    - Provides multiple useful features for smart creation of version releases.
    - Automagically compiles the project.
    - Automagically copies files defined in .versiontool file to the version directory.
    - Supports many different path notation styles. Including relative paths, absolute paths, absolute paths to files or subdirectories in the project directory and much more.
    - If defined in .versiontool creates an alias batch file for calling the executable with an alternate name.