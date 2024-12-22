# Version

**A simple yet powerful C++ command-line tool for compiling, running, and releasing your projects.**  
Originally called **versiontool**, this project has evolved into a robust utility that I regularly use to manage my builds and project releases.

<br/>

## Table of Contents
1. [Features](#features)  
2. [Requirements](#requirements)  
3. [Installation](#installation)  
4. [Usage](#usage)  
    - [General Help](#general-help)
    - [Compile](#compile)
    - [Run](#run)
    - [Release](#release)
    - [Config](#config)
5. [Configuration (.versiontool)](#configuration-versiontool)
6. [Contributing](#contributing)
7. [License](#license)

<br/>

## Features

- **Compile and Run**: Easily compile your source code and execute it with optional arguments.  
- **Release Management**: Create a new version release folder along with an optional installer package if an Inno Setup (ISS.iss) file is present.  
- **Configuration File**: Store file paths, compile commands, aliases, and more in a dedicated `.versiontool` file.  
- **Smart Execution**: Automatically detect output file names from common compiler flags (like `-o` or `/out:`).  
- **Alias Support**: Generate `.bat` alias scripts, making it simpler to call your executable with arguments.  
- **Colorful Console Output** (Windows): Uses Windows API to enable virtual terminal colors.  

<br/>

## Requirements

- **Operating System**: Primarily tested on Windows (uses Windows-specific headers and commands).  
- **Compiler**: A C++17 (or later) compiler.  
- **Inno Setup** (optional): Only required if you want to build an installer via `release` and you have an `ISS.iss` file.  
- **CMake or Visual Studio project** (optional): This tool is compiled in any environment supporting standard C++17 or later.  

> **Note**: The application depends on Windows console commands (like `cmd /c`, `taskkill`, etc.) and certain Windows APIs (like `GetModuleFileNameW`). It may require additional modifications to work on other platforms.

<br/>

## Installation

1. **Clone this Repository**  
   ```bash
   git clone https://github.com/your-username/version.git
   cd version
   ```

2. **Build the Executable**  
   - Using a **Visual Studio** project: Import the files into your project and build in Release mode.  
   - Using **CMake** (example):  
     ```bash
     mkdir build && cd build
     cmake ..
     cmake --build . --config Release
     ```
   - Or simply compile from the command line (Windows example):  
     ```bash
     cl /std:c++17 /EHsc main.cpp /Fe:version.exe
     ```

3. **Place the Executable in Your PATH** (optional)  
   - Copy the generated `version.exe` to a folder that’s in your system `PATH`, or  
   - Update your `PATH` environment variable to include the folder where `version.exe` is located.

<br/>

## Usage

Below are the essential commands and options. After building, use `version [command] [options]`.

### General Help
```bash
version --help
```
Displays usage instructions, available commands, and options.

### Compile
```bash
version compile [--run/-r] [--args/-a "command args"] [COMMAND]
```
- **`version compile`** uses the compile command specified in **`.versiontool`** by default.
- **`--run` / `-r`** immediately runs the compiled program.
- **`--args` / `-a`** passes arguments to the program when running.
- **`COMMAND`** if provided, overrides the default compile command in `.versiontool`.

### Run
```bash
version run [PATH_TO_EXE] [ARGS]
```
- Runs an executable.  
- If no path is specified, it attempts to run the output from the compile command defined in **`.versiontool`**.  

### Release
```bash
version release [VERSION_NAME]
```
- Creates a new version directory under `Versions/`.
- Compiles the project, copies the compiled binaries and any files/directories listed in **`.versiontool`** to the newly created version folder.
- If an `ISS.iss` file is present, it updates the version in that file, attempts to build an installer, and copies the resulting `-Setup.exe` to the new version folder.

### Config
```bash
version config
```
- Displays the contents of your `.versiontool` config file.

```bash
version config --raw
```
- Opens **`.versiontool`** in your default text editor (attempts VS Code, Vim, and Notepad, in that order).

<br/>

## Configuration (.versiontool)

This tool relies on a config file named **`.versiontool`** in the same directory as the executable or in your project’s root folder. It typically contains sections:

1. **Files and directories included in version:**  
   List every file or directory to copy to the release folder.  
   ```
   Files and directories included in version:
   [file or folder path 1]
   [file or folder path 2]
   ----------
   ```
2. **Compile command:**  
   A single line specifying how to compile your project.  
   ```
   Compile command:
   cl /std:c++17 /EHsc main.cpp /Fe:MyProgram.exe
   ----------
   ```
3. **Alias:**  
   Each alias line creates a corresponding `.bat` file in the release folder pointing to your compiled exe.  
   ```
   Alias:
   myprogram
   another_alias
   ----------
   ```

> **Note**: If `.versiontool` does not exist, the tool will prompt you to create one.

<br/>

## Contributing

Contributions, bug reports, and feature requests are welcome!  
1. **Fork** the repository  
2. **Create** a new branch for your feature/fix  
3. **Commit** your changes and submit a **Pull Request**  

If you find a bug or have a suggestion, feel free to open an issue with details and steps to reproduce.

<br/>

## License

see [License](Copyright-Disclaimer.md)