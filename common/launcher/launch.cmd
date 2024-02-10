@echo off

set home=%~dp0%
set home=%home:~0,-1%

"%home%\Shell.exe" ^
	"%home%\Kernel.dll" ^
	"%home%\Script\main.js" ^
	%*

set /p _=Press ^<ENTER^> to ^exit^ program... 
set ERRORLEVEL=0
