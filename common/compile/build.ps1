cd ../../Kernel 
./build.ps1
cd ../Script
./link.ps1
cd ../Shell
./build.ps1
cd ../
Copy-Item -Path ./Shell/build/Release/Shell.exe -Destination ./Kernel/build/kernel/Release/
Set-Content ./Kernel/build/kernel/Release/launch.cmd '@echo off

set home=%~dp0%
set home=%home:~0,-1%

"%home%\Shell.exe" ^
	"%home%\Kernel.dll" ^
	"..\..\..\..\Script\build\main.js" ^
	%*

set /p _=Press ^<ENTER^> to ^exit^ program... 
set ERRORLEVEL=0
'
$TargetPath = (Get-Item .).FullName + "/Kernel/build/kernel/Release"
$WshShell = New-Object -comObject WScript.Shell
$Shortcut = $WshShell.CreateShortcut("$Home\Desktop\launch.lnk")
$Shortcut.TargetPath = $TargetPath + ".\launch.cmd"
$Shortcut.WorkingDirectory = $TargetPath
$Shortcut.Save()
