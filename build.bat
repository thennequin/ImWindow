@echo off

if not exist "genie.exe" (
	echo Downloading genie...
	bitsadmin /transfer "genie" "https://github.com/bkaradzic/bx/raw/master/tools/bin/windows/genie.exe" "%~dp0\genie.exe"
)

genie.exe vs2013
