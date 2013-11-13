@echo off

set stable_modname="..\Medieval Tech 2.0"

REM Guide to add copying of more files
REM
REM call :CopyDir is the function call
REM Arguments:
REM 1st argument: name of file
REM     This is a file added in the new release and should not be present in any previous releases
REM
REM 2nd argument: directory
REM     Tells which directory to copy from the stable release
REM     "none" is allowed, in which case the file from 1st argument is copied and nothing else
REM
REM 3rd argument: URL
REM     This URL will be opened if the file from 1st argument is missing both in this mod and in the stable mod
REM     Skipping 3rd argument mean no page will be opened. Used to open the page just once when a single zip file contain both Art and Sounds


REM 2.0
call :CopyDir "Movies\Victory.bik"    Art     "http://www.moddb.com/members/kailric/downloads/medieval-conquest-15-for-civ4col"
call :CopyDir "DipNALateWar.mp3"      Sounds
call :CopyDir "Medieval2.FPK"         none

REM 2.0b
call :CopyDir "Movies\silkroad.bik"   Art     "http://www.atomicgamer.com/file.php?id=104974"

REM 2.0c
call :CopyDir "Movies\tradefair.bik"  Art     "http://www.atomicgamer.com/file.php?id=105021"

pause
GOTO :EOF

:CopyDir
set TargetDir="Assets\%~2"
IF %2 == "none" (
	set TargetFile="Assets"\%1
) ELSE (
	set TargetFile=%TargetDir%\%1
)


IF NOT EXIST %TargetFile% (
	IF EXIST %stable_modname%\%TargetFile% (
		IF %2 == "none" (
			xcopy %stable_modname%\%TargetFile% %TargetFile%  /y
		) ELSE (
			xcopy %stable_modname%\%TargetDir% %TargetDir%  /e /y /i
		)
	) ELSE (
		IF [%3] NEQ [] (
			START %~3
		)
	)
)
GOTO :EOF
