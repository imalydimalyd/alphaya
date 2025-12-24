cls
@echo off
setlocal enabledelayedexpansion

rem Check if game name is correct (by DeepSeek)
for %%g in (TIC_TAC_TOE GOMOKU) do (
	if "%1"=="%%g" (
		goto :execute
	)
)

echo Unknown game: %1
exit /b 1

:execute
echo Date: %date%
echo Time: %time%
echo Game: %1
echo Device: Terminal
echo;

call g++ -std=c++14 -Wall -O2 src\export\terminal.cpp -DGAME_%1 -o dist\windows_gcc\%1_GCC.exe
IF %ERRORLEVEL% EQU 0 (
	echo G++ OK
) ELSE (
	echo G++ ERROR %ERRORLEVEL%
	exit /b %ERRORLEVEL%
)

call cl /std:c++14 /w /O2 /fp:fast src\export\terminal.cpp -DGAME_%1 /Fedist\windows\%1.exe
IF %ERRORLEVEL% EQU 0 (
	echo MSVC OK
	call dist\windows\%1.exe
) ELSE (
	echo MSVC ERROR %ERRORLEVEL%
	exit /b %ERRORLEVEL%
)