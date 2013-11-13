@echo off

REM mod specific setup
set modname="Medieval_Tech"
set URL="https://github.com/Nightinggale/Medieval_Tech.git"

REM general colo/git setup
set civPath="%HomePath%\Documents\My Games\Sid Meier's Civilization IV Colonization"
set gitDir="PortableGit"

set moddir="%civPath%\MODS"
set gitpath="%civPath%\%gitDir%\bin\git.exe"

echo The following message about a missing path is likely incorrect
echo (might not appear)
IF NOT EXIST %gitpath% (
	set gitpath="git.exe"
)

cd %moddir%

IF EXIST %modname% (
	cd %modname%
	%gitpath% pull
) ELSE (
	%gitpath% clone %URL% %modname%
	cd %modname%
)

call filefetch.bat
