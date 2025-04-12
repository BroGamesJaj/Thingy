[Setup]
AppName=Thingy
AppVersion=1.0
DefaultDirName={pf}\Thingy
DefaultGroupName=Thingy
UninstallDisplayIcon={app}\Thingy.exe
OutputBaseFilename=ThingyInstaller
SetupIconFile=assets/images/ThingyIcon.ico
DisableDirPage=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "build\bin\Windows-AMD64\Release\Thingy.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "build\bin\Windows-AMD64\Release\*.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "assets\*"; DestDir: "{app}\assets"; Flags: recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Thingy"; Filename: "{app}\bin\Thingy.exe"; IconFilename: "{app}\bin\Thingy.exe"
Name: "{userdesktop}\Thingy"; Filename: "{app}\bin\Thingy.exe"; Tasks: desktopicon
Name: "{group}\Uninstall Thingy"; Filename: "{uninstallexe}"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked