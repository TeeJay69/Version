; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Version"
#define MyAppVersion "v2.3.0"
#define MyAppPublisher "Jason Weber"
#define MyAppURL "https://github.com/TeeJay69"
#define MyAppExeName "Version.exe"




[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{9D6A540E-2A9D-4127-9E1A-8BDE58AF1754}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={localappdata}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=C:\Data\TJ\Software\Coding\Versiontool\Copyright-Disclaimer.md
; Uncomment the following line to run in non administrative install mode (install for current user only.)
PrivilegesRequired=lowest  
OutputDir=C:\Data\TJ\Software\Coding\Versiontool
OutputBaseFilename=Version-Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern
; Tell Windows Explorer to reload the environment
ChangesEnvironment=yes

[UninstallDelete]
Type: filesandordirs; Name: "{localappdata}\Version"

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "C:\Data\TJ\Software\Coding\Versiontool\{#MyAppExeName}"; DestDir: "{localappdata}\Version"; Flags: ignoreversion
Source: "C:\Data\TJ\Software\Coding\Versiontool\changelog.md"; DestDir: "{localappdata}\Version";Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"

[Registry]

Root: HKCU; Subkey: "Environment"; ValueType: expandsz; ValueName: "Path"; \
ValueData: "{olddata};{localappdata}\{#MyAppName}"; \
Flags: preservestringtype
