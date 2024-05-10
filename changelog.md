# v2.1.2
### Fix ISS file version update
- Version was not correctly updated

# v2.1.1 Hotfix
    - Patches filesystem exception that occured during all releases
# v2.1.0
    - config command to display config. config '--raw', '-r' to open config file in editor
    - Formatted help message
# v2.0.0
    - Refactor
    - Supports ISS.iss file, updates the version within the file automatically.
    - Compiles ISS.iss file
# v1.2.1
    - Fixed: Combined planned release and critical hotfix.
    - HotFix: Resolved bug resulting in endless loop.
    - Fixed: Recognized version argument.
    - Fixed: Version argument not working with double hyphens.
    - Moved version part above boost initializer to resolve issue with boost::program_options.
    - Noticed fatal bug in previous release v1.2.0.
    - Retained v1.2.0 but removed any official git tags referencing it.
    - Released as v1.2.1.
    
# v1.1.1
    - Reduced dependency on dynamic libraries
    - boost program options now linked as a static instead of shared library
    - Cosmetic changes: Compile indicator, changed message colors
    
# v1.1.0
    - Added command line parameter support
    - Integration with previous user interaction approach
    - Completely refactored program flow
    - Created class for parsing of config file
    - A ton of new features
    - see commit message (db7c9bc7)

# v1.0.0
    - First official release.
    - Provides multiple useful features for smart creation of version releases.
    - Automagically compiles the project.
    - Automagically copies files defined in .versiontool file to the version directory.
    - Supports many different path notation styles. Including relative paths, absolute paths, absolute paths to files or subdirectories in the project directory and much more.
    - If defined in .versiontool creates an alias batch file for calling the executable with an alternate name.