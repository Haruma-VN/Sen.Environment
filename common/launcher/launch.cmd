@echo off

set home=%~dp0%
set home=%home:~0,-1%

"%home%\shell.exe" ^
	"%home%\kernel.dll" ^
	"%home%\script\main.js" ^
	%*

set /p _=Press ^<ENTER^> to ^exit^ program... 
set ERRORLEVEL=0
