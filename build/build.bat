@echo off

if not exist "genie.exe" (
	echo Downloading genie...
	bitsadmin /transfer "genie" "https://github.com/bkaradzic/bx/raw/master/tools/bin/windows/genie.exe" "%~dp0\genie.exe"
)

IF "%1"=="" (
	echo No argument set
	echo Exemple : build.bat vs2015
	exit 1
)

genie.exe %*
