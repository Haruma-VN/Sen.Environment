cd ../../Kernel 
./build.ps1
cd ../Script
./link.ps1
cd ../Shell
./build.ps1
cd ../
Copy-Item -Path ./Shell/build/Release/Shell.exe -Destination ./Kernel/build/kernel/Release/
$string = '@echo off

set home=%~dp0%
set home=%home:~0,-1%

"%home%\Shell.exe" ^
	"%home%\Kernel.dll" ^
	"destination" ^
	%*

set /p _=Press ^<ENTER^> to ^exit^ program... 
set ERRORLEVEL=0
'
$script_target = (Get-Item .).FullName + "\Script\build\main.js"
Set-Content ./Kernel/build/kernel/Release/launch.cmd $string.Replace("destination", $script_target)
$TargetPath = (Get-Item .).FullName + "/Kernel/build/kernel/Release"
$WshShell = New-Object -comObject WScript.Shell
$Shortcut = $WshShell.CreateShortcut("$Home\Desktop\launch.lnk")
$Shortcut.TargetPath = $TargetPath + ".\launch.cmd"
$Shortcut.WorkingDirectory = $TargetPath
$Shortcut.Save()
